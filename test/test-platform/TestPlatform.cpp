/*
 * Copyright (c) 2011-2014, Intel Corporation
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

#include <strings.h>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <assert.h>
#include <errno.h>
#include <convert.hpp>
#include <sstream>
#include "TestPlatform.h"
#include "ParameterMgrPlatformConnector.h"
#include "RemoteProcessorServer.h"

using std::string;

class CParameterMgrPlatformConnectorLogger : public CParameterMgrPlatformConnector::ILogger
{
public:
    CParameterMgrPlatformConnectorLogger() {}

    virtual void log(bool bIsWarning, const string& strLog)
    {

        if (bIsWarning) {

	    std::cerr << strLog << std::endl;
        } else {

	    std::cout << strLog << std::endl;
        }
    }
};

CTestPlatform::CTestPlatform(const string& strClass, int iPortNumber, sem_t& exitSemaphore) :
    _pParameterMgrPlatformConnector(new CParameterMgrPlatformConnector(strClass)),
    _pParameterMgrPlatformConnectorLogger(new CParameterMgrPlatformConnectorLogger),
    _exitSemaphore(exitSemaphore)
{
    _pCommandHandler = new CCommandHandler(this);

    // Add command parsers
    _pCommandHandler->addCommandParser("exit", &CTestPlatform::exit,
                                       0, "", "Exit TestPlatform");
    _pCommandHandler->addCommandParser(
        "createExclusiveSelectionCriterionFromStateList",
        &CTestPlatform::createExclusiveSelectionCriterionFromStateList,
        2, "<name> <stateList>",
        "Create inclusive selection criterion from state name list");
    _pCommandHandler->addCommandParser(
        "createInclusiveSelectionCriterionFromStateList",
        &CTestPlatform::createInclusiveSelectionCriterionFromStateList,
        2, "<name> <stateList>",
        "Create exclusive selection criterion from state name list");

    _pCommandHandler->addCommandParser(
        "createExclusiveSelectionCriterion",
        &CTestPlatform::createExclusiveSelectionCriterion,
        2, "<name> <nbStates>", "Create inclusive selection criterion");
    _pCommandHandler->addCommandParser(
        "createInclusiveSelectionCriterion",
        &CTestPlatform::createInclusiveSelectionCriterion,
        2, "<name> <nbStates>", "Create exclusive selection criterion");

    _pCommandHandler->addCommandParser("start", &CTestPlatform::startParameterMgr,
                                       0, "", "Start ParameterMgr");

    _pCommandHandler->addCommandParser("setCriterionState", &CTestPlatform::setCriterionState,
                                       2, "<name> <state>",
                                       "Set the current state of a selection criterion");
    _pCommandHandler->addCommandParser(
        "applyConfigurations",
        &CTestPlatform::applyConfigurations,
        0, "", "Apply configurations selected by current selection criteria states");

    _pCommandHandler->addCommandParser(
        "setFailureOnMissingSubsystem",
        &CTestPlatform::setter<& CParameterMgrPlatformConnector::setFailureOnMissingSubsystem>,
        1, "true|false", "Set policy for missing subsystems, "
                         "either abort start or fallback on virtual subsystem.");
    _pCommandHandler->addCommandParser(
        "getMissingSubsystemPolicy",
        &CTestPlatform::getter<& CParameterMgrPlatformConnector::getFailureOnMissingSubsystem>,
        0, "", "Get policy for missing subsystems, "
               "either abort start or fallback on virtual subsystem.");

    _pCommandHandler->addCommandParser(
        "setFailureOnFailedSettingsLoad",
        &CTestPlatform::setter<& CParameterMgrPlatformConnector::setFailureOnFailedSettingsLoad>,
        1, "true|false",
        "Set policy for failed settings load, either abort start or continue without domains.");
    _pCommandHandler->addCommandParser(
        "getFailedSettingsLoadPolicy",
        &CTestPlatform::getter<& CParameterMgrPlatformConnector::getFailureOnFailedSettingsLoad>,
        0, "",
        "Get policy for failed settings load, either abort start or continue without domains.");

    _pCommandHandler->addCommandParser(
        "setValidateSchemasOnStart",
        &CTestPlatform::setter<& CParameterMgrPlatformConnector::setValidateSchemasOnStart>,
        1, "true|false",
        "Set policy for schema validation based on .xsd files (false by default).");
    _pCommandHandler->addCommandParser(
        "getValidateSchemasOnStart",
        &CTestPlatform::getter<& CParameterMgrPlatformConnector::getValidateSchemasOnStart>,
        0, "",
        "Get policy for schema validation based on .xsd files.");

    // Create server
    _pRemoteProcessorServer = new CRemoteProcessorServer(iPortNumber, _pCommandHandler);

    _pParameterMgrPlatformConnector->setLogger(_pParameterMgrPlatformConnectorLogger);
}

CTestPlatform::~CTestPlatform()
{
    delete _pRemoteProcessorServer;
    delete _pCommandHandler;
    delete _pParameterMgrPlatformConnectorLogger;
    delete _pParameterMgrPlatformConnector;
}

CTestPlatform::CommandReturn CTestPlatform::exit(
    const IRemoteCommand& remoteCommand, string& strResult)
{
    (void)remoteCommand;

    // Release the main blocking semaphore to quit application
    sem_post(&_exitSemaphore);

    return CTestPlatform::CCommandHandler::EDone;
}

bool CTestPlatform::load(std::string& strError)
{
    // Start remote processor server
    if (!_pRemoteProcessorServer->start(strError)) {

        strError = "TestPlatform: Unable to start remote processor server: " + strError;
        return false;
    }

    return true;
}

//////////////// Remote command parsers
/// Selection Criterion
CTestPlatform::CommandReturn CTestPlatform::createExclusiveSelectionCriterionFromStateList(
    const IRemoteCommand& remoteCommand, string& strResult)
{
    return createExclusiveSelectionCriterionFromStateList(
        remoteCommand.getArgument(0), remoteCommand, strResult) ?
           CTestPlatform::CCommandHandler::EDone : CTestPlatform::CCommandHandler::EFailed;
}

CTestPlatform::CommandReturn CTestPlatform::createInclusiveSelectionCriterionFromStateList(
    const IRemoteCommand& remoteCommand, string& strResult)
{
    return createInclusiveSelectionCriterionFromStateList(
        remoteCommand.getArgument(0), remoteCommand, strResult) ?
           CTestPlatform::CCommandHandler::EDone : CTestPlatform::CCommandHandler::EFailed;
}

CTestPlatform::CommandReturn CTestPlatform::createExclusiveSelectionCriterion(
    const IRemoteCommand& remoteCommand, string& strResult)
{
    return createExclusiveSelectionCriterion(
        remoteCommand.getArgument(0),
        strtoul(remoteCommand.getArgument(1).c_str(), NULL, 0),
        strResult) ?
           CTestPlatform::CCommandHandler::EDone : CTestPlatform::CCommandHandler::EFailed;
}

CTestPlatform::CommandReturn CTestPlatform::createInclusiveSelectionCriterion(
    const IRemoteCommand& remoteCommand, string& strResult)
{
    return createInclusiveSelectionCriterion(
        remoteCommand.getArgument(0),
        strtoul(remoteCommand.getArgument(1).c_str(), NULL, 0),
        strResult) ?
           CTestPlatform::CCommandHandler::EDone : CTestPlatform::CCommandHandler::EFailed;
}

CTestPlatform::CommandReturn CTestPlatform::startParameterMgr(
    const IRemoteCommand& remoteCommand, string& strResult)
{
    (void)remoteCommand;

    return _pParameterMgrPlatformConnector->start(strResult) ?
           CTestPlatform::CCommandHandler::EDone : CTestPlatform::CCommandHandler::EFailed;
}

template <CTestPlatform::setter_t setFunction>
CTestPlatform::CommandReturn CTestPlatform::setter(
    const IRemoteCommand& remoteCommand, string& strResult)
{
    const string& strAbort = remoteCommand.getArgument(0);

    bool bFail;

    if(!convertTo(strAbort, bFail)) {
        return CTestPlatform::CCommandHandler::EShowUsage;
    }

    return (_pParameterMgrPlatformConnector->*setFunction)(bFail, strResult) ?
           CTestPlatform::CCommandHandler::EDone : CTestPlatform::CCommandHandler::EFailed;
}

template <CTestPlatform::getter_t getFunction>
CTestPlatform::CommandReturn CTestPlatform::getter(
    const IRemoteCommand& remoteCommand, string& strResult)
{
    (void)remoteCommand;

    strResult = (_pParameterMgrPlatformConnector->*getFunction)() ? "true" : "false";

    return CTestPlatform::CCommandHandler::ESucceeded;
}

CTestPlatform::CommandReturn CTestPlatform::setCriterionState(
    const IRemoteCommand& remoteCommand, string& strResult)
{

    bool bSuccess;

    const char* pcState = remoteCommand.getArgument(1).c_str();

    char* pcStrEnd;

    // Reset errno to check if it is updated during the conversion (strtol/strtoul)
    errno = 0;

    uint32_t state = strtoul(pcState, &pcStrEnd, 0);

    if (!errno && (*pcStrEnd == '\0')) {
        // Sucessfull conversion, set criterion state by numerical state
        bSuccess = setCriterionState(remoteCommand.getArgument(0), state, strResult);

    } else {
        // Conversion failed, set criterion state by lexical state
        bSuccess = setCriterionStateByLexicalSpace(remoteCommand, strResult);
    }

    return bSuccess ? CTestPlatform::CCommandHandler::EDone : CTestPlatform::CCommandHandler::
           EFailed;

}

CTestPlatform::CommandReturn CTestPlatform::applyConfigurations(const IRemoteCommand& remoteCommand,
                                                                string& strResult)
{
    (void)remoteCommand;
    (void)strResult;

    _pParameterMgrPlatformConnector->applyConfigurations();

    return CTestPlatform::CCommandHandler::EDone;
}

//////////////// Remote command handlers

bool CTestPlatform::createExclusiveSelectionCriterionFromStateList(
                                                                const string& strName,
                                                                const IRemoteCommand& remoteCommand,
                                                                string& strResult)
{

    assert(_pParameterMgrPlatformConnector != NULL);

    ISelectionCriterionTypeInterface* pCriterionType =
        _pParameterMgrPlatformConnector->createSelectionCriterionType(false);

    assert(pCriterionType != NULL);

    uint32_t uiNbStates = remoteCommand.getArgumentCount() - 1;
    uint32_t uiState;

    for (uiState = 0; uiState < uiNbStates; uiState++) {

        const std::string& strValue = remoteCommand.getArgument(uiState + 1);

        if (!pCriterionType->addValuePair(uiState, strValue)) {

            strResult = "Unable to add value: " + strValue;

            return false;
        }
    }

    _pParameterMgrPlatformConnector->createSelectionCriterion(strName, pCriterionType);

    return true;
}

bool CTestPlatform::createInclusiveSelectionCriterionFromStateList(
                                                                const string& strName,
                                                                const IRemoteCommand& remoteCommand,
                                                                string& strResult)
{
    assert(_pParameterMgrPlatformConnector != NULL);

    ISelectionCriterionTypeInterface* pCriterionType =
        _pParameterMgrPlatformConnector->createSelectionCriterionType(true);

    assert(pCriterionType != NULL);

    uint32_t uiNbStates = remoteCommand.getArgumentCount() - 1;

    if (uiNbStates > 32) {

        strResult = "Maximum number of states for inclusive criterion is 32";

        return false;
    }

    uint32_t uiState;

    for (uiState = 0; uiState < uiNbStates; uiState++) {

        const std::string& strValue = remoteCommand.getArgument(uiState + 1);

        if (!pCriterionType->addValuePair(0x1 << uiState, strValue)) {

            strResult = "Unable to add value: " + strValue;

            return false;
        }
    }

    _pParameterMgrPlatformConnector->createSelectionCriterion(strName, pCriterionType);

    return true;
}


bool CTestPlatform::createExclusiveSelectionCriterion(const string& strName,
                                                      uint32_t uiNbStates,
                                                      string& strResult)
{
    ISelectionCriterionTypeInterface* pCriterionType =
        _pParameterMgrPlatformConnector->createSelectionCriterionType(false);

    uint32_t uistate;

    for (uistate = 0; uistate < uiNbStates; uistate++) {

	std::ostringstream ostrValue;

        ostrValue << "State_";
        ostrValue << uistate;

        if (!pCriterionType->addValuePair(uistate, ostrValue.str())) {

            strResult = "Unable to add value: " + ostrValue.str();

            return false;
        }
    }

    _pParameterMgrPlatformConnector->createSelectionCriterion(strName, pCriterionType);

    return true;
}

bool CTestPlatform::createInclusiveSelectionCriterion(const string& strName,
                                                      uint32_t uiNbStates,
                                                      string& strResult)
{
    ISelectionCriterionTypeInterface* pCriterionType =
        _pParameterMgrPlatformConnector->createSelectionCriterionType(true);

    if (uiNbStates > 32) {

        strResult = "Maximum number of states for inclusive criterion is 32";

        return false;
    }

    uint32_t uiState;

    for (uiState = 0; uiState < uiNbStates; uiState++) {

	std::ostringstream ostrValue;

        ostrValue << "State_0x";
        ostrValue << (0x1 << uiState);

        if (!pCriterionType->addValuePair(0x1 << uiState, ostrValue.str())) {

            strResult = "Unable to add value: " + ostrValue.str();

            return false;
        }
    }

    _pParameterMgrPlatformConnector->createSelectionCriterion(strName, pCriterionType);

    return true;
}

bool CTestPlatform::setCriterionState(const string& strName, uint32_t uiState, string& strResult)
{
    ISelectionCriterionInterface* pCriterion =
        _pParameterMgrPlatformConnector->getSelectionCriterion(strName);

    if (!pCriterion) {

        strResult = "Unable to retrieve selection criterion: " + strName;

        return false;
    }

    pCriterion->setCriterionState(uiState);

    return true;
}

bool CTestPlatform::setCriterionStateByLexicalSpace(const IRemoteCommand& remoteCommand,
                                                    string& strResult)
{

    // Get criterion name
    std::string strCriterionName = remoteCommand.getArgument(0);

    ISelectionCriterionInterface* pCriterion =
        _pParameterMgrPlatformConnector->getSelectionCriterion(strCriterionName);

    if (!pCriterion) {

        strResult = "Unable to retrieve selection criterion: " + strCriterionName;

        return false;
    }

    // Get criterion type
    const ISelectionCriterionTypeInterface* pCriterionType = pCriterion->getCriterionType();

    // Get substate number, the first argument (index 0) is the criterion name
    uint32_t uiNbSubStates = remoteCommand.getArgumentCount() - 1;

    // Check that exclusive criterion has only one substate
    if (!pCriterionType->isTypeInclusive() && uiNbSubStates != 1) {

        strResult = "Exclusive criterion " + strCriterionName + " can only have one state";

        return false;
    }

    /// Translate lexical state to numerical state
    int iNumericalState = 0;
    uint32_t uiLexicalSubStateIndex;

    // Parse lexical substates
    std::string strLexicalState = "";

    for (uiLexicalSubStateIndex = 1;
         uiLexicalSubStateIndex <= uiNbSubStates;
         uiLexicalSubStateIndex++) {
        /*
         * getNumericalValue method from ISelectionCriterionTypeInterface strip his parameter
         * first parameter based on | sign. In case that the user uses multiple parameters
         * to set InclusiveCriterion value, we aggregate all desired values to be sure
         * they will be handled correctly.
         */
        if (uiLexicalSubStateIndex != 1) {
            strLexicalState += "|";
        }
        strLexicalState += remoteCommand.getArgument(uiLexicalSubStateIndex);
    }

    // Translate lexical to numerical substate
    if (!pCriterionType->getNumericalValue(strLexicalState, iNumericalState)) {

        strResult = "Unable to find lexical state \""
            + strLexicalState + "\" in criteria " + strCriterionName;

        return false;
    }

    // Set criterion new state
    pCriterion->setCriterionState(iNumericalState);

    return true;
}
