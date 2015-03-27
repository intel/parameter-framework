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
#include "RemoteProcessorServer.h"
#include <assert.h>
#include <string>
#include <semaphore.h>
#include <iostream>

class CRemoteProcessorServer;

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

    /** @see callback with the same arguments for details and other parameters
     *
     * @tparam isInclusive true for creating inclusive criterion, false for an exclusive one
     *
     * @param[in] name the criterion name
     * @param[out] result useful information that client may want to retrieve
     * @return true if success, false otherwise
     *
     * FIXME: remote command should only appear in parser, we need an easy way
     *        to retrieve a vector of argument
     */
    template <bool isInclusive>
    bool createCriterion(const std::string& name,
                         const IRemoteCommand& command,
                         std::string& result)
    {
        assert(_pParameterMgrPlatformConnector != NULL);

        uint32_t nbStates = command.getArgumentCount() - 1;

        core::criterion::Values values;
        for (uint32_t state = 0; state < nbStates; state++) {

            const std::string& value = command.getArgument(state + 1);
            values.emplace_back(value);
        }
        return createCriterion<isInclusive>(name, values, result);
    }

    /** Create a criterion by generating a given number of values
     *
     * @tparam isInclusive true for creating inclusive criterion, false for an exclusive one
     *
     * @param[in] name the criterion name
     * @param[in] nbValues number of possible state value the criterion can have
     * @param[out] result useful information that client may want to retrieve
     * @return true if success, false otherwise
     */
    template <bool isInclusive>
    bool createCriterion(const std::string& name, uint32_t nbValues, std::string& result)
    {
        core::criterion::Values values;
        for (uint32_t state = 0; state < nbValues; state++) {
            // Generate value names, those name are legacy and should be uniformized
            // after functionnal tests rework
            values.emplace_back((isInclusive ? "State_0x" + std::to_string(state + 1) :
                                               "State_" + std::to_string(state)));
        }
        return createCriterion<isInclusive>(name, values, result);
    }

    /** Create a criterion with desired values
     * @tparam isInclusive true for creating inclusive criterion, false for an exclusive one
     *
     * @param[in] name the criterion name
     * @param[in] values criterion desired values
     * @param[out] result useful information that client may want to retrieve
     * @return true if success, false otherwise
     */
    template <bool isInclusive>
    bool createCriterion(const std::string& name,
                         const core::criterion::Values &values,
                         std::string& result)
    {
        core::criterion::Criterion* criterion = (isInclusive ?
            _pParameterMgrPlatformConnector->createInclusiveCriterion(name, values, result) :
            _pParameterMgrPlatformConnector->createExclusiveCriterion(name, values, result));

        if (criterion == nullptr) {
            return false;
        }

        return true;
    }

    /** @see callback with the same arguments for details and other parameters
     *
     * @param[in] name the criterion name
     * @param[out] result useful information that client may want to retrieve
     * @return true if success, false otherwise
     *
     * FIXME: remote command should only appear in parser, we need an easy way
     *        to retrieve a vector of argument
     */
    bool setCriterionState(std::string criterionName,
                           const IRemoteCommand& remoteCommand,
                           std::string& strResult);

    // Connector
    CParameterMgrPlatformConnector* _pParameterMgrPlatformConnector;

    // Logger
    log::CParameterMgrPlatformConnectorLogger* _pParameterMgrPlatformConnectorLogger;

    /** Command Parser delegate */
    command::Parser _commandParser;

    // Remote Processor Server
    CRemoteProcessorServer* _pRemoteProcessorServer;

    // Semaphore used by calling thread to avoid exiting
    sem_t& _exitSemaphore;
};

} /** platform namespace */
} /** test namespace */
