/*
 * Copyright (c) 2011-2015, Intel Corporation
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

#include <stdlib.h>
#include <sstream>
#include <assert.h>
#include <errno.h>
#include <convert.hpp>
#include <sstream>
#include "TestPlatform.h"

using std::string;

CTestPlatform::CTestPlatform(const string &strClass, uint16_t iPortNumber)
    : mParameterMgrPlatformConnector(strClass), mLogger(), mRemoteProcessorServer(iPortNumber)
{
    mParameterMgrPlatformConnector.setLogger(&mLogger);
}

CTestPlatform::~CTestPlatform()
{
}

CTestPlatform::CommandReturn CTestPlatform::exit(const IRemoteCommand & /*command*/,
                                                 string & /*strResult*/)
{
    mRemoteProcessorServer.stop();

    return CTestPlatform::CCommandHandler::EDone;
}

bool CTestPlatform::run(std::string &strError)
{
    // Start remote processor server
    if (!mRemoteProcessorServer.start(strError)) {

        strError = "TestPlatform: Unable to start remote processor server: " + strError;
        return false;
    }

    CCommandHandler commandHandler(this);

    // Add command parsers
    commandHandler.addCommandParser("exit", &CTestPlatform::exit, 0, "", "Exit TestPlatform");
    commandHandler.addCommandParser("createExclusiveSelectionCriterionFromStateList",
                                    &CTestPlatform::createExclusiveSelectionCriterionFromStateList,
                                    2, "<name> <stateList>",
                                    "Create inclusive selection criterion from state name list");
    commandHandler.addCommandParser("createInclusiveSelectionCriterionFromStateList",
                                    &CTestPlatform::createInclusiveSelectionCriterionFromStateList,
                                    2, "<name> <stateList>",
                                    "Create exclusive selection criterion from state name list");

    commandHandler.addCommandParser("createExclusiveSelectionCriterion",
                                    &CTestPlatform::createExclusiveSelectionCriterion, 2,
                                    "<name> <nbStates>", "Create inclusive selection criterion");
    commandHandler.addCommandParser("createInclusiveSelectionCriterion",
                                    &CTestPlatform::createInclusiveSelectionCriterion, 2,
                                    "<name> <nbStates>", "Create exclusive selection criterion");

    commandHandler.addCommandParser("start", &CTestPlatform::startParameterMgr, 0, "",
                                    "Start ParameterMgr");

    commandHandler.addCommandParser("setCriterionState", &CTestPlatform::setCriterionState, 2,
                                    "<name> <state>",
                                    "Set the current state of a selection criterion");
    commandHandler.addCommandParser(
        "applyConfigurations", &CTestPlatform::applyConfigurations, 0, "",
        "Apply configurations selected by current selection criteria states");

    commandHandler.addCommandParser(
        "setFailureOnMissingSubsystem",
        &CTestPlatform::setter<&CParameterMgrPlatformConnector::setFailureOnMissingSubsystem>, 1,
        "true|false", "Set policy for missing subsystems, "
                      "either abort start or fallback on virtual subsystem.");
    commandHandler.addCommandParser(
        "getMissingSubsystemPolicy",
        &CTestPlatform::getter<&CParameterMgrPlatformConnector::getFailureOnMissingSubsystem>, 0,
        "", "Get policy for missing subsystems, "
            "either abort start or fallback on virtual subsystem.");

    commandHandler.addCommandParser(
        "setFailureOnFailedSettingsLoad",
        &CTestPlatform::setter<&CParameterMgrPlatformConnector::setFailureOnFailedSettingsLoad>, 1,
        "true|false",
        "Set policy for failed settings load, either abort start or continue without domains.");
    commandHandler.addCommandParser(
        "getFailedSettingsLoadPolicy",
        &CTestPlatform::getter<&CParameterMgrPlatformConnector::getFailureOnFailedSettingsLoad>, 0,
        "", "Get policy for failed settings load, either abort start or continue without domains.");

    commandHandler.addCommandParser(
        "setValidateSchemasOnStart",
        &CTestPlatform::setter<&CParameterMgrPlatformConnector::setValidateSchemasOnStart>, 1,
        "true|false", "Set policy for schema validation based on .xsd files (false by default).");
    commandHandler.addCommandParser(
        "getValidateSchemasOnStart",
        &CTestPlatform::getter<&CParameterMgrPlatformConnector::getValidateSchemasOnStart>, 0, "",
        "Get policy for schema validation based on .xsd files.");

    commandHandler.addCommandParser("getSchemaUri", &CTestPlatform::getSchemaUri, 0, "",
                                    "Get the directory where schemas can be found.");
    commandHandler.addCommandParser("setSchemaUri", &CTestPlatform::setSchemaUri, 1, "<directory>",
                                    "Set the directory where schemas can be found.");

    return mRemoteProcessorServer.process(commandHandler);
}

//////////////// Remote command parsers
/// Selection Criterion
CTestPlatform::CommandReturn CTestPlatform::createExclusiveSelectionCriterionFromStateList(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    return createExclusiveSelectionCriterionFromStateList(remoteCommand.getArgument(0),
                                                          remoteCommand, strResult)
               ? CTestPlatform::CCommandHandler::EDone
               : CTestPlatform::CCommandHandler::EFailed;
}

CTestPlatform::CommandReturn CTestPlatform::createInclusiveSelectionCriterionFromStateList(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    return createInclusiveSelectionCriterionFromStateList(remoteCommand.getArgument(0),
                                                          remoteCommand, strResult)
               ? CTestPlatform::CCommandHandler::EDone
               : CTestPlatform::CCommandHandler::EFailed;
}

CTestPlatform::CommandReturn CTestPlatform::createExclusiveSelectionCriterion(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    return createExclusiveSelectionCriterion(remoteCommand.getArgument(0),
                                             strtoul(remoteCommand.getArgument(1).c_str(), NULL, 0),
                                             strResult)
               ? CTestPlatform::CCommandHandler::EDone
               : CTestPlatform::CCommandHandler::EFailed;
}

CTestPlatform::CommandReturn CTestPlatform::createInclusiveSelectionCriterion(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    return createInclusiveSelectionCriterion(remoteCommand.getArgument(0),
                                             strtoul(remoteCommand.getArgument(1).c_str(), NULL, 0),
                                             strResult)
               ? CTestPlatform::CCommandHandler::EDone
               : CTestPlatform::CCommandHandler::EFailed;
}

CTestPlatform::CommandReturn CTestPlatform::startParameterMgr(
    const IRemoteCommand & /*remoteCommand*/, string &strResult)
{
    return mParameterMgrPlatformConnector.start(strResult)
               ? CTestPlatform::CCommandHandler::EDone
               : CTestPlatform::CCommandHandler::EFailed;
}

template <CTestPlatform::setter_t setFunction>
CTestPlatform::CommandReturn CTestPlatform::setter(const IRemoteCommand &remoteCommand,
                                                   string &strResult)
{
    const string &strAbort = remoteCommand.getArgument(0);

    bool bFail;

    if (!convertTo(strAbort, bFail)) {
        return CTestPlatform::CCommandHandler::EShowUsage;
    }

    return (mParameterMgrPlatformConnector.*setFunction)(bFail, strResult)
               ? CTestPlatform::CCommandHandler::EDone
               : CTestPlatform::CCommandHandler::EFailed;
}

template <CTestPlatform::getter_t getFunction>
CTestPlatform::CommandReturn CTestPlatform::getter(const IRemoteCommand & /*command*/,
                                                   string &strResult)
{
    strResult = (mParameterMgrPlatformConnector.*getFunction)() ? "true" : "false";

    return CTestPlatform::CCommandHandler::ESucceeded;
}

CTestPlatform::CommandReturn CTestPlatform::getSchemaUri(const IRemoteCommand & /*remotecommand*/,
                                                         string &result)
{
    result = mParameterMgrPlatformConnector.getSchemaUri();
    return CTestPlatform::CCommandHandler::EDone;
}

CTestPlatform::CommandReturn CTestPlatform::setSchemaUri(const IRemoteCommand &remotecommand,
                                                         string & /*result*/)
{
    mParameterMgrPlatformConnector.setSchemaUri(remotecommand.getArgument(0));
    return CTestPlatform::CCommandHandler::EDone;
}

CTestPlatform::CommandReturn CTestPlatform::setCriterionState(const IRemoteCommand &remoteCommand,
                                                              string &strResult)
{

    bool bSuccess;

    uint32_t state;

    if (convertTo(remoteCommand.getArgument(1), state)) {
        // Sucessfull conversion, set criterion state by numerical state
        bSuccess = setCriterionState(remoteCommand.getArgument(0), state, strResult);

    } else {
        // Conversion failed, set criterion state by lexical state
        bSuccess = setCriterionStateByLexicalSpace(remoteCommand, strResult);
    }

    return bSuccess ? CTestPlatform::CCommandHandler::EDone
                    : CTestPlatform::CCommandHandler::EFailed;
}

CTestPlatform::CommandReturn CTestPlatform::applyConfigurations(const IRemoteCommand & /*command*/,
                                                                string & /*strResult*/)
{
    mParameterMgrPlatformConnector.applyConfigurations();

    return CTestPlatform::CCommandHandler::EDone;
}

//////////////// Remote command handlers

bool CTestPlatform::createExclusiveSelectionCriterionFromStateList(
    const string &strName, const IRemoteCommand &remoteCommand, string &strResult)
{
    ISelectionCriterionTypeInterface *pCriterionType =
        mParameterMgrPlatformConnector.createSelectionCriterionType(false);

    assert(pCriterionType != NULL);

    size_t nbStates = remoteCommand.getArgumentCount() - 1;

    for (size_t state = 0; state < nbStates; state++) {

        const std::string &strValue = remoteCommand.getArgument(state + 1);

        // FIXME state type vs addValuePair params
        if (!pCriterionType->addValuePair(int(state), strValue, strResult)) {

            strResult = "Unable to add value: " + strValue + ": " + strResult;

            return false;
        }
    }

    mParameterMgrPlatformConnector.createSelectionCriterion(strName, pCriterionType);

    return true;
}

bool CTestPlatform::createInclusiveSelectionCriterionFromStateList(
    const string &strName, const IRemoteCommand &remoteCommand, string &strResult)
{
    ISelectionCriterionTypeInterface *pCriterionType =
        mParameterMgrPlatformConnector.createSelectionCriterionType(true);

    assert(pCriterionType != NULL);

    size_t nbStates = remoteCommand.getArgumentCount() - 1;

    for (size_t state = 0; state < nbStates; state++) {

        const std::string &strValue = remoteCommand.getArgument(state + 1);

        if (!pCriterionType->addValuePair(0x1 << state, strValue, strResult)) {

            strResult = "Unable to add value: " + strValue + ": " + strResult;

            return false;
        }
    }

    mParameterMgrPlatformConnector.createSelectionCriterion(strName, pCriterionType);

    return true;
}

bool CTestPlatform::createExclusiveSelectionCriterion(const string &strName, size_t nbStates,
                                                      string &strResult)
{
    ISelectionCriterionTypeInterface *pCriterionType =
        mParameterMgrPlatformConnector.createSelectionCriterionType(false);

    for (size_t state = 0; state < nbStates; state++) {

        std::ostringstream ostrValue;

        ostrValue << "State_";
        ostrValue << state;

        // FIXME state type vs addValuePair params
        if (!pCriterionType->addValuePair(int(state), ostrValue.str(), strResult)) {

            strResult = "Unable to add value: " + ostrValue.str() + ": " + strResult;

            return false;
        }
    }

    mParameterMgrPlatformConnector.createSelectionCriterion(strName, pCriterionType);

    return true;
}

bool CTestPlatform::createInclusiveSelectionCriterion(const string &strName, size_t nbStates,
                                                      string &strResult)
{
    ISelectionCriterionTypeInterface *pCriterionType =
        mParameterMgrPlatformConnector.createSelectionCriterionType(true);

    for (size_t state = 0; state < nbStates; state++) {

        std::ostringstream ostrValue;

        ostrValue << "State_0x";
        ostrValue << (0x1 << state);

        if (!pCriterionType->addValuePair(0x1 << state, ostrValue.str(), strResult)) {

            strResult = "Unable to add value: " + ostrValue.str() + ": " + strResult;

            return false;
        }
    }

    mParameterMgrPlatformConnector.createSelectionCriterion(strName, pCriterionType);

    return true;
}

bool CTestPlatform::setCriterionState(const string &strName, uint32_t uiState, string &strResult)
{
    ISelectionCriterionInterface *pCriterion =
        mParameterMgrPlatformConnector.getSelectionCriterion(strName);

    if (!pCriterion) {

        strResult = "Unable to retrieve selection criterion: " + strName;

        return false;
    }

    pCriterion->setCriterionState(uiState);

    return true;
}

bool CTestPlatform::setCriterionStateByLexicalSpace(const IRemoteCommand &remoteCommand,
                                                    string &strResult)
{

    // Get criterion name
    std::string strCriterionName = remoteCommand.getArgument(0);

    ISelectionCriterionInterface *pCriterion =
        mParameterMgrPlatformConnector.getSelectionCriterion(strCriterionName);

    if (!pCriterion) {

        strResult = "Unable to retrieve selection criterion: " + strCriterionName;

        return false;
    }

    // Get criterion type
    const ISelectionCriterionTypeInterface *pCriterionType = pCriterion->getCriterionType();

    // Get substate number, the first argument (index 0) is the criterion name
    size_t nbSubStates = remoteCommand.getArgumentCount() - 1;

    // Check that exclusive criterion has only one substate
    if (!pCriterionType->isTypeInclusive() && nbSubStates != 1) {

        strResult = "Exclusive criterion " + strCriterionName + " can only have one state";

        return false;
    }

    /// Translate lexical state to numerical state
    int iNumericalState = 0;
    size_t lexicalSubStateIndex;

    // Parse lexical substates
    std::string strLexicalState = "";

    for (lexicalSubStateIndex = 1; lexicalSubStateIndex <= nbSubStates; lexicalSubStateIndex++) {
        /*
         * getNumericalValue method from ISelectionCriterionTypeInterface strip his parameter
         * first parameter based on | sign. In case that the user uses multiple parameters
         * to set InclusiveCriterion value, we aggregate all desired values to be sure
         * they will be handled correctly.
         */
        if (lexicalSubStateIndex != 1) {
            strLexicalState += "|";
        }
        strLexicalState += remoteCommand.getArgument(lexicalSubStateIndex);
    }

    // Translate lexical to numerical substate
    if (!pCriterionType->getNumericalValue(strLexicalState, iNumericalState)) {

        strResult = "Unable to find lexical state \"" + strLexicalState + "\" in criteria " +
                    strCriterionName;

        return false;
    }

    // Set criterion new state
    pCriterion->setCriterionState(iNumericalState);

    return true;
}
