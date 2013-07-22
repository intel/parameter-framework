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

#include "ParameterMgrPlatformConnector.h"
#include "RemoteCommandHandlerTemplate.h"
#include <string>
#include <list>

using namespace std;

class CParameterMgrPlatformConnectorLogger;
class CRemoteProcessorServer;
class ISelectionCriterionInterface;

class CTestPlatform
{
    typedef TRemoteCommandHandlerTemplate<CTestPlatform> CCommandHandler;
    typedef CCommandHandler::CommandStatus CommandReturn;
public:
    CTestPlatform(const string &strclass, int iPortNumber);
    virtual ~CTestPlatform();

    // Init
    bool load(string& strError);

private:
    //////////////// Remote command parsers
    /// Selection Criterion
    CommandReturn createExclusiveSelectionCriterionFromStateList(
            const IRemoteCommand& remoteCommand, string& strResult);
    CommandReturn createInclusiveSelectionCriterionFromStateList(
            const IRemoteCommand& remoteCommand, string& strResult);

    CommandReturn createExclusiveSelectionCriterion(
            const IRemoteCommand& remoteCommand, string& strResult);
    CommandReturn createInclusiveSelectionCriterion(
            const IRemoteCommand& remoteCommand, string& strResult);

    /** Callback to set a criterion's value, see ISelectionCriterionInterface::setCriterionState.
     * @see CCommandHandler::RemoteCommandParser for detail on each arguments and return
     *
     * @param[in] remoteCommand the first argument should be the name of the criterion to set.
     *                          if the criterion is provided in lexical space,
     *                              the folowing arguments should be criterion new values
     *                          if the criterion is provided in numerical space,
     *                              the second argument should be the criterion new value
     */
    CommandReturn setCriterionState(
            const IRemoteCommand& remoteCommand, string& strResult);

    /** Callback to start the PFW, see CParameterMgrPlatformConnector::start.
     * @see CCommandHandler::RemoteCommandParser for detail on each arguments and return
     *
     * @param[in] remoteCommand is ignored
     */
    CommandReturn startParameterMgr(
            const IRemoteCommand& remoteCommand, string& strResult);

    /** Callback to apply PFW configuration, see CParameterMgrPlatformConnector::applyConfiguration.
     * @see CCommandHandler::RemoteCommandParser for detail on each arguments and return
     *
     * @param[in] remoteCommand is ignored
     *
     * @return EDone (never fails)
     */
    CommandReturn applyConfigurations(
            const IRemoteCommand& remoteCommand, string& strResult);

    /** The type of a CParameterMgrPlatformConnector boolean setter. */
    typedef bool (CParameterMgrPlatformConnector::*setter_t)(bool, string&);
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
            const IRemoteCommand& remoteCommand, string& strResult);

    /** The type of a CParameterMgrPlatformConnector boolean getter. */
    typedef bool (CParameterMgrPlatformConnector::*getter_t)();
    /** Template callback to create a ParameterMgrPlatformConnector boolean getter callback.
     * @see CCommandHandler::RemoteCommandParser for detail on each arguments and return
     *
     * Convert to boolean returned by the template parameter function converted to a
     * string ("True", "False") and return it.
     *
     * @tparam the boolean getter method.
     * @param[in] remoteCommand is ignored
     *
     * @return EDone (never fails)
     */
    template<getter_t getFunction>
    CommandReturn getter(const IRemoteCommand& remoteCommand, string& strResult);

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

