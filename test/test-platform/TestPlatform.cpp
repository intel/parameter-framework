/*
 * INTEL CONFIDENTIAL
 * Copyright  2011 Intel
 * Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its suppliers
 * or licensors. Title to the Material remains with Intel Corporation or its
 * suppliers and licensors. The Material contains trade secrets and proprietary
 * and confidential information of Intel or its suppliers and licensors. The
 * Material is protected by worldwide copyright and trade secret laws and
 * treaty provisions. No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intels prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Intel in writing.
 *
 * CREATED: 2011-11-25
 * UPDATED: 2011-11-25
 */
#include <strings.h>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <assert.h>
#include <errno.h>
#include "TestPlatform.h"
#include "ParameterMgrPlatformConnector.h"
#include "RemoteProcessorServer.h"

class CParameterMgrPlatformConnectorLogger : public CParameterMgrPlatformConnector::ILogger
{
public:
    CParameterMgrPlatformConnectorLogger() {}

    virtual void log(bool bIsWarning, const string& strLog) {

        if (bIsWarning) {

            cerr << strLog << endl;
        } else {

            cout << strLog << endl;
        }
    }
};


CTestPlatform::CTestPlatform(const string& strClass, int iPortNumber) :
    _pParameterMgrPlatformConnector(new CParameterMgrPlatformConnector(strClass)),
    _pParameterMgrPlatformConnectorLogger(new CParameterMgrPlatformConnectorLogger)
{
    _pCommandHandler = new CCommandHandler(this);

    // Add command parsers
    _pCommandHandler->addCommandParser("createExclusiveSelectionCriterionFromStateList", &CTestPlatform::createExclusiveSelectionCriterionFromStateListCommandProcess, 2, "<name> <stateList>", "Create inclusive selection criterion from state name list");
    _pCommandHandler->addCommandParser("createInclusiveSelectionCriterionFromStateList", &CTestPlatform::createInclusiveSelectionCriterionFromStateListCommandProcess, 2, "<name> <stateList>", "Create exclusive selection criterion from state name list");

    _pCommandHandler->addCommandParser("createExclusiveSelectionCriterion", &CTestPlatform::createExclusiveSelectionCriterionCommandProcess, 2, "<name> <nbStates>", "Create inclusive selection criterion");
    _pCommandHandler->addCommandParser("createInclusiveSelectionCriterion", &CTestPlatform::createInclusiveSelectionCriterionCommandProcess, 2, "<name> <nbStates>", "Create exclusive selection criterion");

    _pCommandHandler->addCommandParser("start", &CTestPlatform::startParameterMgrCommandProcess, 0, "", "Start ParameterMgr");
    _pCommandHandler->addCommandParser("setCriterionState", &CTestPlatform::setCriterionStateCommandProcess, 2, "<name> <state>", "Set the current state of a selection criterion");
    _pCommandHandler->addCommandParser("applyConfigurations", &CTestPlatform::applyConfigurationsCommandProcess, 0, "", "Apply configurations selected by current selection criteria states");

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

bool CTestPlatform::load(std::string& strError)
{
    // Start remote processor server
    if (!_pRemoteProcessorServer->start()) {

        strError = "Unable to start remote processor server";

        return false;
    }

    return true;
}

//////////////// Remote command parsers
/// Selection Criterion
CTestPlatform::CCommandHandler::CommandStatus CTestPlatform::createExclusiveSelectionCriterionFromStateListCommandProcess(const IRemoteCommand& remoteCommand, string& strResult)
{
    return createExclusiveSelectionCriterionFromStateList(remoteCommand.getArgument(0), remoteCommand, strResult) ?
            CTestPlatform::CCommandHandler::EDone : CTestPlatform::CCommandHandler::EFailed;
}

CTestPlatform::CCommandHandler::CommandStatus CTestPlatform::createInclusiveSelectionCriterionFromStateListCommandProcess(const IRemoteCommand& remoteCommand, string& strResult)
{
    return createInclusiveSelectionCriterionFromStateList(remoteCommand.getArgument(0), remoteCommand, strResult) ?
            CTestPlatform::CCommandHandler::EDone : CTestPlatform::CCommandHandler::EFailed;
}

CTestPlatform::CCommandHandler::CommandStatus CTestPlatform::createExclusiveSelectionCriterionCommandProcess(const IRemoteCommand& remoteCommand, string& strResult)
{
    return createExclusiveSelectionCriterion(remoteCommand.getArgument(0), strtoul(remoteCommand.getArgument(1).c_str(), NULL, 0), strResult) ?
            CTestPlatform::CCommandHandler::EDone : CTestPlatform::CCommandHandler::EFailed;
}

CTestPlatform::CCommandHandler::CommandStatus CTestPlatform::createInclusiveSelectionCriterionCommandProcess(const IRemoteCommand& remoteCommand, string& strResult)
{
    return createInclusiveSelectionCriterion(remoteCommand.getArgument(0), strtoul(remoteCommand.getArgument(1).c_str(), NULL, 0), strResult) ?
            CTestPlatform::CCommandHandler::EDone : CTestPlatform::CCommandHandler::EFailed;
}

CTestPlatform::CCommandHandler::CommandStatus CTestPlatform::startParameterMgrCommandProcess(const IRemoteCommand& remoteCommand, string& strResult)
{
    (void)remoteCommand;

    return _pParameterMgrPlatformConnector->start(strResult) ?
            CTestPlatform::CCommandHandler::EDone : CTestPlatform::CCommandHandler::EFailed;
}

CTestPlatform::CCommandHandler::CommandStatus CTestPlatform::setCriterionStateCommandProcess(const IRemoteCommand& remoteCommand, string& strResult)
{

    bool bSuccess;

    const char * pcState = remoteCommand.getArgument(1).c_str();

    char *pcStrEnd;

    // Reset errno to check if it is updated during the conversion (strtol/strtoul)
    errno = 0;

    uint32_t state = strtoul(pcState , &pcStrEnd, 0);

    if (!errno && (*pcStrEnd == '\0')) {
        // Sucessfull conversion, set criterion state by numerical state
        bSuccess = setCriterionState(remoteCommand.getArgument(0), state , strResult ) ;

    } else {
        // Conversion failed, set criterion state by lexical state
        bSuccess = setCriterionStateByLexicalSpace(remoteCommand , strResult );
    }

    return bSuccess ? CTestPlatform::CCommandHandler::EDone : CTestPlatform::CCommandHandler::EFailed;

}

CTestPlatform::CCommandHandler::CommandStatus CTestPlatform::applyConfigurationsCommandProcess(const IRemoteCommand& remoteCommand, string& strResult)
{
    (void)remoteCommand;
    (void)strResult;

    _pParameterMgrPlatformConnector->applyConfigurations();

    return CTestPlatform::CCommandHandler::EDone;
}

//////////////// Remote command handlers

bool CTestPlatform::createExclusiveSelectionCriterionFromStateList(const string& strName, const IRemoteCommand& remoteCommand, string& strResult)
{

    assert (_pParameterMgrPlatformConnector != NULL);

    ISelectionCriterionTypeInterface* pCriterionType = _pParameterMgrPlatformConnector->createSelectionCriterionType(false);

    assert(pCriterionType != NULL);

    uint32_t uiNbStates = remoteCommand.getArgumentCount() - 1 ;
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

bool CTestPlatform::createInclusiveSelectionCriterionFromStateList(const string& strName, const IRemoteCommand& remoteCommand, string& strResult)
{
    assert (_pParameterMgrPlatformConnector != NULL);

    ISelectionCriterionTypeInterface* pCriterionType = _pParameterMgrPlatformConnector->createSelectionCriterionType(true);

    assert(pCriterionType != NULL);

    uint32_t uiNbStates = remoteCommand.getArgumentCount() - 1 ;

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


bool CTestPlatform::createExclusiveSelectionCriterion(const string& strName, uint32_t uiNbStates, string& strResult)
{
    ISelectionCriterionTypeInterface* pCriterionType = _pParameterMgrPlatformConnector->createSelectionCriterionType(false);

    uint32_t uistate;

    for (uistate = 0; uistate < uiNbStates; uistate++) {

        ostringstream ostrValue;

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

bool CTestPlatform::createInclusiveSelectionCriterion(const string& strName, uint32_t uiNbStates, string& strResult)
{
    ISelectionCriterionTypeInterface* pCriterionType = _pParameterMgrPlatformConnector->createSelectionCriterionType(true);

    if (uiNbStates > 32) {

        strResult = "Maximum number of states for inclusive criterion is 32";

        return false;
    }

    uint32_t uiState;

    for (uiState = 0; uiState < uiNbStates; uiState++) {

        ostringstream ostrValue;

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
    ISelectionCriterionInterface* pCriterion = _pParameterMgrPlatformConnector->getSelectionCriterion(strName);

    if (!pCriterion) {

        strResult = "Unable to retrieve selection criterion: " + strName;

        return false;
    }

    pCriterion->setCriterionState(uiState);

    return true;
}

bool CTestPlatform::setCriterionStateByLexicalSpace(const IRemoteCommand& remoteCommand, string& strResult)
{

    // Get criterion name
    std::string strCriterionName = remoteCommand.getArgument(0);

    ISelectionCriterionInterface* pCriterion = _pParameterMgrPlatformConnector->getSelectionCriterion(strCriterionName);

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
    uint32_t uiNumericalState = 0;
    uint32_t uiLexicalSubStateIndex;

    // Parse lexical substates
    for (uiLexicalSubStateIndex = 1; uiLexicalSubStateIndex <= uiNbSubStates; uiLexicalSubStateIndex++) {

        int iNumericalSubState;

        const std::string& strLexicalSubState = remoteCommand.getArgument(uiLexicalSubStateIndex);

        // Translate lexical to numerical substate
        if (!pCriterionType->getNumericalValue(strLexicalSubState, iNumericalSubState)) {

            strResult = "Unable to find lexical state \"" + strLexicalSubState + "\" in criteria " + strCriterionName;

            return false;
        }

        // Aggregate numerical substates
        uiNumericalState |= iNumericalSubState;
    }

    // Set criterion new state
    pCriterion->setCriterionState(uiNumericalState);

    return true;
}
