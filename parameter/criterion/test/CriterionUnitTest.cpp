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

#include <criterion/Criteria.h>
#include <criterion/Criterion.h>
#include <criterion/InclusiveCriterion.h>
#include <log/Logger.h>
#include "XmlSerializingContext.h"
#include "XmlStreamDocSink.h"
#include "XmlMemoryDocSource.h"

#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <random>
#include <bitset>
#include <algorithm>
#include <stdexcept>

using namespace core;
using namespace core::criterion;

/** Raw logging class Helper */
class TestLogger : public log::ILogger
{
public:
    void info(const std::string& info) { logBuf << "Info: " << info; }
    void warning(const std::string& warning) { logBuf << "Warning: " << warning; }
    std::string getLog() { return logBuf.str(); }

private:
    std::ostringstream logBuf;
};

struct LoggingTest {
    LoggingTest() : mRawLogger(), mLogger(mRawLogger) {}

    void REQUIRE_FAILURE(bool success, const std::string& result)
    {
        THEN("It should be an error")
        {
            INFO("Result message was: '" + result + "'");
            CHECK(not success);
            CHECK(not result.empty());
        }
    }

    void REQUIRE_FAILURE(bool success)
    {
        THEN("It should be an error")
        {
            CHECK(not success);
        }
    }

    void REQUIRE_SUCCESS(bool success, const std::string& result = "")
    {
        THEN("It should be a success")
        {
            INFO("Result message was" + (result.empty() ? " empty" : ": '" + result + "'"));
            CHECK(success);
        }
    }

    inline void removeWhitespaces(std::string& s)
    {
        s.erase(std::remove_if(s.begin(), s.end(), ::isspace), s.end());
    }

    /** xml serialization into a string helper */
    void xmlSerialize(std::string& result, IXmlSource* sourceNode, const std::string& nodeName)
    {
        std::string error;
        CXmlElement xmlElement;
        CXmlSerializingContext context(error);
        CXmlMemoryDocSource source(sourceNode, false, nodeName);

        std::ostringstream sinkStream;
        CXmlStreamDocSink sink(sinkStream);

        // Processing will call XmlSource toXml method
        REQUIRE_SUCCESS(sink.process(source, context), error);
        result = sinkStream.str();
    }

    /** Raw logger we can check */
    TestLogger mRawLogger;
    /** Application standard Logger */
    log::Logger mLogger;
};

/** Test fixtures for Criteria */
struct CriteriaTest : LoggingTest {
    void checkListCriteria(bool human, bool typeInfo)
    {
        std::list<std::string> results, desireds;
        for (auto& description : mDescriptions) {
            desireds.push_back(description.criterion->getFormattedDescription(true, true));
        }
        mCriteria.listSelectionCriteria(results, human, typeInfo);
        THEN("Each criterion description should be in the listing")
        {
            for (const auto& result : results) {
                CAPTURE(result);
                const auto& it = std::find(desireds.begin(), desireds.end(), result);
                CHECK(it != desireds.end());
            }
        }
    }

    /** Structure to describe and test criteria */
    struct CriterionDescription {
        const std::string name;
        bool isInclusive;
        Criterion* criterion;
    };
    using CriterionDescriptions = std::vector<CriterionDescription>;

    /** Tested descriptions */
    CriterionDescriptions mDescriptions {
        { "A", true, nullptr },
        { "B", false, nullptr },
        { "C", true, nullptr },
        { "D", false, nullptr }
    };

    /** Tested criteria object */
    Criteria mCriteria {};
};

/** Test fixtures for Criterion */
struct CriterionTest : public LoggingTest {
    using CriterionValues = std::map<std::string, int>;

    /** Help to generate some values */
    CriterionValues generateCriterionValues(bool isInclusive, size_t nbValues)
    {
        // 0 is invalid for inclusive criterion
        int offset = isInclusive ? 1 : 0;
        CriterionValues criterionValues;

        for (size_t i = offset; i < nbValues; i++) {
            criterionValues.emplace("Crit_" + std::to_string(i), i);
        }

        return criterionValues;
    }

    void checkUnknownValueGet(Criterion& criterion)
    {
        WHEN("Getting a literal value from an unknown numerical one")
        {
            std::string result;
            std::string literalValue;
            REQUIRE_FAILURE(criterion.getLiteralValue(2, literalValue));
            CHECK(literalValue.empty());
        }
        WHEN("Getting a numerical value from an unknown literal one")
        {
            std::string result;
            int numericalValue = 0;
            REQUIRE_FAILURE(criterion.getNumericalValue("UnknowValue", numericalValue));
            CHECK(numericalValue == 0);
        }
    }

    void checkExistingValueInsertion(Criterion& criterion)
    {
        WHEN("Adding an existing literal value")
        {
            std::string result;
            std::string literalValue = "DoubleState";
            REQUIRE_SUCCESS(criterion.addValuePair(1, literalValue, result), result);
            REQUIRE_FAILURE(criterion.addValuePair(2, literalValue, result), result);
        }
        // FIXME
        // WHEN("Adding an existing numerical value") {
        //    std::string result;
        //    int numericalValue = 1;
        //    REQUIRE_SUCCESS(criterion.addValuePair(numericalValue, "State1", result), result);
        //    REQUIRE_FAILURE(criterion.addValuePair(numericalValue, "State2", result), result);
        // }
    }

    void checkInclusiveCriterionSet(Criterion& criterion)
    {
        // CriterionValues contains 31 value as defined previously
        WHEN("Setting many inclusive value at the same time")
        {
            std::bitset<31> stateMask("001001001001001");
            std::bitset<31> subStateMask("000001001000001");
            std::bitset<31> almostSubStateMask("100001001000001");
            std::bitset<31> excludeMask("010000010010000");

            CAPTURE(stateMask.to_ulong());
            criterion.setCriterionState(stateMask.to_ulong());

            WHEN("Matching with 'Includes' a mask contained in the state mask")
            {
                REQUIRE_SUCCESS(criterion.match("Includes", subStateMask.to_ulong()));
            }
            WHEN("Matching with 'Includes' a mask with not all its bit set in the current state")
            {
                REQUIRE_FAILURE(criterion.match("Includes", almostSubStateMask.to_ulong()));
            }
            WHEN("Matching with 'Includes' a mask with no common bit with the current state")
            {
                REQUIRE_FAILURE(criterion.match("Includes", excludeMask.to_ulong()));
            }
            WHEN("Matching with 'Excludes' a mask contained in the state mask")
            {
                REQUIRE_FAILURE(criterion.match("Excludes", subStateMask.to_ulong()));
            }
            WHEN("Matching with 'Excludes' a mask with not all its bit set in the current state")
            {
                REQUIRE_FAILURE(criterion.match("Excludes", almostSubStateMask.to_ulong()));
            }
            WHEN("Matching with 'Excludes' a mask with no common bit with the current state")
            {
                REQUIRE_SUCCESS(criterion.match("Excludes", excludeMask.to_ulong()));
            }
        }
    }

    void checkExclusiveCriterionSet(Criterion& criterion, CriterionValues& criterionValues)
    {
        WHEN("Setting the current value")
        {
            int currentState = criterion.getCriterionState();
            CAPTURE(currentState);
            std::string oldLog = mRawLogger.getLog();
            criterion.setCriterionState(currentState);
            THEN("Criterion should not be modified")
            {
                CHECK(not criterion.hasBeenModified());
            }
            THEN("No information should be logged")
            {
                CHECK(oldLog == mRawLogger.getLog());
            }
        }

        for (auto& value : criterionValues) {
            if (value.second != criterion.getCriterionState()) {
                WHEN("Setting a new value")
                {
                    CAPTURE(value.second);
                    criterion.setCriterionState(value.second);

                    THEN("State should have been updated")
                    {
                        CHECK(criterion.getCriterionState() == value.second);
                        CHECK(criterion.getFormattedState() == value.first);
                    }
                    THEN("Criterion should be modified")
                    {
                        CHECK(criterion.hasBeenModified());
                    }
                    THEN("Criterion Is match method should be valid")
                    {
                        CHECK(criterion.match("Is", value.second));
                    }
                    THEN("Criterion IsNot match method should not be valid")
                    {
                        CHECK(not criterion.match("IsNot", value.second));
                    }
                    THEN("Criterion update event should be logged")
                    {
                        size_t logPos = mRawLogger.getLog().find(
                            "Info: Selection criterion changed event: " +
                            criterion.getFormattedDescription(false, false));
                        CHECK(logPos != std::string::npos);
                    }
                }
            }
        }
        WHEN("Setting  many value in a raw")
        {
            // Set value which are valid for inclusive or exclusive criterion
            criterion.setCriterionState(2);
            criterion.setCriterionState(4);
            THEN("Criterion should be modified")
            {
                CHECK(criterion.hasBeenModified());
            }
            THEN("Criterion multi modification should be logged")
            {
                size_t logPos = mRawLogger.getLog().find("Warning: Selection criterion '" +
                                                         criterion.getCriterionName() +
                                                         "' has been modified 1 time(s) without any"
                                                         " configuration application");
                CHECK(logPos != std::string::npos);
            }
            WHEN("Resetting criterion status")
            {
                criterion.resetModifiedStatus();
                THEN("Criterion should not be modified")
                {
                    CHECK(not criterion.hasBeenModified());
                }
            }
        }
    }

    void checkSerialization(Criterion& criterion)
    {
        std::string kind = criterion.isInclusive() ? "Inclusive" : "Exclusive";
        WHEN("Serializing through xml")
        {
            std::string defaultValue = criterion.isInclusive() ? "none" : "&lt;none&gt;";
            std::string defaultState = criterion.isInclusive() ?
                                       R"(<ValuePair Literal="none" Numerical="0"/>)" : "";
            std::string xmlDescription =
                R"(<?xml version="1.0" encoding="UTF-8"?>
                  <SelectionCriterion Value=")"
                + defaultValue +
                "\" Name=\"" + criterion.getCriterionName() +
                "\" Kind=\"" + kind +
                R"(">
                    <ValuePair Literal="a" Numerical="2"/>
                    <ValuePair Literal="b" Numerical="3"/>
                    <ValuePair Literal="c" Numerical="4"/>)" +
                defaultState +
                "</SelectionCriterion>";

            std::string result;
            xmlSerialize(result, &criterion, "SelectionCriterion");
            // Remove whitespaces as they are not relevant in xml
            removeWhitespaces(result);
            removeWhitespaces(xmlDescription);

            THEN("Generated xml match expectation")
            {
                CHECK(result == xmlDescription);
            }
        }

        std::string defaultState = (criterion.isInclusive() ? "none" : "<none>");
        std::string defaultStateType = criterion.isInclusive() ? ", none}" : "}";
        WHEN("Serializing through Csv")
        {
            std::string nameInfo = "Criterion name: " + criterion.getCriterionName();
            std::string currentStateInfo = std::string(", current state: ") + defaultState;

            THEN("Generated csv match expectation")
            {
                std::string csvDescription = nameInfo + currentStateInfo;
                std::string dump = criterion.getFormattedDescription(false, false);
                CHECK(dump == csvDescription);
            }

            THEN("Generated csv with type information match expectation")
            {
                std::string csvDescription = nameInfo + ", type kind: " + kind + currentStateInfo +
                                             ", states: {a, b, c" + defaultStateType;
                std::string dump = criterion.getFormattedDescription(true, false);
                CHECK(dump == csvDescription);
            }
        }
        WHEN("Generating human readable description")
        {
            THEN("Generated description match expectation")
            {
                std::string description = criterion.getCriterionName() + " = " + defaultState;
                std::string dump = criterion.getFormattedDescription(false, true);
                CHECK(dump == description);
            }

            THEN("Generated description with type information match expectation")
            {
                std::string defaultStateHuman = criterion.isInclusive() ? ", none}" : "}";
                std::string description = criterion.getCriterionName() + ":";
                std::string titleDecorator(description.length(), '=');
                description = "\n" + description + "\n" + titleDecorator +
                              "\nPossible states (" + kind + "): {a, b, c" + defaultStateType +
                              "\n" + "Current state = " + defaultState;
                std::string dump = criterion.getFormattedDescription(true, true);
                CHECK(dump == description);
            }
        }
    }

    void checkDisplay(Criterion& criterion)
    {
        std::string possibleValues = std::string("{a, b, c") +
                                     (criterion.isInclusive() ? ", none" : "") + "}";
        WHEN("Adding some criterion value")
        {
            std::string result;
            REQUIRE_SUCCESS(criterion.addValuePair(2, "a", result), result);
            REQUIRE_SUCCESS(criterion.addValuePair(3, "b", result), result);
            REQUIRE_SUCCESS(criterion.addValuePair(4, "c", result), result);
            THEN("Possible values match all values added in the criterion")
            {
                CHECK(criterion.listPossibleValues() == possibleValues);
            }

            checkSerialization(criterion);

            if (criterion.isInclusive()) {
                std::bitset<31> validStateMask("1110");
                WHEN("Setting some criterion value")
                {
                    criterion.setCriterionState(validStateMask.to_ulong());
                    THEN("Formatted state contains all set values")
                    {
                        std::string formattedState = "a|b|c";
                        CHECK(criterion.getFormattedState() == formattedState);
                    }
                }
                WHEN("Setting a mask containing unknown values")
                {
                    std::bitset<31> erroneousStateMask("10101010101110");
                    criterion.setCriterionState(erroneousStateMask.to_ulong());
                    THEN("Formatted state take into account only registered values")
                    {
                        std::string formattedState = "a|b|c";
                        CHECK(criterion.getFormattedState() == formattedState);
                    }
                    // Check matching in this special configuration
                    WHEN("Matching with 'Includes' the mask corresponding to the formatted one")
                    {
                        REQUIRE_SUCCESS(criterion.match("Includes", validStateMask.to_ulong()));
                    }
                    // FIXME: correct set state in order to avoid this case
                    // WHEN("Matching with 'Is' the mask corresponding to the formatted one") {
                    //    REQUIRE_SUCCESS(criterion.match("Is", validStateMask.to_ulong()));
                    // }
                }
            }
        }
    }

    void checkInsertionBijectivity(Criterion& criterion)
    {
        WHEN("Adding some criterion value")
        {
            /** Generate 31 values, no more because inclusive criterion cannot handle it
             * Unbounded values are tested later for inclusive criterion
             */
            CriterionValues criterionValues = generateCriterionValues(criterion.isInclusive(), 31);

            for (auto& value : criterionValues) {
                std::string result;
                REQUIRE_SUCCESS(criterion.addValuePair(value.second, value.first, result), result);

                THEN("Numerical value should correspond")
                {
                    int numericalValue;
                    REQUIRE_SUCCESS(criterion.getNumericalValue(value.first, numericalValue));
                    CHECK(numericalValue == value.second);
                }
                THEN("Literal value should correspond")
                {
                    std::string literalValue;
                    REQUIRE_SUCCESS(criterion.getLiteralValue(value.second, literalValue));
                    CHECK(literalValue == value.first);
                }
            }

            checkExclusiveCriterionSet(criterion, criterionValues);
            if (criterion.isInclusive()) {
                // Inclusive criterion has a more evolved setting behavior
                checkInclusiveCriterionSet(criterion);
            }
        }
    }

    void checkCriterionBasicBehavior(Criterion& criterion, std::string name)
    {
        using MatchMethods = std::vector<std::string>;
        /** key indicates if it's available for exclusive criterion */
        const std::map<bool, MatchMethods> mMatchMethods {
            { true, { "Is", "IsNot" } },
            { false, { "Includes", "Excludes" } }
        };

        THEN("Only corresponding match method are available")
        {
            for (auto& matcher : mMatchMethods) {
                for (auto& matchMethod : matcher.second) {
                    INFO("Checking availibility of '" + matchMethod + "' match method");
                    /** A method is authorized if available or if the criterion is not inclusive
                     * and the method is defined in mMatchMethods as unavailable for exclusive
                     * criterion
                     */
                    bool isAuthorizedMethod = criterion.isMatchMethodAvailable(matchMethod) or
                                              not (criterion.isInclusive() or matcher.first);
                    CHECK(isAuthorizedMethod);
                }
            }
        }
        WHEN("Undefined match method is requested")
        {
            REQUIRE_THROWS_AS(criterion.match("InvalidMatch", 0), std::out_of_range);
        }

        THEN("The criterion has not been modified")
        {
            CHECK(not criterion.hasBeenModified());
        }
        THEN("The criterion has the good name")
        {
            CHECK(criterion.getCriterionName() == name);
        }

        checkInsertionBijectivity(criterion);
        checkExistingValueInsertion(criterion);
        checkUnknownValueGet(criterion);
        checkDisplay(criterion);
    }

};

SCENARIO_METHOD(CriterionTest, "Criterion lifecycle", "[criterion]")
{

    GIVEN("An exclusive criterion")
    {
        const std::string criterionName = "ExclusiveCriterion";
        Criterion criterion(criterionName, mLogger);

        THEN("The criterion is not inclusive")
        {
            CHECK(not criterion.isInclusive());
        }
        THEN("There is no available states")
        {
            CHECK(criterion.listPossibleValues() == "{}");
        }
        THEN("No state is currently set")
        {
            CHECK(criterion.getFormattedState() == "<none>");
        }

        checkCriterionBasicBehavior(criterion, criterionName);

        WHEN("We add an negative numerical value")
        {
            std::string result;
            // FIXME: allow only positive numerical value
            REQUIRE_SUCCESS(criterion.addValuePair(-3, "Negative", result), result);
        }
        WHEN("We add a random numerical value")
        {
            std::string result;
            std::default_random_engine generator;
            // FIXME: use uint32_t internally instead
            // Criterion State type is int32_t
            std::uniform_int_distribution<int32_t> dist;
            int32_t numericalValue = dist(generator);
            CAPTURE(numericalValue);
            REQUIRE_SUCCESS(criterion.addValuePair(numericalValue, "Random", result), result);
        }
    }

    GIVEN("An inclusive criterion")
    {
        const std::string criterionName = "InclusiveCriterion";
        InclusiveCriterion criterion(criterionName, mLogger);

        THEN("The criterion is inclusive")
        {
            CHECK(criterion.isInclusive());
        }
        THEN("Default state is available")
        {
            const std::string defaultState = "none";
            CHECK(criterion.listPossibleValues() == "{" + defaultState + "}");
            int numericalValue;
            REQUIRE_SUCCESS(criterion.getNumericalValue(defaultState, numericalValue));
            CHECK(numericalValue == 0);
            std::string literalValue;
            REQUIRE_SUCCESS(criterion.getLiteralValue(0, literalValue));
            CHECK(literalValue == defaultState);
        }
        THEN("Default state is set")
        {
            CHECK(criterion.getCriterionState() == 0);
            CHECK(criterion.getFormattedState() == "none");
        }

        checkCriterionBasicBehavior(criterion, criterionName);

        WHEN("We add a state with 0 as numerical value")
        {
            std::string result;
            REQUIRE_FAILURE(criterion.addValuePair(0, "Crit_0", result), result);
        }
    }
}

SCENARIO_METHOD(CriteriaTest, "Criteria Use", "[criterion]")
{

    GIVEN("A criteria object")
    {
        WHEN("Adding some criteria")
        {

            for (auto& description : mDescriptions) {
                Criterion* addedCriterion = (description.isInclusive ?
                                             mCriteria.createInclusiveCriterion(description.name,
                                                                                mLogger) :
                                             mCriteria.createExclusiveCriterion(description.name,
                                                                                mLogger));
                description.criterion = addedCriterion;
                THEN("Added criteria match the request")
                {
                    CAPTURE(description.name);
                    CHECK(addedCriterion->isInclusive() == description.isInclusive);
                    CHECK(addedCriterion->getCriterionName() == description.name);
                }
            }
            WHEN("Retrieving added criteria")
            {
                for (auto& description : mDescriptions) {
                    CAPTURE(description.name);
                    CHECK(mCriteria.getSelectionCriterion(description.name) ==
                          description.criterion);
                    const Criterion* criterion = mCriteria.getSelectionCriterion(description.name);
                    CHECK(criterion == description.criterion);
                }
            }
            WHEN("Retrieving unknown criteria")
            {
                /** FIXME: nullptr in check expression is not available in
                 * Ubuntu catch version for now. We should upgrade it one day.
                 */
                CHECK(mCriteria.getSelectionCriterion("Unknown") == NULL);
            }
            WHEN("Modifying criteria")
            {
                for (auto& description : mDescriptions) {
                    std::string result;
                    REQUIRE_SUCCESS(description.criterion->addValuePair(1, "State", result),
                                    result);
                    description.criterion->setCriterionState(1);
                    CHECK(description.criterion->hasBeenModified());
                }
                WHEN("Resetting criteria status")
                {
                    mCriteria.resetModifiedStatus();
                    THEN("Status of all criteria has been reset")
                    {
                        for (auto& description : mDescriptions) {
                            CHECK(not description.criterion->hasBeenModified());
                        }
                    }
                }
            }
            WHEN("Listing criteria")
            {
                checkListCriteria(true, true);
                checkListCriteria(true, false);
                checkListCriteria(false, true);
                checkListCriteria(false, false);
            }
            WHEN("Serializing criteria")
            {
                std::string xmlDescription =
                    R"(<?xml version="1.0" encoding="UTF-8"?>
                       <SelectionCriteria>
                        <SelectionCriterion Value="none" Name="A" Kind="Inclusive">
                            <ValuePair Literal="none" Numerical="0"/>
                        </SelectionCriterion>
                        <SelectionCriterion Value="&lt;none&gt;" Name="B" Kind="Exclusive"/>
                        <SelectionCriterion Value="none" Name="C" Kind="Inclusive">
                            <ValuePair Literal="none" Numerical="0"/>
                        </SelectionCriterion>
                        <SelectionCriterion Value="&lt;none&gt;" Name="D" Kind="Exclusive"/>
                       </SelectionCriteria>)";

                std::string result;
                xmlSerialize(result, &mCriteria, "SelectionCriteria");

                // Remove whitespaces as they are not relevant in xml
                removeWhitespaces(result);
                removeWhitespaces(xmlDescription);

                THEN("Generated xml match expectation")
                {
                    CHECK(result == xmlDescription);
                }
            }
        }
    }
}
