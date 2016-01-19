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

#include "parameter_export.h"

#include "SelectionCriterionTypeInterface.h"
#include "SelectionCriterionInterface.h"
#include "ParameterHandle.h"
#include "ElementHandle.h"
#include "ParameterMgrLoggerForward.h"

class CParameterMgr;

class PARAMETER_EXPORT CParameterMgrPlatformConnector
{
    friend class CParameterMgrLogger<CParameterMgrPlatformConnector>;

public:
    /** Interface to implement to provide a custom logger to the PF.
     *
     * Override info and warning methods to specify how each log level
     * should be printed.
     *
     * @note Errors are always returned synchronously. Never logged.
     */
    class ILogger
    {
    public:
        virtual void info(const std::string &strLog) = 0;
        virtual void warning(const std::string &strLog) = 0;

    protected:
        virtual ~ILogger() {}
    };

    // Construction
    CParameterMgrPlatformConnector(const std::string &strConfigurationFilePath);
    virtual ~CParameterMgrPlatformConnector();

    // Selection Criteria interface. Beware returned objects are lent, clients shall not delete
    // them!
    // Should be called before start
    ISelectionCriterionTypeInterface *createSelectionCriterionType(bool bIsInclusive = false);
    ISelectionCriterionInterface *createSelectionCriterion(
        const std::string &strName,
        const ISelectionCriterionTypeInterface *pSelectionCriterionType);
    // Selection criterion retrieval
    ISelectionCriterionInterface *getSelectionCriterion(const std::string &strName) const;

    // Logging
    // Should be called before start
    void setLogger(ILogger *pLogger);

    // Start
    bool start(std::string &strError);

    // Started state
    bool isStarted() const;

    // Configuration application
    void applyConfigurations();

    // Dynamic parameter handling
    // Returned objects are owned by clients
    // Must be cassed after successfull start
    CParameterHandle *createParameterHandle(const std::string &strPath,
                                            std::string &strError) const;

    /** Creates a handle to a configurable element.
     *
     * The returned object is owned by the client who is responsible to delete it.
     *
     * @param[in] path A string representing a path to a configurable element.
     * @param[out] error On error: an human readable error message
     *                   On success: undefined
     *
     * @return An element handle on success
     *         NULL on error
     */
    ElementHandle *createElementHandle(const std::string &path, std::string &error) const;

    /** Is the remote interface forcefully disabled ?
     */
    bool getForceNoRemoteInterface() const;

    /**
     * Forcefully disable the remote interface or cancel this policy.
     *
     * Has no effect if called after calling start().
     *
     * @param[in] bForceNoRemoteInterface disable the remote interface if true.
     */
    void setForceNoRemoteInterface(bool bForceNoRemoteInterface);

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
    bool setFailureOnMissingSubsystem(bool bFail, std::string &strError);

    /** Would start fail in case of missing subsystems.
      *
      * @return if the subsystem load will fail on missing subsystem.
      */
    bool getFailureOnMissingSubsystem() const;

    /** Should start fail in failed settings load.
      *
      * Will fail if called on started instance.
      *
      * @param[in] bFail If set to true, parameterMgr start will fail on failed settings load.
      *                  If set to false, failed settings load will be ignored.
      * @param[out] strError On error: an human readable error message
      *                      On success: undefined
      *
      * @return false if unable to set, true otherwise.
      */
    bool setFailureOnFailedSettingsLoad(bool bFail, std::string &strError);
    /** Would start fail in case of failed settings load.
      *
      * @return failure on failed settings load policy state.
      */
    bool getFailureOnFailedSettingsLoad() const;

    /** Get the XML Schemas URI
     *
     * @returns the XML Schemas URI
     */
    const std::string &getSchemaUri() const;

    /** Override the XML Schemas URI
     *
     * @param[in] schemaUri the XML Schemas URI
     */
    void setSchemaUri(const std::string &schemaUri);

    /** Should .xml files be validated on start ?
     *
     * @param[in] bValidate:
     *     If set to true, parameterMgr will abort when being unable to validate .xml files
     *     If set to false, no .xml/xsd validation will happen (default behaviour)
     * @param[out] strError On error: an human readable error message
     *                      On success: undefined
     *
     * @return false if unable to set, true otherwise.
     */
    bool setValidateSchemasOnStart(bool bValidate, std::string &strError);

    /** Would .xml files be validated on start?
     *
     * @return areSchemasValidated
     */
    bool getValidateSchemasOnStart() const;

private:
    CParameterMgrPlatformConnector(const CParameterMgrPlatformConnector &);
    CParameterMgrPlatformConnector &operator=(const CParameterMgrPlatformConnector &);
    // Private logging
    void info(const std::string &log);
    void warning(const std::string &log);

protected:
    // Private logging
    CParameterMgrLogger<CParameterMgrPlatformConnector> *_pParameterMgrLogger;
    // Implementation
    CParameterMgr *_pParameterMgr;
    // State
    bool _bStarted;
    // Logging
    ILogger *_pLogger;
};
