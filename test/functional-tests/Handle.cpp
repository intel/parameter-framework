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

#include "Config.hpp"
#include "Test.hpp"
#include "Exception.hpp"
#include "TmpFile.hpp"

#include "ParameterFramework.hpp"
#include "ElementHandle.hpp"

#include <catch.hpp>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <string>
#include <list>

#include <stdlib.h>

using std::string;
using std::list;
using Bytes = std::vector<uint8_t>;

namespace parameterFramework
{

struct AllParamsPF : public ParameterFramework
{
    AllParamsPF() : ParameterFramework{getConfig()} { REQUIRE_NOTHROW(start()); }

    string getBasicParams()
    {
        string structure = R"(
            <BooleanParameter Name="bool" Description="bool"/>
            <BooleanParameter ArrayLength="2" Name="bool_array" Description="bool-array"/>

            <IntegerParameter Signed="false" Min="33" Max="123" Size="16" Name="integer"/>
            <IntegerParameter Signed="true" Min="-10" Max="10" Size="32" ArrayLength="4" Name="integer_array"/>

            <FixedPointParameter Size="32" Integral="3" Fractional="4" Name="fix_point"/>
            <FixedPointParameter Size="32" Integral="3" Fractional="4" ArrayLength="3" Name="fix_point_array"/>

            <EnumParameter Size="8" Name="enum">
                <ValuePair Literal="min"  Numerical="-128"/>
                <ValuePair Literal="five" Numerical="5"/>
                <ValuePair Literal="max"  Numerical="127"/>
            </EnumParameter>
            <EnumParameter Size="16" ArrayLength="4" Name="enum_array">
                <ValuePair Literal="eight"  Numerical="8"/>
                <ValuePair Literal="min"  Numerical="-32767"/>
            </EnumParameter>)";

        // String and bit parameter arrays are not supported
        structure += R"(
            <StringParameter MaxLength="63" Name="string"/>

            <BitParameterBlock Size="64" Name="bit_block">
                <BitParameter Pos="1"  Size="1"  Max="1"  Name="one"/>
                <BitParameter Pos="2"  Size="2"  Max="2"  Name="two"/>
                <BitParameter Pos="6"  Size="6"  Max="10" Name="six"/>
                <BitParameter Pos="16" Size="16" Max="99" Name="sixteen"/>
                <BitParameter Pos="32" Size="32" Max="4294967295" Name="thirty_two"/>
            </BitParameterBlock>
            )";
        return structure;
    }

    Config getConfig()
    {
        Config config;
        config.components = nodeDesc("ComponentType", "component_type", getBasicParams());
        config.instances =
            getBasicParams() + nodeDesc("ParameterBlock", "parameter_block", getBasicParams()) +
            nodeDesc("ParameterBlock", "parameter_block_array", getBasicParams(),
                     "ArrayLength='2'") +
            nodeDesc("Component", "component_scalar", "", "Type='component_type'") +
            nodeDesc("Component", "component_array", "", "Type='component_type' ArrayLength='2'");
        return config;
    }

    void checkStructure(const string &path, const string &expected)
    {
        CHECK_NOTHROW(checkXMLEq(ElementHandle{*this, path}.getStructureAsXML(), expected));
    }

    /** Use libxml2 to pretty format xml.
     * Equivalent of xmllint --format
     */
    static string canonicalizeXML(const string &xml)
    {
        // Parse xml
        // Might be better to specialize std::default_delete<xmlDoc>.
        std::unique_ptr<xmlDoc, void (*)(xmlDoc *)> doc{
            xmlReadMemory(xml.c_str(), (int)xml.length(), "structure.xml", nullptr,
                          XML_PARSE_NOBLANKS),
            xmlFreeDoc};
        if (doc == nullptr) {
            throw Exception{"Failed to parse document: " + xml};
        }

        // Dump it formated
        int size;

        // Need to use exception unsafe raw pointer as of libxml2 c api
        xmlChar *unsafeFormated;

        // TODO: Should use canonicalization (aka c14n).
        //       cf: http://xmlsoft.org/html/libxml-c14n.html
        //           https://en.wikipedia.org/wiki/Canonical_XML
        //       Additionally to what is listed on that page,
        //       attributes are also ordered deterministically.
        //       That would solve the workaround in the node function with pre/post attributes.
        //       Unfortunately c14n is not available in appveyor (Windows CI) libxml2 prebuild
        xmlDocDumpFormatMemoryEnc(doc.get(), &unsafeFormated, &size, "UTF-8", 1);
        std::unique_ptr<xmlChar, void (*)(void *)> formated{unsafeFormated, xmlFree};

        if (formated == nullptr) {
            throw Exception{"Could not dump xml: " + xml};
        }

        return string{(char *)formated.get()};
    }

    static void checkEq(const string &result, const string &expected)
    {
        CHECK(result == expected);

        // Pretty print the word differences with colors
        // It does not matter if it fails as the test would still fail
        // due to the above CHECK.
        if (result != expected) {
            utility::TmpFile resultFile(result);
            utility::TmpFile expectedFile(expected);
            string command = "git --no-pager diff --word-diff-regex='[^ <>]+'"
                             "                    --color --no-index --exit-code " +
                             resultFile.getPath() + ' ' + expectedFile.getPath();

            // `system` return -1 or 127 on failure, the command error code otherwise
            // `git diff` return 1 if the files are the different (thanks to --exit-code)
            auto status = system(command.c_str());
#ifdef WIFEXITED // Posix platform
            bool success = WIFEXITED(status) and WEXITSTATUS(status) == 1;
#else
            bool success = status == 1;
#endif
            if (not success) {
                WARN("Warning: Failed to pretty-print the difference between "
                     "actual and expected results with `git diff'");
            }
        }
    }

    static void checkXMLEq(const string &result, const string &expected)
    {
        checkEq(canonicalizeXML(result), canonicalizeXML(expected));
    }

    static string node(string tag, string name, string content, string attributes = "",
                       string postAttributes = "")
    {
        return "<" + tag + " " + attributes + " Name='" + name + "' " + postAttributes + ">" +
               content + "</" + tag + ">";
    }
    /** Node with a description.
     * @param[in] maybeDescription If nullptr, description will be generated from the name
     *                             Otherwise, the description.
     */
    static string nodeDesc(string tag, string name, string content, string attributes = "",
                           const char *maybeDescription = nullptr)
    {
        string description = "description_" + name;
        if (maybeDescription != nullptr) {
            description = maybeDescription;
        }
        return node(tag, name, content, attributes, "Description='" + description + "'");
    }

    static string rootNode(string name, string attributes, string content)
    {
        return '<' + name + ' ' + attributes + '>' + content + "</" + name + '>';
    }
};

SCENARIO_METHOD(AllParamsPF, "Export boolean", "[handler][structure][xml]")
{
    string expected = rootNode("BooleanParameter", "Name='bool' Description='bool'", "");
    checkStructure("/test/test/bool", expected);
}

SCENARIO_METHOD(AllParamsPF, "Export component", "[handler][structure][xml]")
{
    string expected = rootNode("ParameterBlock", "Name='component_scalar' "
                                                 "Description='description_component_scalar'",
                               getBasicParams());
    checkStructure("/test/test/component_scalar", expected);
}

SCENARIO_METHOD(AllParamsPF, "Export component array", "[handler][structure][xml]")
{
    string expected = rootNode(
        "ParameterBlock", "Name='component_array' Description='description_component_array'",
        nodeDesc("ParameterBlock", "0", getBasicParams(), "", "description_component_array") +
            nodeDesc("ParameterBlock", "1", getBasicParams(), "", "description_component_array"));
    checkStructure("/test/test/component_array", expected);
}

SCENARIO_METHOD(AllParamsPF, "Export all parameters", "[handler][structure][xml]")
{
    string paramExpected = getBasicParams() +
                           nodeDesc("ParameterBlock", "parameter_block", getBasicParams()) +
                           nodeDesc("ParameterBlock", "parameter_block_array",
                                    nodeDesc("ParameterBlock", "0", getBasicParams(), "",
                                             // description is inherited from array
                                             "description_parameter_block_array") +
                                        nodeDesc("ParameterBlock", "1", getBasicParams(), "",
                                                 "description_parameter_block_array")) +
                           // Components should be exported as parameterBlock
                           nodeDesc("ParameterBlock", "component_scalar", getBasicParams()) +
                           nodeDesc("ParameterBlock", "component_array",
                                    nodeDesc("ParameterBlock", "0", getBasicParams(), "",
                                             // description is inherited from array
                                             "description_component_array") +
                                        nodeDesc("ParameterBlock", "1", getBasicParams(), "",
                                                 "description_component_array"));

    WHEN ("Exporting subsystem") {
        string expected = rootNode("Subsystem", "Name='test'", paramExpected);
        checkStructure("/test/test", expected);
    }

    WHEN ("Exporting systemClass") {
        string expected = rootNode("SystemClass", "Name='test'",
                                   "<Subsystem Name='test'>" + paramExpected + "</Subsystem>");

        // Awkwardly, the root and its first child are the same element
        checkStructure("/test", expected);
        checkStructure("/", expected);
    }
}

struct SettingsTestPF : public AllParamsPF
{
    static string parameterBlockNode(string name, string settings)
    {
        return node("ParameterBlock", name, settings);
    };
    static string mkBasicSettings(string settings, string name)
    {
        return rootNode("ParameterBlock", "Name='" + name + "'", settings);
    }

    static string fullXMLSettings(const string &basicSettings)
    {
        string settings = basicSettings;
        settings +=
            parameterBlockNode("parameter_block", settings) +
            parameterBlockNode("parameter_block_array", parameterBlockNode("0", settings) +
                                                            parameterBlockNode("1", settings)) +
            parameterBlockNode("component_scalar", settings) +
            parameterBlockNode("component_array", parameterBlockNode("0", settings) +
                                                      parameterBlockNode("1", settings));

        return rootNode("SystemClass", "Name='test'", node("Subsystem", "test", settings, ""));
    }

    static string fullBytesSettings(const string &basicSettings)
    {
        string fullSettings;
        // We have the "basic params" repeated 7 times across the test
        // structure
        for (size_t i = 0; i < 7; ++i) {
            fullSettings += basicSettings;
        }
        return fullSettings;
    }

    /** Print Bytes as string separated hexadecimal number. */
    static string showBytes(const Bytes &bytes)
    {
        using namespace std;
        ostringstream ss;
        ss.exceptions(ostream::badbit | ostream::failbit);
        for (auto byte : bytes) {
            ss << hex << setw(2) << setfill('0') << int{byte} << ' ';
        }
        return ss.str();
    }

    static Bytes readBytes(const string &strBytes)
    {
        using namespace std;
        istringstream ss{strBytes};
        ss.exceptions(istream::badbit | istream::failbit);
        Bytes bytes(strBytes.size() / 3);

        for (auto &byte : bytes) {
            uint16_t notCharByte;
            ss >> hex >> setw(2) >> notCharByte;
            byte = static_cast<char>(notCharByte);
        }
        return bytes;
    }

    static void checkBytesEq(const Bytes &result, const string &expect)
    {
        checkEq(showBytes(result), expect);
    }
    static void checkBytesEq(const Bytes &result, const Bytes &expect)
    {
        checkEq(showBytes(result), showBytes(expect));
    }
};

static const char *defaultBasicSettingsXML = R"(
      <BooleanParameter Name="bool">0</BooleanParameter>
      <BooleanParameter Name="bool_array">0 0</BooleanParameter>
      <IntegerParameter Name="integer">33</IntegerParameter>
      <IntegerParameter Name="integer_array">-10 -10 -10 -10</IntegerParameter>
      <FixedPointParameter Name="fix_point">0.0000</FixedPointParameter>
      <FixedPointParameter Name="fix_point_array">0.0000 0.0000 0.0000</FixedPointParameter>
      <EnumParameter Name="enum">min</EnumParameter>
      <EnumParameter Name="enum_array">eight eight eight eight</EnumParameter>
      <StringParameter Name="string"></StringParameter>
      <BitParameterBlock Name="bit_block">
        <BitParameter Name="one">0</BitParameter>
        <BitParameter Name="two">0</BitParameter>
        <BitParameter Name="six">0</BitParameter>
        <BitParameter Name="sixteen">0</BitParameter>
        <BitParameter Name="thirty_two">0</BitParameter>
      </BitParameterBlock>
)";

static const char *testBasicSettingsXML = R"(
      <BooleanParameter Name="bool">1</BooleanParameter>
      <BooleanParameter Name="bool_array">0 1</BooleanParameter>
      <IntegerParameter Name="integer">100</IntegerParameter>
      <IntegerParameter Name="integer_array">-10 0 8 10</IntegerParameter>
      <FixedPointParameter Name="fix_point">2.2500</FixedPointParameter>
      <FixedPointParameter Name="fix_point_array">7.1250 0.6875 -1.0000</FixedPointParameter>
      <EnumParameter Name="enum">five</EnumParameter>
      <EnumParameter Name="enum_array">eight min eight min</EnumParameter>
      <StringParameter Name="string">A string of 32 character.@@@@@@@</StringParameter>
      <BitParameterBlock Name="bit_block">
        <BitParameter Name="one">1</BitParameter>
        <BitParameter Name="two">2</BitParameter>
        <BitParameter Name="six">10</BitParameter>
        <BitParameter Name="sixteen">72</BitParameter>
        <BitParameter Name="thirty_two">4294967295</BitParameter>
      </BitParameterBlock>
)";
static const char *testRawHexBasicSettingsXML = R"(
      <BooleanParameter Name="bool">0x1</BooleanParameter>
      <BooleanParameter Name="bool_array">0x0 0x1</BooleanParameter>
      <IntegerParameter Name="integer">0x0064</IntegerParameter>
      <IntegerParameter Name="integer_array">0xFFFFFFF6 0x00000000 0x00000008 0x0000000A</IntegerParameter>
      <FixedPointParameter ValueSpace="Raw" Name="fix_point">0x24000000</FixedPointParameter>
      <FixedPointParameter ValueSpace="Raw" Name="fix_point_array">0x72000000 0x0B000000 0xF0000000</FixedPointParameter>
      <EnumParameter Name="enum">five</EnumParameter>
      <EnumParameter Name="enum_array">eight min eight min</EnumParameter>
      <StringParameter Name="string">A string of 32 character.@@@@@@@</StringParameter>
      <BitParameterBlock Name="bit_block">
        <BitParameter Name="one">0x1</BitParameter>
        <BitParameter Name="two">0x2</BitParameter>
        <BitParameter Name="six">0xA</BitParameter>
        <BitParameter Name="sixteen">0x48</BitParameter>
        <BitParameter Name="thirty_two">0xFFFFFFFF</BitParameter>
      </BitParameterBlock>
)";

SCENARIO_METHOD(SettingsTestPF, "Export and import XML settings", "[handler][settings][xml]")
{
    WHEN ("Exporting root XML") {
        auto getAsXML = [this](string path) { return ElementHandle(*this, path).getAsXML(); };
        CHECK(getAsXML("/") == getAsXML("/test"));
        checkXMLEq(getAsXML("/"), fullXMLSettings(defaultBasicSettingsXML));
    }

    ElementHandle basicParams(*this, "/test/test/parameter_block");
    WHEN ("Exporting basic parameter XML") {
        checkXMLEq(basicParams.getAsXML(),
                   mkBasicSettings(defaultBasicSettingsXML, "parameter_block"));
    }
    string testSettings = mkBasicSettings(testBasicSettingsXML, "parameter_block");
    string rawTestSettings = mkBasicSettings(testRawHexBasicSettingsXML, "parameter_block");

    auto checkExport = [&] {
        THEN ("Exported settings should be the ones imported") {
            checkXMLEq(basicParams.getAsXML(), testSettings);
        }
        THEN ("Exported raw settings should be the ones imported") {
            setRawValueSpace(true);
            setHexOutputFormat(true);
            checkXMLEq(basicParams.getAsXML(), rawTestSettings);
        }
    };
    WHEN ("Importing basic parameter XML") {
        CHECK_NOTHROW(basicParams.setAsXML(testSettings));
        checkExport();
    }
    WHEN ("Importing raw basic parameter XML") {
        CHECK_NOTHROW(basicParams.setAsXML(rawTestSettings));
        checkExport();
    }
}

static const string defaultBasicSettingsBytes =
    "00 00 00 21 00 f6 ff ff ff f6 ff ff ff f6 ff ff ff f6 ff ff ff 00 00 00 00 "
    "00 00 00 00 00 00 00 00 00 00 00 00 80 08 00 08 00 08 00 08 00 00 00 00 00 00 "
    "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
    "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
    "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ";

static const string testBasicSettingsBytes =
    "01 00 01 64 00 f6 ff ff ff 00 00 00 00 08 00 00 00 0a 00 00 00 00 00 00 24 "
    "00 00 00 72 00 00 00 0b 00 00 00 f0 05 08 00 01 80 08 00 01 80 41 20 73 74 72 "
    "69 6e 67 20 6f 66 20 33 32 20 63 68 61 72 61 63 74 65 72 2e 40 40 40 40 40 40 "
    "40 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
    "00 00 00 00 00 00 00 8a 02 48 00 ff ff ff ff ";

SCENARIO_METHOD(SettingsTestPF, "Bijection of binary show and read", "[identity][test]")
{
    CHECK(showBytes(readBytes(testBasicSettingsBytes)) == testBasicSettingsBytes);
}

SCENARIO_METHOD(SettingsTestPF, "Export and import root binary settings",
                "[handler][settings][bytes]")
{
    ElementHandle root(*this, "/");
    ElementHandle systemClass(*this, "/");

    THEN ("Root and system class should export the same binary") {
        checkBytesEq(root.getAsBytes(), systemClass.getAsBytes());
    }
    WHEN ("Exporting root binary") {
        checkBytesEq(root.getAsBytes(), fullBytesSettings(defaultBasicSettingsBytes));
    }
    WHEN ("Importing root binary") {
        string rootTestSettings = fullBytesSettings(testBasicSettingsBytes);
        REQUIRE_NOTHROW(root.setAsBytes(readBytes(rootTestSettings)));
        THEN ("Exported settings should be the ones imported") {
            checkBytesEq(root.getAsBytes(), rootTestSettings);
        }
    }
}

SCENARIO_METHOD(SettingsTestPF, "Export and import basic binary settings",
                "[handler][settings][bytes]")
{
    ElementHandle basicParams(*this, "/test/test/parameter_block");
    WHEN ("Exporting basic parameter binary") {
        checkBytesEq(basicParams.getAsBytes(), defaultBasicSettingsBytes);
    }
    WHEN ("Importing basic parameter binary") {
        REQUIRE_NOTHROW(basicParams.setAsBytes(readBytes(testBasicSettingsBytes)));
        THEN ("Exported settings should be the ones imported") {
            checkBytesEq(basicParams.getAsBytes(), testBasicSettingsBytes);
        }
    }
}

SCENARIO_METHOD(SettingsTestPF, "Export and import array binary settings",
                "[handler][settings][bytes]")
{
    ElementHandle array(*this, "/test/test/parameter_block_array");
    ElementHandle elem0(*this, "/test/test/parameter_block_array/0");
    WHEN ("Importing one array element") {
        REQUIRE_NOTHROW(elem0.setAsBytes(readBytes(testBasicSettingsBytes)));
        THEN ("The other element should not have changed") {
            checkBytesEq(array.getAsBytes(), testBasicSettingsBytes + defaultBasicSettingsBytes);
        }
    }
}

SCENARIO_METHOD(SettingsTestPF, "Import root in one format, export in an other",
                "[handler][settings][bytes][xml]")
{
    ElementHandle root(*this, "/test");
    string rootBytesSettings = fullBytesSettings(testBasicSettingsBytes);
    string rootXMLSettings = fullXMLSettings(testBasicSettingsXML);

    WHEN ("Importing root binary") {
        REQUIRE_NOTHROW(root.setAsBytes(readBytes(rootBytesSettings)));
        THEN ("Exported XML settings should be the ones imported") {
            checkXMLEq(root.getAsXML(), rootXMLSettings);
        }
    }

    WHEN ("Importing root XML") {
        REQUIRE_NOTHROW(root.setAsXML(rootXMLSettings));
        THEN ("Exported bytes settings should be the ones imported") {
            checkBytesEq(root.getAsBytes(), rootBytesSettings);
        }
    }
}

SCENARIO_METHOD(SettingsTestPF, "Import basic params in one format, export in an other",
                "[handler][settings][bytes][xml]")
{
    ElementHandle basicParams(*this, "/test/test/parameter_block_array/0");
    string basicXMLSettings = mkBasicSettings(testBasicSettingsXML, "0");

    WHEN ("Importing basic parameters binary") {
        REQUIRE_NOTHROW(basicParams.setAsBytes(readBytes(testBasicSettingsBytes)));
        THEN ("Exported XML settings should be the ones imported") {
            checkXMLEq(basicParams.getAsXML(), basicXMLSettings);
        }
    }

    WHEN ("Importing basic parameters XML") {
        REQUIRE_NOTHROW(basicParams.setAsXML(basicXMLSettings));
        THEN ("Exported bytes settings should be the ones imported") {
            checkBytesEq(basicParams.getAsBytes(), testBasicSettingsBytes);
        }
    }
}

struct MappingPF : public ParameterFramework
{
    MappingPF() : ParameterFramework{getConfig()} { REQUIRE_NOTHROW(start()); }

    struct TestVector
    {
        string path;
        string humanReadable;
        list<string> valid;
        list<string> invalid;
    };

    list<TestVector> testVectors = {
        // clang-format off
        {"/test/test",
            {"rootK:rootV"},
            {"root"},
            {"param", "type", "instance", "derived"}},
        {"/test/test/param",
            {"rootK:rootV, paramK:paramV"},
            {"root", "param"},
            {"type", "derived", "instance"}},
        {"/test/test/component",
            {"rootK:rootV, typeK:typeV, derivedK:derivedV, instanceK:instanceV"},
            {"root", "type", "derived", "instance"},
            {"param"}}
        // clang-format on
    };

    Config getConfig()
    {
        Config config;
        config.subsystemMapping = "rootK:rootV";
        config.components = "<ComponentType   Name='componentType' Mapping='typeK:typeV'        />"
                            "<ComponentType   Extends='componentType' Name='derivedComponentType' "
                            "Mapping='derivedK:derivedV' />";
        config.instances = "<BooleanParameter Name='param'         Mapping='paramK:paramV'      />"
                           "<Component        Name='component'     Mapping='instanceK:instanceV'  "
                           "           Type='derivedComponentType'                              />";
        return config;
    }
};

SCENARIO_METHOD(MappingPF, "showMapping command", "[mapping]")
{
    auto cmdHandler = std::unique_ptr<CommandHandlerInterface>(createCommandHandler());

    for (auto &testVector : testVectors) {
        string output;
        CHECK(cmdHandler->process("showMapping", {testVector.path}, output));
        CHECK(output == testVector.humanReadable);
    }
}

SCENARIO_METHOD(MappingPF, "Mapping handle access", "[handler][mapping]")
{
    GIVEN ("A PF with mappings") {
        for (auto &test : testVectors) {
            GIVEN ("An element handle of " + test.path) {
                ElementHandle handle(*this, test.path);

                for (auto &valid : test.valid) {
                    THEN ("The following mapping should exist: " + valid) {
                        CHECK(handle.getMappingData(valid + "K") == valid + "V");
                    }
                }

                for (auto &invalid : test.invalid) {
                    THEN ("The following mapping should not exist: " + invalid) {
                        CHECK_THROWS_AS(handle.getMappingData(invalid + "K"), Exception);
                    }
                }
            }
        }
    }
}

SCENARIO_METHOD(SettingsTestPF, "Handle Get/Set as various kinds", "[handler][dynamic]")
{
    ElementHandle intScalar(*this, "/test/test/parameter_block/integer");
    WHEN ("Setting a scalar integer") {
        WHEN ("As an array") {
            THEN ("It should fail") {
                CHECK_THROWS(intScalar.setAsIntegerArray({0, 0}));
            }
        }
        WHEN ("As a scalalar") {
            THEN ("It should succeed") {
                uint32_t expected = 111;
                CHECK_NOTHROW(intScalar.setAsInteger(expected));
                AND_THEN ("Getting it back should give the same value") {
                    uint32_t back = 42;
                    CHECK_NOTHROW(intScalar.getAsInteger(back));
                    CHECK(back == expected);
                }
            }
        }
    }

    ElementHandle intArray(*this, "/test/test/parameter_block/integer_array");
    WHEN ("Setting a array integer") {
        WHEN ("As a scalar") {
            THEN ("It should fail") {
                CHECK_THROWS(intArray.setAsSignedInteger(0));
            }
        }
        WHEN ("As a integer") {
            THEN ("It should succeed") {
                const std::vector<int32_t> expected = {-9, 8, -7, 6};
                CHECK_NOTHROW(intArray.setAsSignedIntegerArray(expected));
                AND_THEN ("Getting it back should give the same value") {
                    std::vector<int32_t> back = {-42, 42, 43, -43};
                    CHECK_NOTHROW(intArray.getAsSignedIntegerArray(back));
                    CHECK(back == expected);
                }
            }
        }
    }
}
} // namespace parameterFramework
