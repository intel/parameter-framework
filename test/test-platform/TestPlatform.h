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
#pragma once

#include "command/Parser.h"
#include "ParameterMgrPlatformConnector.h"
#include <string>
#include <semaphore.h>
#include <iostream>

class CRemoteProcessorServer;
class ISelectionCriterionInterface;

namespace test
{
namespace platform
{
namespace log
{

/** Logger exposed to the parameter-framework */
class CParameterMgrPlatformConnectorLogger : public CParameterMgrPlatformConnector::ILogger
{
public:
    CParameterMgrPlatformConnectorLogger() {}

    virtual void info(const std::string& log)
    {
        std::cout << log << std::endl;
    }

    virtual void warning(const std::string& log)
    {
        std::cerr << log << std::endl;
    }
};

} /** log namespace */

class CTestPlatform
{

    /** Remote command parser has access to private command handle function */
    friend class command::Parser;

public:
    CTestPlatform(const std::string &strclass, int iPortNumber, sem_t& exitSemaphore);
    virtual ~CTestPlatform();

    // Init
    bool load(std::string& strError);

private:

    // Commands
    // FIXME: IRemoteCommand object should be used only in command::Parser
    bool createExclusiveSelectionCriterionFromStateList(const std::string& strName, const IRemoteCommand& remoteCommand, std::string& strResult);
    bool createInclusiveSelectionCriterionFromStateList(const std::string& strName, const IRemoteCommand& remoteCommand, std::string& strResult);

    bool createExclusiveSelectionCriterion(const std::string& strName, uint32_t uiNbValues, std::string& strResult);
    bool createInclusiveSelectionCriterion(const std::string& strName, uint32_t uiNbValues, std::string& strResult);
    bool setCriterionState(const std::string& strName, uint32_t uiState, std::string& strResult);
    bool setCriterionStateByLexicalSpace(const IRemoteCommand& remoteCommand, std::string& strResult);

    // Connector
    CParameterMgrPlatformConnector* _pParameterMgrPlatformConnector;

    // Logger
    log::CParameterMgrPlatformConnectorLogger* _pParameterMgrPlatformConnectorLogger;

    /** Command Parser delegate */
    command::Parser _commandParser;

    // Remote Processor Server
    CRemoteProcessorServer* _pRemoteProcessorServer;

    // Port number for the server socket
    int _portNumber;

    // Semaphore used by calling thread to avoid exiting
    sem_t& _exitSemaphore;
};

} /** platform namespace */
} /** test namespace */
