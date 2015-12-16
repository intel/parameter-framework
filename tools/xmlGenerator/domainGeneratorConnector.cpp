#include <ParameterMgrFullConnector.h>
#include <Tokenizer.h>

#include <iostream>
#include <sstream>
#include <memory>
#include <string>
#include <stdexcept>

using std::string;
using std::runtime_error;

class MyLogger final : public CParameterMgrFullConnector::ILogger
{
public:
    void info(const std::string &log) override { std::cerr << "Info: " << log << std::endl; }

    void warning(const std::string &log) override { std::cerr << "Warning: " << log << std::endl; }
};

class XmlGenerator
{
public:
    XmlGenerator(const string &toplevelConfig, bool validate, bool verbose, string schemasDir)
        : mConnector(toplevelConfig),
          mCommandHandler(mConnector.createCommandHandler())
    {
        if (verbose) {
            mLogger.reset(new MyLogger);
            mConnector.setLogger(mLogger.get());
        }

        mConnector.setSchemaUri(schemasDir);
        mConnector.setValidateSchemasOnStart(validate);

        // Disable irrelevant failure conditions
        mConnector.setFailureOnMissingSubsystem(false);
        mConnector.setFailureOnFailedSettingsLoad(false);

        // Disable the remote interface because we don't need it and it might
        // get in the way (e.g. the port is already in use)
        mConnector.setForceNoRemoteInterface(true);
    }

    /** Read each line of the input stream and takes an action accordingly
     *
     * Returns when the input stream reaches end of file
     *
     * @param[in] input The input stream to read from
     */
    void parse(std::istream &input);

    /** Prints the Parameter Framework's instance configuration
     *
     * @param[out] output The stream to which output the configuration
     */
    void exportDomains(std::ostream &output);

private:
    void addCriteria(std::vector<string> &tokens);
    void start();

    CParameterMgrFullConnector mConnector;
    std::unique_ptr<MyLogger> mLogger;
    std::unique_ptr<CommandHandlerInterface> mCommandHandler;
};

void XmlGenerator::addCriteria(std::vector<string> &tokens)
{
    if (tokens.size() < 3) {
        throw runtime_error("Not enough arguments to criterion creation request");
    }

    auto inclusiveness = tokens.front() == "inclusive";
    tokens.erase(begin(tokens));

    auto name = tokens.front();
    tokens.erase(begin(tokens));

    auto criterionType = mConnector.createSelectionCriterionType(inclusiveness);
    if (criterionType == nullptr) {
        throw runtime_error("Failed to create an " +
                        string(inclusiveness ? "inclusive" : "exclusive") +
                        " criterion type");
    }

    int index = 0;
    for (const auto &literalValue : tokens) {
        // inclusive criteria are bitfields
        int numericalCriterionValue = inclusiveness ? 1 << index : index;
        string error;
        bool success = criterionType->addValuePair(numericalCriterionValue, literalValue, error);

        if (not success) {
            std::ostringstream message;
            message << "Valuepair '" << numericalCriterionValue << "/" << literalValue
                    << " rejected for " << name << ". (" << error << ")";
            throw runtime_error(message.str());
        }
        index++;
    }

    // We don't need to keep a reference to the criterion - no need to store
    // the returned pointer.
    if (mConnector.createSelectionCriterion(name, criterionType) == nullptr) {
        throw runtime_error("Failed to create criterion '" + name + "'");
    }
}

void XmlGenerator::parse(std::istream &input)
{
    // Read each command from the input
    // if it is a "createSelectionCriterion" command, apply special
    // treatment to handle it ourselves instead of passing the command to the
    // command handler.
    // Such lines will look like:
    //     createSelectionCriterion inclusive|exclusive <name> <value> [value, ...]
    // If it is a "start", it means that the caller has finished creating the
    // criteria and the Parameter Framework must be started.
    string line;
    while (not input.eof()) {
        std::getline(std::cin, line);

        auto tokens = Tokenizer(line, string(1, '\0')).split();
        if (tokens.empty()) {
            continue;
        }
        auto command = tokens.front();
        tokens.erase(begin(tokens)); // drop the command name

        if (command == "createSelectionCriterion") {
            addCriteria(tokens);
        } else if (command == "start") {
            start();
        } else {
            string output;
            if (not mCommandHandler->process(command, tokens, output)) {
                std::cerr << output << std::endl;
            }
        }
    }
}

void XmlGenerator::start()
{
    string error;
    if (not mConnector.start(error)) {
        throw runtime_error("Start failed: " + error);
    }

    error.clear();
    if (not mConnector.setTuningMode(true, error)) {
        throw runtime_error("Failed to turn tuning mode on: " + error);
    }
}

void XmlGenerator::exportDomains(std::ostream &output)
{
    string error;
    string domains;
    if (not mConnector.exportDomainsXml(domains, true, false, error)) {
        throw runtime_error("Export failed: " + error);
    } else {
        output << domains;
    }
}

void usage()
{
    std::cerr << "Usage:" << std::endl;
    std::cerr << "\t" << "domainGeneratorConnector"
              << " <top-level config>"
                 " <verbose ('1': verbose, else: terse)>"
                 " <validate ('1': validate, else: don't validate)>"
                 " <path to the schemas' directory>" << std::endl;
    std::cerr << "All arguments are mandatory. If no validation is required, the path to the"
                 " schemas can be an empty string." << std::endl;
    std::cerr << "This program is not intended to be used standalone but rather called through"
                 " domainGenerator.py" << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc <= 4) {
        usage();
        return 1;
    }

    string toplevelConfig(argv[1]);
    bool verbose = string(argv[2]) == "1";
    bool validate = string(argv[3]) == "1";
    string schemasDir(argv[4]);

    try {
        XmlGenerator xmlGenerator(toplevelConfig, validate, verbose, schemasDir);
        xmlGenerator.parse(std::cin);
        // TODO: add a check for conflicting elements
        xmlGenerator.exportDomains(std::cout);
    } catch (runtime_error e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
