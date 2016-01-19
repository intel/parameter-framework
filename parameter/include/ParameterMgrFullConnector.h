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
#pragma once

#include "parameter_export.h"

#include "SelectionCriterionTypeInterface.h"
#include "SelectionCriterionInterface.h"
#include "ParameterHandle.h"
#include "ParameterMgrLoggerForward.h"
#include "ParameterMgrPlatformConnector.h"
#include "CommandHandlerInterface.h"

#include <string>
#include <list>
#include <memory>
#include <stdint.h>

class CParameterMgr;

/** @note Since only C++11 components use this interface
 *        (contrary to the more restricted CParameterMgrPlatformConnector),
 *        could this api not be transition to c++11 ?
 */
class PARAMETER_EXPORT CParameterMgrFullConnector : public CParameterMgrPlatformConnector
{
public:
    /** String list type which can hold list of error/info and can be presented to client */
    typedef std::list<std::string> Results;

    CParameterMgrFullConnector(const std::string &strConfigurationFilePath);

    /** Create and return a command handler for this ParameterMgr instance
     *
     * The caller owns the returned pointer and is responsible for deleting it
     * before destroying the Connector object.
     *
     * @returns a Command Handler
     */
    CommandHandlerInterface *createCommandHandler();

    /** @deprecated Same as its overload without error handling.
     * @note this deprecated method in not available in the python wrapper.
     */
    void setFailureOnMissingSubsystem(bool bFail);
    using CParameterMgrPlatformConnector::setFailureOnMissingSubsystem;

    /** @deprecated Same as its overload without error handling.
     * @note this deprecated method in not available in the python wrapper.
     */
    void setFailureOnFailedSettingsLoad(bool bFail);
    using CParameterMgrPlatformConnector::setFailureOnFailedSettingsLoad;

    /** @deprecated Same as its overload without error handling.
     * @note this deprecated method in not available in the python wrapper.
     */
    void setValidateSchemasOnStart(bool bValidate);
    using CParameterMgrPlatformConnector::setValidateSchemasOnStart;

    //////////// Tuning /////////////
    // Tuning mode
    bool setTuningMode(bool bOn, std::string &strError);
    bool isTuningModeOn() const;

    // Current value space for user set/get value interpretation
    void setValueSpace(bool bIsRaw);
    bool isValueSpaceRaw() const;

    // Current Output Raw Format for user get value interpretation
    void setOutputRawFormat(bool bIsHex);
    bool isOutputRawFormatHex() const;
    // Automatic hardware synchronization control (during tuning session)
    bool setAutoSync(bool bAutoSyncOn, std::string &strError);
    bool isAutoSyncOn() const;
    bool sync(std::string &strError);

    // User set/get parameters
    bool accessParameterValue(const std::string &strPath, std::string &strValue, bool bSet,
                              std::string &strError);
    bool accessConfigurationValue(const std::string &strDomain, const std::string &strConfiguration,
                                  const std::string &strPath, std::string &strValue, bool bSet,
                                  std::string &strError);

    /**
     * Returns the element mapping corresponding to the path given in parameter.
     *
     * @param[in] strPath Path of an element
     * @param[out] strValue A sting containing the mapping
     *
     * @return true if a mapping was found for this element
     */
    bool getParameterMapping(const std::string &strPath, std::string &strValue) const;
    ////////// Configuration/Domains handling //////////////
    // Creation/Deletion
    bool createDomain(const std::string &strName, std::string &strError);
    bool deleteDomain(const std::string &strName, std::string &strError);
    bool renameDomain(const std::string &strName, const std::string &strNewName,
                      std::string &strError);
    bool deleteAllDomains(std::string &strError);
    bool setSequenceAwareness(const std::string &strName, bool bSequenceAware,
                              std::string &strResult);
    bool getSequenceAwareness(const std::string &strName, bool &bSequenceAware,
                              std::string &strResult);
    bool createConfiguration(const std::string &strDomain, const std::string &strConfiguration,
                             std::string &strError);
    bool deleteConfiguration(const std::string &strDomain, const std::string &strConfiguration,
                             std::string &strError);
    bool renameConfiguration(const std::string &strDomain, const std::string &strConfiguration,
                             const std::string &strNewConfiguration, std::string &strError);

    /** Restore a configuration
     *
     * @param[in] strDomain the domain name
     * @param[in] strConfiguration the configuration name
     * @param[out] errors, errors encountered during restoration
     * @return true if success false otherwise
     */
    bool restoreConfiguration(const std::string &strDomain, const std::string &strConfiguration,
                              Results &errors);

    bool saveConfiguration(const std::string &strDomain, const std::string &strConfiguration,
                           std::string &strError);

    // Configurable element - domain association
    bool addConfigurableElementToDomain(const std::string &strDomain,
                                        const std::string &strConfigurableElementPath,
                                        std::string &strError);
    bool removeConfigurableElementFromDomain(const std::string &strDomain,
                                             const std::string &strConfigurableElementPath,
                                             std::string &strError);
    bool split(const std::string &strDomain, const std::string &strConfigurableElementPath,
               std::string &strError);
    bool setElementSequence(const std::string &strDomain, const std::string &strConfiguration,
                            const std::vector<std::string> &astrNewElementSequence,
                            std::string &strError);

    bool setApplicationRule(const std::string &strDomain, const std::string &strConfiguration,
                            const std::string &strApplicationRule, std::string &strError);
    bool getApplicationRule(const std::string &strDomain, const std::string &strConfiguration,
                            std::string &strResult);
    bool clearApplicationRule(const std::string &strDomain, const std::string &strConfiguration,
                              std::string &strError);

    /**
      * Method that imports Configurable Domains from an Xml source.
      *
      * @param[in] strXmlSource a string containing an xml description or a path to an xml file
      * @param[in] bWithSettings a boolean that determines if the settings should be used in the
      * xml description
      * @param[in] bFromFile a boolean that determines if the source is an xml description in
      * strXmlSource or contained in a file. In that case strXmlSource is just the file path.
      * @param[out] strError is used as the error output
      *
      * @return false if any error occures
      */
    bool importDomainsXml(const std::string &strXmlSource, bool bWithSettings, bool bFromFile,
                          std::string &strError);
    /**
      * Method that imports a single Configurable Domain from an Xml source.
      *
      * @param[in] xmlSource a string containing an xml description or a path to an xml file
      * @param[in] overwrite when importing an existing domain, allow overwriting or return an
      * error
      * @param[in] withSettings a boolean that determines if the settings should be used in the
      * xml description
      * @param[in] fromFile a boolean that determines if the source is an xml description in
      * strXmlSource or contained in a file. In that case strXmlSource is just the file path.
      * @param[out] errorMsg is used as the error output
      *
      * @return false if any error occurs
      */
    bool importSingleDomainXml(const std::string &xmlSource, bool overwrite, bool withSettings,
                               bool fromFile, std::string &errorMsg);
    /**
      * Method that imports a single Configurable Domain from an string
      * describing an Xml source.
      *
      * @deprecated use the other versions of importSingleDomainXml instead
      *
      * @param[in] strXmlSource a string containing an xml description
      * @param[in] bOverwrite when importing an existing domain, allow overwriting or return an
      * error
      * @param[out] strError is used as the error output
      *
      * @return false if any error occurs
      */
    bool importSingleDomainXml(const std::string &strXmlSource, bool bOverwrite,
                               std::string &strError);

    /**
      * Method that exports Configurable Domains to an Xml destination.
      *
      * @param[in,out] strXmlDest a string containing an xml description or a path to an xml file
      * @param[in] bWithSettings a boolean that determines if the settings should be used in the
      * xml description
      * @param[in] bToFile a boolean that determines if the destination is an xml description in
      * strXmlDest or contained in a file. In that case strXmlDest is just the file path.
      * @param[out] strError is used as the error output
      *
      * @return false if any error occures, true otherwise.
      */
    bool exportDomainsXml(std::string &strXmlDest, bool bWithSettings, bool bToFile,
                          std::string &strError) const;

    /**
      * Method that exports a given Configurable Domain to an Xml destination.
      *
      * @param[in,out] strXmlDest a string containing an xml description or a path to an xml file
      * @param[in] strDomainName the name of the domain to be exported
      * @param[in] bWithSettings a boolean that determines if the settings should be used in the
      * xml description
      * @param[in] bToFile a boolean that determines if the destination is an xml description in
      * strXmlDest or contained in a file. In that case strXmlDest is just the file path.
      * @param[out] strError is used as the error output
      *
      * @return false if any error occurs, true otherwise.
      */
    bool exportSingleDomainXml(std::string &strXmlDest, const std::string &strDomainName,
                               bool bWithSettings, bool bToFile, std::string &strError) const;

private:
    // disallow copying because this class manages raw pointers' lifecycle
    CParameterMgrFullConnector(const CParameterMgrFullConnector &);
    CParameterMgrFullConnector &operator=(const CParameterMgrFullConnector &);
};
