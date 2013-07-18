/*  ParameterMgrPlatformConnector.h
 **
 ** Copyright © 2011 Intel
 **
 ** Licensed under the Apache License, Version 2.0 (the "License");
 ** you may not use this file except in compliance with the License.
 ** You may obtain a copy of the License at
 **
 **     http://www.apache.org/licenses/LICENSE-2.0
 **
 ** Unless required by applicable law or agreed to in writing, software
 ** distributed under the License is distributed on an "AS IS" BASIS,
 ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 ** See the License for the specific language governing permissions and
 ** limitations under the License.
 **
 ** AUTHOR: Patrick Benavoli (patrickx.benavoli@intel.com)
 ** CREATED: 2011-06-01
 ** UPDATED: 2011-07-27
 **
 */
#pragma once

#include "SelectionCriterionTypeInterface.h"
#include "SelectionCriterionInterface.h"
#include "ParameterHandle.h"

class CParameterMgr;
class CParameterMgrLogger;

class CParameterMgrPlatformConnector
{
    friend class CParameterMgrLogger;
public:
    // Logger interface
    class ILogger
    {
    public:
        virtual void log(bool bIsWarning, const std::string& strLog) = 0;
    protected:
        virtual ~ILogger() {}
    };

    // Construction
    CParameterMgrPlatformConnector(const std::string& strConfigurationFilePath);
    ~CParameterMgrPlatformConnector(); // Not virtual since not supposed to be derived!

    // Selection Criteria interface. Beware returned objects are lent, clients shall not delete them!
    // Should be called before start
    ISelectionCriterionTypeInterface* createSelectionCriterionType(bool bIsInclusive = false);
    ISelectionCriterionInterface* createSelectionCriterion(const std::string& strName, const ISelectionCriterionTypeInterface* pSelectionCriterionType);
    // Selection criterion retrieval
    ISelectionCriterionInterface* getSelectionCriterion(const std::string& strName) const;

    // Logging
    // Should be called before start
    void setLogger(ILogger* pLogger);

    // Start
    bool start(std::string& strError);

    // Started state
    bool isStarted() const;

    // Configuration application
    void applyConfigurations();

    // Dynamic parameter handling
    // Returned objects are owned by clients
    // Must be cassed after successfull start
    CParameterHandle* createParameterHandle(const std::string& strPath, std::string& strError) const;

    /** Should start fail in case of missing subsystems.
      *
      * Will fail if called on started instance.
      *
      * @param[in] bFail: If set to true,  parameterMgr start will fail on missing subsystems
      *                   If set to false, missing subsystems will fallbacks on virtual subsystem
      * @param[out] strError a string describing the error if the function failed,
                             unmodified otherwise.
      *
      * @return false if unable to set, true otherwise.
      */
    bool setFailureOnMissingSubsystem(bool bFail, std::string& strError);

    /** Would start fail in case of missing subsystems.
      *
      * @return if the subsystem load will fail on missing subsystem.
      */
    bool getFailureOnMissingSubsystem();

    /** Should start fail in failed settings load.
      *
      * Will fail if called on started instance.
      *
      * @param[in] bFail: If set to true, parameterMgr start will fail on failed settings load.
      *                   If set to false, failed settings load will be ignored.
      * @param[out] strResult a string containing the result of the command.
      *
      * @return false if unable to set, true otherwise.
      */
    bool setFailureOnFailedSettingsLoad(bool bFail, std::string& strError);
    /** Would start fail in case of failed settings load.
      *
      * @return failure on failed settings load policy state.
      */
    bool getFailureOnFailedSettingsLoad();

private:
    CParameterMgrPlatformConnector(const CParameterMgrPlatformConnector&);
    CParameterMgrPlatformConnector& operator=(const CParameterMgrPlatformConnector&);
    // Private logging
    void doLog(bool bIsWarning, const std::string& strLog);

    // Implementation
    CParameterMgr* _pParameterMgr;
    // State
    bool _bStarted;
    // Logging
    ILogger* _pLogger;
    // Private logging
    CParameterMgrLogger* _pParameterMgrLogger;
};

