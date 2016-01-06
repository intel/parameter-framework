/*
 * Copyright (c) 2015, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <ParameterMgrFullConnector.h>
#include <Tokenizer.h>
#include <Utility.h>

#include <iostream>
#include <sstream>
#include <memory>
#include <string>
#include <limits>
#include <numeric>
#include <algorithm>
#include <stdexcept>

using std::string;

class MyLogger final : public CParameterMgrFullConnector::ILogger
{
public:
    void info(const std::string &log) override { std::cerr << "Info: " << log << std::endl; }

    void warning(const std::string &log) override { std::cerr << "Warning: " << log << std::endl; }
};

class XmlGenerator
{
public:
    using Exception = std::runtime_error;

    XmlGenerator(const string &toplevelConfig, bool validate, bool verbose, string schemasDir)
        : mConnector(toplevelConfig), mCommandHandler(mConnector.createCommandHandler())
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

    /** Reads each line of the input stream and takes an action accordingly
     *
     * Returns when the input stream reaches end of file
     *
     * The commands are the usual PF tunning commands and some additional specials.
     * Special commands:
     *  - `createSelectionCriterion inclusive|exclusive <name> <value> [value, ...]`
     *    Create a criterion with the given properties.
     *  - `start` start the Parameter Framework. All criteria must have been created.
     *
     * @param[in] input The input stream to read from
     *
     * @return the number of error that occurred
     */
    size_t parse(std::istream &input);

    /** Check for elements belonging to several domains
     *
     * Prints conflicting elements, if any, on the error output.
     *
     * @returns true if there are conflicting elements, false otherwise
     */
    bool conflictingElements();

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
        throw Exception("Not enough arguments to criterion creation request");
    }

    auto inclusiveness = tokens.front() == "inclusive";
    tokens.erase(begin(tokens));

    auto name = tokens.front();
    tokens.erase(begin(tokens));

    auto criterionType = mConnector.createSelectionCriterionType(inclusiveness);
    if (criterionType == nullptr) {
        throw Exception("Failed to create an " + string(inclusiveness ? "inclusive" : "exclusive") +
                        " criterion type");
    }

    int index = 0;
    for (const auto &literalValue : tokens) {
        // inclusive criteria are bitfields
        int numericalValue = inclusiveness ? 1 << index : index;
        string error;
        bool success = criterionType->addValuePair(numericalValue, literalValue, error);

        if (not success) {
            std::ostringstream message;
            message << "Valuepair (" << numericalValue << ", '" << literalValue
                    << "') rejected for " << name << ": " << error;
            throw Exception(message.str());
        }
        index++;
    }

    // We don't need to keep a reference to the criterion - no need to store
    // the returned pointer.
    if (mConnector.createSelectionCriterion(name, criterionType) == nullptr) {
        throw Exception("Failed to create criterion '" + name + "'");
    }
}

size_t XmlGenerator::parse(std::istream &input)
{
    string line;
    size_t errorNb = 0;
    while (not input.eof()) {
        std::getline(std::cin, line);

        auto tokens = Tokenizer(line, string(1, '\0'), false).split();
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
                errorNb++;

                std::cerr << accumulate(begin(tokens), end(tokens),
                                        "Failed to executing command: `" + command + "'",
                                        [](string l, string r) { return l + " `" + r + "'"; })
                          << std::endl
                          << output << std::endl;
            }
        }
    }
    return errorNb;
}

bool XmlGenerator::conflictingElements()
{
    string conflicting;
    if (not mCommandHandler->process("listConflictingElements", {}, conflicting)) {
        // Should not happen
        throw Exception("Failed to list conflicting elements");
    }

    if (not conflicting.empty()) {
        std::cerr << "There are conflicting elements:" << std::endl << conflicting;
        return true;
    }

    return false;
}

void XmlGenerator::start()
{
    string error;
    if (not mConnector.start(error)) {
        throw Exception("Start failed: " + error);
    }

    error.clear();
    // Switch to tunning mode as the tunning commands
    // are the only commands possible with this connector.
    if (not mConnector.setTuningMode(true, error)) {
        throw Exception("Failed to turn tuning mode on: " + error);
    }
}

void XmlGenerator::exportDomains(std::ostream &output)
{
    string error;
    string domains;
    if (not mConnector.exportDomainsXml(domains, true, false, error)) {
        throw Exception("Export failed: " + error);
    } else {
        output << domains;
    }
}

static const char *usage =
    R"(Usage: domainGeneratorConnector <top-level config> <verbose> <validate> <path>

 <verbose>       'verbose': verbose, else: terse
 <validate>      'validate': validate, else: don't validate
 <path>          path to the schemas' directory

All arguments are mandatory. If no validation is required,
the path to the schemas can be an empty string.

Exit with the number of (recoverable or not error) that occured.

This program is not intended to be used standalone but rather called through
domainGenerator.py)";

/** On linux at least, a program can not exit with a value greater than 255.
 * @return min(code, 255);
 */
template <class T>
static inline int normalizeExitCode(T code)
{
    return int(std::min<T>(code, std::numeric_limits<uint8_t>::max()));
}

int main(int argc, char *argv[])
{
    using std::endl;

    if (argc <= 4) {
        std::cerr << usage << std::endl;
        return 1;
    }

    string toplevelConfig = argv[1];
    bool verbose = string(argv[2]) == "verbose";
    bool validate = string(argv[3]) == "validate";
    string schemasDir = argv[4];

    if (verbose) {
        std::cerr << "Domain generator config:" << endl
                  << "    toplevelConfig=" << toplevelConfig << endl
                  << "    verbose=" << verbose << endl
                  << "    validate=" << validate << endl
                  << "    schemasDir=" << schemasDir << endl;
    }

    try {
        XmlGenerator xmlGenerator(toplevelConfig, validate, verbose, schemasDir);
        auto errorNb = xmlGenerator.parse(std::cin);
        if (xmlGenerator.conflictingElements()) {
            errorNb++;
        }
        xmlGenerator.exportDomains(std::cout);

        return normalizeExitCode(errorNb);
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
