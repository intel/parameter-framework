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
#pragma once

#include "RemoteCommandHandlerTemplate.h"
#include <string>
#include <list>

using namespace std;

class CParameterMgrPlatformConnector;
class CParameterMgrPlatformConnectorLogger;
class CRemoteProcessorServer;
class ISelectionCriterionInterface;

class CTestPlatform
{
    typedef TRemoteCommandHandlerTemplate<CTestPlatform> CCommandHandler;
public:
    CTestPlatform(const string &strclass, int iPortNumber);
    virtual ~CTestPlatform();

    // Init
    bool load(string& strError);

private:
    //////////////// Remote command parsers
    /// Selection Criterion
    CCommandHandler::CommandStatus createExclusiveSelectionCriterionFromStateListCommandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus createInclusiveSelectionCriterionFromStateListCommandProcess(const IRemoteCommand& remoteCommand, string& strResult);

    CCommandHandler::CommandStatus createExclusiveSelectionCriterionCommandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus createInclusiveSelectionCriterionCommandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus startParameterMgrCommandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus setCriterionStateCommandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus applyConfigurationsCommandProcess(const IRemoteCommand& remoteCommand, string& strResult);

    /** Callback to set if the PFW start should fail in case of missing subsystems.
     *
     * @param[in] remoteCommand contains the arguments of the received command.
     * @param[out] strResult a string containing the result of the command.
     *
     * @return CCommandHandler::ESucceeded if command succeeded
     *         or CCommandHandler::EFailed otherwise
     */
    CCommandHandler::CommandStatus setFailureOnMissingSubsystemCommandProcess(
            const IRemoteCommand& remoteCommand, string& strResult);
    /** Callback to get if the PFW start should fail in case of missing subsystems.
     *
     * @param[in] remoteCommand contains the arguments of the received command.
     * @param[out] strResult a string containing the result of the command.
     *
     * @return CCommandHandler::ESucceeded if command succeeded
     *         or CCommandHandler::EFailed otherwise
     */
    CCommandHandler::CommandStatus getFailureOnMissingSubsystemCommandProcess(
            const IRemoteCommand& remoteCommand, string& strResult);

    // Commands
    bool createExclusiveSelectionCriterionFromStateList(const string& strName, const IRemoteCommand& remoteCommand, string& strResult);
    bool createInclusiveSelectionCriterionFromStateList(const string& strName, const IRemoteCommand& remoteCommand, string& strResult);

    bool createExclusiveSelectionCriterion(const string& strName, uint32_t uiNbValues, string& strResult);
    bool createInclusiveSelectionCriterion(const string& strName, uint32_t uiNbValues, string& strResult);
    bool setCriterionState(const string& strName, uint32_t uiState, string& strResult);
    bool setCriterionStateByLexicalSpace(const IRemoteCommand& remoteCommand, string& strResult);

    // Connector
    CParameterMgrPlatformConnector* _pParameterMgrPlatformConnector;

    // Logger
    CParameterMgrPlatformConnectorLogger* _pParameterMgrPlatformConnectorLogger;

    // Command Handler
    CCommandHandler* _pCommandHandler;

    // Remote Processor Server
    CRemoteProcessorServer* _pRemoteProcessorServer;
};

