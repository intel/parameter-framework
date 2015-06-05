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
#pragma once

#include "ParameterMgrPlatformConnector.h"
#include "RemoteCommandHandlerTemplate.h"
#include <string>
#include <list>
#include <semaphore.h>

class CParameterMgrPlatformConnectorLogger;
class CRemoteProcessorServer;
class ISelectionCriterionInterface;

class CTestPlatform
{
    typedef TRemoteCommandHandlerTemplate<CTestPlatform> CCommandHandler;
    typedef CCommandHandler::CommandStatus CommandReturn;
public:
    CTestPlatform(const std::string &strclass, int iPortNumber, sem_t& exitSemaphore);
    virtual ~CTestPlatform();

    // Init
    bool load(std::string& strError);

private:
    //////////////// Remote command parsers
    /// Selection Criterion
    CommandReturn createExclusiveSelectionCriterionFromStateList(
            const IRemoteCommand& remoteCommand, std::string& strResult);
    CommandReturn createInclusiveSelectionCriterionFromStateList(
            const IRemoteCommand& remoteCommand, std::string& strResult);

    CommandReturn createExclusiveSelectionCriterion(
            const IRemoteCommand& remoteCommand, std::string& strResult);
    CommandReturn createInclusiveSelectionCriterion(
            const IRemoteCommand& remoteCommand, std::string& strResult);

    /** Callback to set a criterion's value, see ISelectionCriterionInterface::setCriterionState.
     * @see CCommandHandler::RemoteCommandParser for detail on each arguments and return
     *
     * @param[in] remoteCommand the first argument should be the name of the criterion to set.
     *                          if the criterion is provided in lexical space,
     *                              the following arguments should be criterion new values
     *                          if the criterion is provided in numerical space,
     *                              the second argument should be the criterion new value
     */
    CommandReturn setCriterionState(
            const IRemoteCommand& remoteCommand, std::string& strResult);

    /** Callback to start the PFW, see CParameterMgrPlatformConnector::start.
     * @see CCommandHandler::RemoteCommandParser for detail on each arguments and return
     *
     * @param[in] remoteCommand is ignored
     */
    CommandReturn startParameterMgr(
            const IRemoteCommand& remoteCommand, std::string& strResult);

    /** Callback to apply PFW configuration, see CParameterMgrPlatformConnector::applyConfiguration.
     * @see CCommandHandler::RemoteCommandParser for detail on each arguments and return
     *
     * @param[in] remoteCommand is ignored
     *
     * @return EDone (never fails)
     */
    CommandReturn applyConfigurations(
            const IRemoteCommand& remoteCommand, std::string& strResult);

    /** Callback to exit the test-platform.
     *
     * @param[in] remoteCommand is ignored
     *
     * @return EDone (never fails)
     */
    CommandReturn exit(const IRemoteCommand& remoteCommand, std::string& strResult);

    /** The type of a CParameterMgrPlatformConnector boolean setter. */
    typedef bool (CParameterMgrPlatformConnector::*setter_t)(bool, std::string&);
    /** Template callback to create a _pParameterMgrPlatformConnector boolean setter callback.
     * @see CCommandHandler::RemoteCommandParser for detail on each arguments and return
     *
     * Convert the remoteCommand first argument to a boolean and call the
     * template parameter function with this value.
     *
     * @tparam the boolean setter method.
     * @param[in] remoteCommand the first argument should be ether "on" or "off".
     */
    template<setter_t setFunction>
    CommandReturn setter(
            const IRemoteCommand& remoteCommand, std::string& strResult);

    /** The type of a CParameterMgrPlatformConnector boolean getter. */
    typedef bool (CParameterMgrPlatformConnector::*getter_t)();
    /** Template callback to create a ParameterMgrPlatformConnector boolean getter callback.
     * @see CCommandHandler::RemoteCommandParser for detail on each arguments and return
     *
     * Convert to boolean returned by the template parameter function converted to a
     * std::string ("True", "False") and return it.
     *
     * @param the boolean getter method.
     * @param[in] remoteCommand is ignored
     *
     * @return EDone (never fails)
     */
    template<getter_t getFunction>
    CommandReturn getter(const IRemoteCommand& remoteCommand, std::string& strResult);

    // Commands
    bool createExclusiveSelectionCriterionFromStateList(const std::string& strName, const IRemoteCommand& remoteCommand, std::string& strResult);
    bool createInclusiveSelectionCriterionFromStateList(const std::string& strName, const IRemoteCommand& remoteCommand, std::string& strResult);

    bool createExclusiveSelectionCriterion(const std::string& strName, uint32_t uiNbValues, std::string& strResult);
    bool createInclusiveSelectionCriterion(const std::string& strName, uint32_t uiNbValues, std::string& strResult);
    bool setCriterionState(const std::string& strName, uint32_t uiState, std::string& strResult);
    bool setCriterionStateByLexicalSpace(const IRemoteCommand& remoteCommand, std::string& strResult);

    // Connector
    CParameterMgrPlatformConnector* _pParameterMgrPlatformConnector;

    // Logger
    CParameterMgrPlatformConnectorLogger* _pParameterMgrPlatformConnectorLogger;

    // Command Handler
    CCommandHandler* _pCommandHandler;

    // Remote Processor Server
    CRemoteProcessorServer* _pRemoteProcessorServer;

    // Semaphore used by calling thread to avoid exiting
    sem_t& _exitSemaphore;
};

