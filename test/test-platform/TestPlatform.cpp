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
using core::criterion::CriterionInterface;

class CParameterMgrPlatformConnectorLogger : public CParameterMgrPlatformConnector::ILogger
{
public:
    CParameterMgrPlatformConnectorLogger() {}

    virtual void info(const string& log)
    {
        std::cout << log << std::endl;
    }

    virtual void warning(const string& log)
    {
        std::cerr << log << std::endl;
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
        "createExclusiveCriterionFromStateList",
        &CTestPlatform::createExclusiveCriterionFromStateList,
        2, "<name> <stateList>",
        "Create inclusive selection criterion from state name list");
    _pCommandHandler->addCommandParser(
        "createInclusiveCriterionFromStateList",
        &CTestPlatform::createInclusiveCriterionFromStateList,
        2, "<name> <stateList>",
        "Create exclusive selection criterion from state name list");

    _pCommandHandler->addCommandParser(
        "createExclusiveCriterion",
        &CTestPlatform::createExclusiveCriterion,
        2, "<name> <nbStates>", "Create inclusive selection criterion");
    _pCommandHandler->addCommandParser(
        "createInclusiveCriterion",
        &CTestPlatform::createInclusiveCriterion,
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
/// Criterion
CTestPlatform::CommandReturn
CTestPlatform::createExclusiveCriterionFromStateList(const IRemoteCommand& remoteCommand,
                                                     string& strResult)
{
    return createExclusiveCriterionFromStateList(remoteCommand.getArgument(0),
                                                 remoteCommand, strResult) ?
           CTestPlatform::CCommandHandler::EDone : CTestPlatform::CCommandHandler::EFailed;
}

CTestPlatform::CommandReturn CTestPlatform::createInclusiveCriterionFromStateList(
    const IRemoteCommand& remoteCommand, string& strResult)
{
    return createInclusiveCriterionFromStateList(remoteCommand.getArgument(0),
                                                 remoteCommand, strResult) ?
           CTestPlatform::CCommandHandler::EDone : CTestPlatform::CCommandHandler::EFailed;
}

CTestPlatform::CommandReturn
CTestPlatform::createExclusiveCriterion(const IRemoteCommand& remoteCommand,
                                        string& strResult)
{
    return createExclusiveCriterion(remoteCommand.getArgument(0),
                                    strtoul(remoteCommand.getArgument(1).c_str(), NULL, 0),
                                    strResult) ?
           CTestPlatform::CCommandHandler::EDone : CTestPlatform::CCommandHandler::EFailed;
}

CTestPlatform::CommandReturn
CTestPlatform::createInclusiveCriterion(const IRemoteCommand& remoteCommand, string& strResult)
{
    return createInclusiveCriterion(remoteCommand.getArgument(0),
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
    // Get criterion name
    std::string strCriterionName = remoteCommand.getArgument(0);

    CriterionInterface* pCriterion =
        _pParameterMgrPlatformConnector->getCriterion(strCriterionName);

    if (!pCriterion) {

        strResult = "Unable to retrieve selection criterion: " + strCriterionName;

        return CTestPlatform::CCommandHandler::EFailed;
    }

    // Get substate number, the first argument (index 0) is the criterion name
    uint32_t uiNbSubStates = remoteCommand.getArgumentCount() - 1;

    // Check that exclusive criterion has only one substate
    if (!pCriterion->isInclusive() && uiNbSubStates != 1) {

        strResult = "Exclusive criterion " + strCriterionName + " can only have one state";

        return CTestPlatform::CCommandHandler::EFailed;
    }

    core::criterion::State state{};
    for (uint32_t i = 1; i <= uiNbSubStates; i++) {
        state.emplace(remoteCommand.getArgument(i));
    }

    // Set criterion new state
    if (!pCriterion->setState(state, strResult)) {
        return CTestPlatform::CCommandHandler::EFailed;
    }

    return CTestPlatform::CCommandHandler::EDone;
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

bool CTestPlatform::createExclusiveCriterionFromStateList(const string& name,
                                                          const IRemoteCommand& remoteCommand,
                                                          string& result)
{

    assert(_pParameterMgrPlatformConnector != NULL);

    uint32_t nbStates = remoteCommand.getArgumentCount() - 1;

    using namespace core::criterion;
    Values values;

    for (uint32_t state = 0; state < nbStates; state++) {

        const std::string& value = remoteCommand.getArgument(state + 1);
        values.emplace_back(value);
    }

    CriterionInterface* criterion =
        _pParameterMgrPlatformConnector->createExclusiveCriterion(name, values, result);

    if (criterion == nullptr) {
        return false;
    }

    return true;
}

bool CTestPlatform::createInclusiveCriterionFromStateList(const string& name,
                                                          const IRemoteCommand& remoteCommand,
                                                          string& result)
{
    assert(_pParameterMgrPlatformConnector != NULL);
    uint32_t nbStates = remoteCommand.getArgumentCount() - 1;

    using namespace core::criterion;
    Values values;

    for (uint32_t state = 0; state < nbStates; state++) {

        const std::string& value = remoteCommand.getArgument(state + 1);
        values.emplace_back(value);
    }

    CriterionInterface* criterion =
        _pParameterMgrPlatformConnector->createInclusiveCriterion(name, values, result);

    if (criterion == nullptr) {
        return false;
    }

    return true;
}


bool CTestPlatform::createExclusiveCriterion(const string& name, uint32_t nbStates, string& result)
{
    using namespace core::criterion;
    Values values;

    for (uint32_t state = 0; state < nbStates; state++) {
        values.emplace_back("State_" + std::to_string(state));
    }
    CriterionInterface* criterion =
        _pParameterMgrPlatformConnector->createExclusiveCriterion(name, values, result);

    if (criterion == nullptr) {
        return false;
    }

    return true;
}

bool CTestPlatform::createInclusiveCriterion(const string& name, uint32_t nbStates, string& result)
{
    using namespace core::criterion;
    Values values;

    if (nbStates > 32) {

        result = "Maximum number of states for inclusive criterion is 32";

        return false;
    }

    for (uint32_t state = 0; state < nbStates; state++) {
        values.emplace_back("State_0x" + std::to_string(1 << state));
    }
    CriterionInterface* criterion =
        _pParameterMgrPlatformConnector->createInclusiveCriterion(name, values, result);

    if (criterion == nullptr) {
        return false;
    }

    return true;
}
