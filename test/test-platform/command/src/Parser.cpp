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

#include "command/Parser.h"
#include "TestPlatform.h"
#include "RemoteProcessorServer.h"
#include "ParameterMgrPlatformConnector.h"
#include "convert.hpp"

namespace test
{
namespace platform
{
namespace command
{

Parser::CommandHandler::RemoteCommandParserItems Parser::gRemoteCommandParserItems = {
    { "exit",
      { &Parser::exit, 0, "",
        "Exit TestPlatform" } },
    { "createExclusiveCriterionFromStateList",
      { &Parser::createCriterionFromStateList<false>, 2, "<name> <stateList>",
          "Create inclusive selection criterion from state name list" } },
    { "createInclusiveCriterionFromStateList",
      { &Parser::createCriterionFromStateList<true>, 2, "<name> <stateList>",
        "Create exclusive selection criterion from state name list" } },
    { "createExclusiveCriterion",
      { &Parser::createCriterion<false>, 2, "<name> <nbStates>",
        "Create inclusive selection criterion" } },
    { "createInclusiveCriterion",
      { &Parser::createCriterion<true>, 2, "<name> <nbStates>",
        "Create exclusive selection criterion" } },
    { "start",
      { &Parser::startParameterMgr, 0, "",
        "Start ParameterMgr" } },
    { "setCriterionState",
      { &Parser::setCriterionState, 2, "<name> <state>",
        "Set the current state of a selection criterion" } },
    { "applyConfigurations",
      { &Parser::applyConfigurations, 0, "",
        "Apply configurations selected by current selection criteria states" } },
    { "setFailureOnMissingSubsystem",
      { &Parser::setter<& CParameterMgrPlatformConnector::setFailureOnMissingSubsystem>, 1,
        "true|false",
        "Set policy for missing subsystems, either abort start or fallback on virtual"
        " subsystem." } },
    { "getMissingSubsystemPolicy",
      { &Parser::getter<& CParameterMgrPlatformConnector::getFailureOnMissingSubsystem>, 0, "",
        "Get policy for missing subsystems, "
        "either abort start or fallback on virtual subsystem." } },
    { "setFailureOnFailedSettingsLoad",
      { &Parser::setter<& CParameterMgrPlatformConnector::setFailureOnFailedSettingsLoad>, 1,
        "true|false",
        "Set policy for failed settings load, either abort start or continue without domains." }},
    { "getFailedSettingsLoadPolicy",
      { &Parser::getter<& CParameterMgrPlatformConnector::getFailureOnFailedSettingsLoad>, 0, "",
        "Get policy for failed settings load, either abort start or continue without domains." }},
    { "setValidateSchemasOnStart",
      { &Parser::setter<& CParameterMgrPlatformConnector::setValidateSchemasOnStart>, 1,
        "true|false",
        "Set policy for schema validation based on .xsd files (false by default)." } },
    { "getValidateSchemasOnStart",
      { &Parser::getter<& CParameterMgrPlatformConnector::getValidateSchemasOnStart>, 0, "",
        "Get policy for schema validation based on .xsd files." } }
};

Parser::Parser(CTestPlatform& testPlatform) :
    mTestPlatform(testPlatform), mCommandHandler(CommandHandler(*this, gRemoteCommandParserItems))
{
}

Parser::CommandHandler* Parser::getCommandHandler()
{
    return &mCommandHandler;
}

Parser::CommandReturn Parser::exit(const IRemoteCommand&, std::string&)
{
    // Release the main blocking semaphore to quit application
    sem_post(&mTestPlatform._exitSemaphore);

    return Parser::CommandHandler::EDone;
}

template <bool isInclusive> Parser::CommandReturn
Parser::createCriterion(const IRemoteCommand& remoteCommand, std::string& result)
{
    uint32_t nbValues = 0;
    if(!convertTo(remoteCommand.getArgument(1), nbValues)) {
        return Parser::CommandHandler::EShowUsage;
    }
    return mTestPlatform.createCriterion<isInclusive>(remoteCommand.getArgument(0),
                                                      nbValues, result) ?
           Parser::CommandHandler::EDone : Parser::CommandHandler::EFailed;
}

template <bool isInclusive> Parser::CommandReturn
Parser::createCriterionFromStateList(const IRemoteCommand& remoteCommand, std::string& result)
{
    return mTestPlatform.createCriterion<isInclusive>(remoteCommand.getArgument(0),
                                                      remoteCommand, result) ?
           Parser::CommandHandler::EDone : Parser::CommandHandler::EFailed;
}

Parser::CommandReturn Parser::startParameterMgr(const IRemoteCommand&, std::string& result)
{
    return mTestPlatform._parameterMgrPlatformConnector.start(result) ?
           Parser::CommandHandler::EDone : Parser::CommandHandler::EFailed;
}

template <Parser::setter_t setFunction>
Parser::CommandReturn Parser::setter(const IRemoteCommand& remoteCommand, std::string& result)
{
    bool fail;
    if(!convertTo(remoteCommand.getArgument(0), fail)) {
        return Parser::CommandHandler::EShowUsage;
    }
    return (mTestPlatform._parameterMgrPlatformConnector.*setFunction)(fail, result) ?
           Parser::CommandHandler::EDone : Parser::CommandHandler::EFailed;
}

template <Parser::getter_t getFunction>
Parser::CommandReturn Parser::getter(const IRemoteCommand&, std::string& result)
{
    result = (mTestPlatform._parameterMgrPlatformConnector.*getFunction)() ? "true" : "false";
    return Parser::CommandHandler::ESucceeded;
}

Parser::CommandReturn Parser::setCriterionState(const IRemoteCommand& remoteCommand,
                                                std::string& result)
{
    return mTestPlatform.setCriterionState(remoteCommand.getArgument(0),
                                           remoteCommand,
                                           result) ?
           Parser::CommandHandler::EDone : Parser::CommandHandler::EFailed;
}

Parser::CommandReturn Parser::applyConfigurations(const IRemoteCommand&, std::string&)
{
    mTestPlatform._parameterMgrPlatformConnector.applyConfigurations();

    return Parser::CommandHandler::EDone;
}

} /** command namespace */
} /** platform namespace */
} /** test namespace */
