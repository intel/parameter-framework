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

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main()
#include <catch.hpp>

/** Ubuntu keeps using an old catch version where "Method" version of BDD Macros are
 * not available. This macro adds this functionality in case that catch version is too old.
 */
#ifndef SCENARIO_METHOD
#define SCENARIO_METHOD(className, name, tags) \
    INTERNAL_CATCH_TEST_CASE_METHOD(className, "Scenario: " name, tags)
#endif

#include <xmlserializer/Attribute.h>
#include <xmlserializer/Node.h>
#include <xmlserializer/Serializer.h>
#include <xmlserializer/Deserializer.h>

#include <typeinfo>
#include <sstream>
#include <string>
#include <limits>
#include <stdexcept>

using namespace core::xml::binding;

template <typename T>
struct VariableHolder
{
    T get() { return value; }
    void set(T newValue){ value = newValue; }

    T value;
};

template <typename... TypeList>
class NumericalAttributesTest
{
public:
    void operator()() {}
};

template <typename T, typename... TypeList>
class NumericalAttributesTest<T, TypeList...> : NumericalAttributesTest<TypeList...>
{
public:
    void operator()()
    {
        numericalAttributeTest();

        // Base class call
        NumericalAttributesTest<TypeList...>::operator ()();
    }

private:

    void getSetValueTest(T value, Attribute &attribute, VariableHolder<T> &holder)
    {
        WHEN("[" + std::string(typeid(T).name()) + "] Setting a value")
        {
            attribute.set(std::to_string(value));
            THEN("[" + std::string(typeid(T).name()) + "] The holder should contains this value")
            {
              CHECK(holder.value == value);
            }
            WHEN("[" + std::string(typeid(T).name()) + "] Retrieving the value as string")
            {
                THEN("[" + std::string(typeid(T).name()) +
                        "] It should match the string which represents the value set")
                {
                  CHECK(attribute.get() == std::to_string(holder.value));
                }
            }
        }
    }

    void testLimits(Attribute &attribute, VariableHolder<T> &holder)
    {
        WHEN("[" + std::string(typeid(T).name()) + "] Setting the maximum acceptable value")
        {
            getSetValueTest(std::numeric_limits<T>::lowest(), attribute, holder);
        }
        WHEN("[" + std::string(typeid(T).name()) + "] Setting the minimum acceptable value")
        {
            getSetValueTest(std::numeric_limits<T>::max(), attribute, holder);
        }
    }

    void numericalAttributeTest()
    {
        GIVEN("[" + std::string(typeid(T).name()) +
                "] A Variable holder of a numeric type")
        {
            VariableHolder<T> holder{0};

            GIVEN("[" + std::string(typeid(T).name()) +
                    "] An attribute, created with the Helper, associated to that variable holder")
            {
                Attribute attribute{"TestedAttributeWithHelper", makeBinder(holder.value)};
                testLimits(attribute, holder);
            }
            GIVEN("[" + std::string(typeid(T).name()) +
                    "] An attribute associated to that variable holder")
            {
                Attribute attribute{"TestedAttribute",
                                    Type<T>{},
                                    [&holder] () { return holder.get(); },
                                    [&holder] (T value) { holder.set(value); } };
                testLimits(attribute, holder);
            }
        }
    }
};

void booleanAttributeTest(bool booleanValue, Attribute &attribute, VariableHolder<bool> holder)
{
    THEN("The holder should contains this value")
    {
        CHECK(holder.value == booleanValue);
    }
    WHEN("Retrieving the value as string")
    {
        THEN("It should match the string which represents the value set")
        {
            // Implementation define that serialization generates lower case values
            CHECK(attribute.get() == (booleanValue ? "true" : "false"));
        }
    }
}

SCENARIO("Attribute", "[Xml Binding]")
{
    GIVEN("A list containing numerical type holder")
    {
        // Missing   long long, unsigned long long, long double
        NumericalAttributesTest<int, long, unsigned, short unsigned,
                                unsigned long, float, double>{}();
    }
    GIVEN("A string value holder")
    {
        VariableHolder<std::string> holder{""};
        GIVEN("An attribute, created with the Helper, associated to that variable holder")
        {
            Attribute attribute{"StringAttribute", makeBinder(holder.value)};
            WHEN("Setting a string value")
            {
                const std::string value = "TestValue";
                attribute.set(value);
                THEN("The holder should contains this value")
                {
                    CHECK(holder.value == value);
                }
                WHEN("Retrieving the value as string")
                {
                    THEN("It should match the string which represents the value set")
                    {
                        CHECK(attribute.get() == holder.value);
                    }
                }
            }
        }
    }
    GIVEN("A boolean value")
    {
        VariableHolder<bool> holder{false};
        GIVEN("An attribute, created with the Helper, associated to that variable holder")
        {
            Attribute attribute{"BooleanAttribute", makeBinder(holder.value)};
            WHEN("Setting true value")
            {
                attribute.set("true");
                booleanAttributeTest(true, attribute, holder);
            }
            WHEN("Setting TRUE value")
            {
                attribute.set("TRUE");
                booleanAttributeTest(true, attribute, holder);
            }
            WHEN("Setting 1 (true) value")
            {
                attribute.set("1");
                booleanAttributeTest(true, attribute, holder);
            }
            WHEN("Setting false value")
            {
                attribute.set("false");
                booleanAttributeTest(false, attribute, holder);
            }
            WHEN("Setting FALSE value")
            {
                attribute.set("FALSE");
                booleanAttributeTest(false, attribute, holder);
            }
            WHEN("Setting 0 (false) value")
            {
                attribute.set("0");
                booleanAttributeTest(false, attribute, holder);
            }
            WHEN("Setting an invalid value")
            {
                REQUIRE_THROWS_AS(attribute.set("Et ouais Mec!"), std::invalid_argument);
            }
        }
    }
}

class SerializerTest
{
public:
    struct FillMe
    {
        FillMe() {}
        FillMe(int integer, std::string string, bool boolean) :
            mInteger(integer), mString(string), mBoolean(boolean)
        {
        }

        bool operator == (const FillMe &fillMe)
        {
            return mInteger == fillMe.mInteger &&
                   mString == fillMe.mString &&
                   mBoolean == fillMe.mBoolean;
        }

        int mInteger = 0;
        std::string mString = "";
        bool mBoolean = false;
    };

    struct FillMeBigger : FillMe
    {
        FillMeBigger() {}
        FillMeBigger(int integer, std::string string, bool boolean, double doubleParam) :
            FillMe(integer, string, boolean), mDouble(doubleParam)
        {
        }
        FillMeBigger(const FillMe &fillMe, double doubleParam) :
            FillMe(fillMe), mDouble(doubleParam)
        {
        }

        bool operator == (const FillMeBigger &fillMeBigger)
        {
            return FillMe::operator == (fillMeBigger) && mDouble == fillMeBigger.mDouble;
        }

        double mDouble;
    };

    inline void removeWhitespaces(std::string& s)
    {
        s.erase(std::remove_if(s.begin(), s.end(), ::isspace), s.end());
    }

    Node getTestBindings(FillMe &toFill)
    {
        using namespace core::xml::binding;
        Node child {
            "FillChild",
            Body {
                Attributes {
                    { "BooleanAttr", makeBinder(toFill.mBoolean) },
                    { "StringAttr", makeBinder(toFill.mString) }
                },
                Nodes {} /** empty child tag list */
            }
        };
        return Node {
            "FillMe",
            Body {
                Attributes { { "IntegerAttr", makeBinder(toFill.mInteger) } },
                Nodes { child }
            }
        };
    }

    Node getTestBindings(FillMeBigger &toFill)
    {
        auto bindings = getTestBindings(dynamic_cast<FillMe&>(toFill));
        bindings.second.attributes.emplace_back(Attribute{"DoubleAttr", makeBinder(toFill.mDouble)});
        return bindings;
    }

    template <core::xml::serialization::ImportSource sourceType>
    void testDeserialization(const std::string &source,
                             const FillMe &structRef,
                             const std::string &schema = "")
    {
        using namespace core::xml::serialization;
        GIVEN("An empty structure")
        {
            FillMe fillMe{};
            GIVEN("Corresponding bindings")
            {
                auto bindings = getTestBindings(fillMe);
                WHEN("Deserializing the file into the empty structure")
                {
                    THEN("Deserializing goes well, filled structure carry desired information")
                    {
                        REQUIRE_NOTHROW(Deserializer<sourceType>(source, bindings, schema));
                        CHECK(fillMe == structRef);
                    }
                }
            }
        }
    }

    void testDeserializationError(const std::string &source, const std::string &schema = "")
    {
        using namespace core::xml::serialization;
        GIVEN("An empty structure")
        {
            FillMe fillMe{};
            GIVEN("Corresponding bindings")
            {
                auto bindings = getTestBindings(fillMe);
                WHEN("Deserializing the file into the empty structure")
                {
                    THEN("Deserializing does not work")
                    {
                        REQUIRE_THROWS_AS(
                                Deserializer<ImportSource::String>(source, bindings, schema),
                                std::runtime_error);
                    }
                }
            }
        }
    }
};

SCENARIO_METHOD(SerializerTest, "Serialization", "[Xml Binding]")
{
    using namespace core::xml::serialization;
    GIVEN("A string containing a correct XML")
    {
        const std::string xmlRef =
            R"(<?xml version="1.0" encoding="UTF-8"?>
               <FillMe IntegerAttr="1984">
                   <FillChild BooleanAttr="true" StringAttr="Et ouais Mec!"/>
               </FillMe>)";

        GIVEN("A reference structure filled with previous XML data")
        {
            const FillMe structRef{1984, "Et ouais Mec!", true};
            testDeserialization<ImportSource::String>(xmlRef, structRef);

            GIVEN("Corresponding bindings")
            {
                FillMe structRefCpy{structRef};
                auto bindings = getTestBindings(structRefCpy);
                WHEN("Serializing the structure into a valid stream")
                {
                    std::ostringstream output;
                    THEN("Serialization goes well and stream contains the reference xml file")
                    {
                        REQUIRE_NOTHROW(Serializer(output, bindings));
                        std::string result{output.str()};
                        std::string xmlRefCpy{xmlRef};

                        removeWhitespaces(result);
                        removeWhitespaces(xmlRefCpy);

                        CHECK(xmlRefCpy == result);
                        CHECK(structRefCpy == structRef);
                    }
                }
                WHEN("Serializing the structure into an invalid stream")
                {
                    std::ostream nullStream(0);
                    THEN("Serialization does not work")
                    {
                        REQUIRE_THROWS_AS(Serializer(nullStream, bindings), std::runtime_error);
                    }
                }
            }
            GIVEN("Invalid bindings in an XML Schema point of view")
            {
                FillMeBigger structRefCpy{structRef, 13.46};
                auto bindings = getTestBindings(structRefCpy);
                WHEN("Serializing the structure into a valid stream")
                {
                    std::ostringstream output;
                    THEN("Serialization goes well and stream contains the reference xml file")
                    {
                        REQUIRE_NOTHROW(Serializer(output, bindings));
                    }
                }
                GIVEN("The XML schema of the XML string")
                {
                    const std::string &xmlSchemaFile = "bindingTest.xsd";
                    WHEN("Serializing the structure into a valid stream")
                    {
                        std::ostringstream output;
                        THEN("Serialization does not work")
                        {
                            REQUIRE_THROWS_AS(Serializer(output, bindings, xmlSchemaFile),
                                              std::runtime_error);
                        }
                    }
                }
            }
        }
    }
    GIVEN("A string containing an XML with a bad root node")
    {
        const std::string xmlInvalidNode =
            R"(<?xml version="1.0" encoding="UTF-8"?>
               <PleaseFill BooleanAttr="1984">
                   <FillChild BooleanAttr="true" StringAttr="Et ouais Mec!"/>
               </PleaseFill>)";
        testDeserializationError(xmlInvalidNode);
    }
    GIVEN("A string containing an XML with a node not listed in bindings")
    {
        const std::string xmlInvalidNode =
            R"(<?xml version="1.0" encoding="UTF-8"?>
               <FillMe BooleanAttr="1984">
                   <FillDaughter BooleanAttr="true" StringAttr="Et ouais Mec!"/>
               </FillMe>)";
        testDeserializationError(xmlInvalidNode);
    }
    GIVEN("A string containing an XML with an invalid attribute value")
    {
        const std::string xmlInvalidAttribute =
            R"(<?xml version="1.0" encoding="UTF-8"?>
               <FillMe IntegerAttr="1984">
                   <FillChild BooleanAttr="YesOfCourse" StringAttr="Et ouais Mec!"/>
               </FillMe>)";
        testDeserializationError(xmlInvalidAttribute);
    }
    GIVEN("A string containing an invalid XML in the point of view of a schema")
    {
        const std::string xmlInvalidAttribute =
            R"(<?xml version="1.0" encoding="UTF-8"?>
               <FillMe IntegerAttr="1984" StringAttr="blabla">
                   <FillChild BooleanAttr="true" StringAttr="Et ouais Mec!"/>
               </FillMe>)";
        GIVEN("A reference structure filled with previous XML data")
        {
            const FillMe structRef{1984, "Et ouais Mec!", true};
            testDeserialization<ImportSource::String>(xmlInvalidAttribute, structRef);
        }
        GIVEN("The corresponding XML schema")
        {
            const std::string &xmlSchemaFile = "bindingTest.xsd";
            testDeserializationError(xmlInvalidAttribute, xmlSchemaFile);
        }
    }
    GIVEN("A file containing a valid xml")
    {
        const std::string &xmlFile = "valid.xml";
        GIVEN("A reference structure filled with data from the XML file")
        {
            const FillMe structRef{666666, "Premature optimization is the root of all evil", false};
            testDeserialization<ImportSource::File>(xmlFile, structRef);
        }

        GIVEN("The corresponding XML schema")
        {
            const std::string &xmlSchemaFile = "bindingTest.xsd";
            GIVEN("A reference structure filled with data from the XML file")
            {
                const FillMe structRef {
                    666666, "Premature optimization is the root of all evil", false
                };
                testDeserialization<ImportSource::File>(xmlFile, structRef, xmlSchemaFile);
            }
        }
    }
}
