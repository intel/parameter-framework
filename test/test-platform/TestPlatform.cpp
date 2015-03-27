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

#include <iostream>
#include <sstream>
#include <assert.h>
#include "TestPlatform.h"
#include "ParameterMgrPlatformConnector.h"
#include "RemoteProcessorServer.h"

using std::string;

namespace test
{
namespace platform
{
namespace log
{

void log::CParameterMgrPlatformConnectorLogger::log(bool bIsWarning, const std::string& strLog)
{
    if (bIsWarning) {
        std::cerr << strLog << std::endl;
    } else {
        std::cout << strLog << std::endl;
    }
}

} /** log namespace */

CTestPlatform::CTestPlatform(const string& strClass, int iPortNumber, sem_t& exitSemaphore) :
    _pParameterMgrPlatformConnector(new CParameterMgrPlatformConnector(strClass)),
    _pParameterMgrPlatformConnectorLogger(new log::CParameterMgrPlatformConnectorLogger),
    _commandParser(*this),
    _portNumber(iPortNumber),
    _exitSemaphore(exitSemaphore)
{
    // Create server
    _pRemoteProcessorServer = new CRemoteProcessorServer(iPortNumber,
                                                         _commandParser.getCommandHandler());

    _pParameterMgrPlatformConnector->setLogger(_pParameterMgrPlatformConnectorLogger);
}

CTestPlatform::~CTestPlatform()
{
    delete _pRemoteProcessorServer;
    delete _pParameterMgrPlatformConnectorLogger;
    delete _pParameterMgrPlatformConnector;
}

bool CTestPlatform::load(std::string& strError)
{
    // Start remote processor server
    if (!_pRemoteProcessorServer->start()) {

	std::ostringstream oss;
        oss << "TestPlatform: Unable to start remote processor server on port " << _portNumber;
        strError = oss.str();

        return false;
    }

    return true;
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

} /** platform namespace */
} /** test namespace */
