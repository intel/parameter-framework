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

using std::string;
using Bytes = std::vector<uint8_t>;

namespace parameterFramework
{

struct AllParamsPF : public ParameterFramework
{
    AllParamsPF() : ParameterFramework{getConfig()}
    {
        REQUIRE_NOTHROW(start());
    }

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
        config.instances = getBasicParams() +
                           nodeDesc("ParameterBlock", "parameter_block",getBasicParams()) +
                           nodeDesc("ParameterBlock", "parameter_block_array",
                                    getBasicParams(), "ArrayLength='2'") +
                           nodeDesc("Component", "component_scalar", "", "Type='component_type'") +
                           // Test that ArrayLength have no effect on components
                           nodeDesc("Component", "component_array", "",
                                    "Type='component_type' ArrayLength='2'");
        return config;
   }

    void checkStructure(const string &path, const string &expected)
    {
        CHECK_NOTHROW(checkXMLEq(ElementHandle{*this, path}.getStructureAsXML(),
                      expected));
    }

    /** Use libxml2 to pretty format xml.
     * Equivalent of xmllint --format
     */
    static string canonicalizeXML(const string &xml)
    {
        // Parse xml
        // Might be better to specialize std::default_delete<xmlDoc>.
        std::unique_ptr<xmlDoc, void(*)(xmlDoc *)> doc{
                xmlReadMemory(xml.c_str(), (int)xml.length(), "structure.xml", nullptr, XML_PARSE_NOBLANKS),
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
        xmlDocDumpFormatMemoryEnc(doc.get(), &unsafeFormated, &size, "UTF-8",1);
        std::unique_ptr<xmlChar, void(*)(void *)> formated{unsafeFormated, xmlFree};

        if (formated == nullptr) {
            throw Exception{"Could not dump xml: " + xml};
        }

        return string{(char *)formated.get()};
    }

    static void checkEq(const string &result, const string& expected)
    {
        CHECK(result == expected);

        // Pretty print the word differences with colors
        // It does not matter if it fails as the test would still fail
        // due to the above CHECK.
        if (result != expected) {
            utility::TmpFile resultFile(result);
            utility::TmpFile expectedFile(expected);
            auto gitCommand = "git --no-pager diff --word-diff-regex='[^ <>]+' --color --no-index ";
            system((gitCommand + resultFile.getPath() + ' ' + expectedFile.getPath()).c_str());
        }
    }

    static void checkXMLEq(const string &result, const string &expected)
    {
        checkEq(canonicalizeXML(result), canonicalizeXML(expected));
    }

    static string node(string tag, string name, string content,
                       string attributes = "", string postAttributes = "")
    {
        return "<" + tag + " " + attributes + " Name='" + name + "' " + postAttributes + ">" +
                content +
               "</" + tag +">";
    }
    /** Node with a description.
     * @param[in] maybeDescription If nullptr, description will be generated from the name
     *                             Otherwise, the description.
     */
    static string nodeDesc(string tag, string name, string content, string attributes = "",
                           const char *maybeDescription=nullptr)
    {
        string description = "description_" + name;
        if (maybeDescription != nullptr) {
            description = maybeDescription;
        }
        return node(tag, name, content, attributes, "Description='" + description + "'");
    }

    static string rootNode(string name, string attributes, string content)
    {
        return '<' + name + " xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'"
                            " xsi:noNamespaceSchemaLocation='" + name + ".xsd'" +
                             ' ' + attributes + '>' +
                content + "</" + name + '>';
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
    string expected = rootNode("ParameterBlock", "Name='component_array' "
                                                 "Description='description_component_array'",
                               // component array are the same as non array for now
                               getBasicParams());
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
                           nodeDesc("ParameterBlock", "component_array", getBasicParams());

    WHEN("Exporting subsystem") {
        string expected = rootNode("Subsystem", "Name='test'", paramExpected);
        checkStructure("/test/test", expected);
    }


    WHEN("Exporting systemClass") {
        string expected = rootNode("SystemClass", "Name='test'",
                                       "<Subsystem Name='test'>" + paramExpected + "</Subsystem>");

        // Awkwardly, the root and its first child are the same element
        checkStructure("/test", expected);
        checkStructure("/", expected);
    }
}

} // namespace parameterFramework
