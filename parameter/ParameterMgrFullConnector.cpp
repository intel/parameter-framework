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
#include "ParameterMgrFullConnector.h"
#include "ParameterMgr.h"
#include "ParameterMgrLogger.h"

#include <list>

using std::string;

CParameterMgrFullConnector::CParameterMgrFullConnector(const string& strConfigurationFilePath) :
    _pParameterMgr(new CParameterMgr(strConfigurationFilePath)), _pLogger(NULL)
{
    _pParameterMgrLogger = new CParameterMgrLogger<CParameterMgrFullConnector>(*this);
    _pParameterMgr->setLogger(_pParameterMgrLogger);
}

CParameterMgrFullConnector::~CParameterMgrFullConnector()
{
    delete _pParameterMgr;
    delete _pParameterMgrLogger;
}


bool CParameterMgrFullConnector::start(string& strError)
{
    // Create data structure & Init flow
    return _pParameterMgr->load(strError);
}

void CParameterMgrFullConnector::setLogger(CParameterMgrFullConnector::ILogger* pLogger)
{
    _pLogger = pLogger;
}

// Private logging
void CParameterMgrFullConnector::doLog(bool bIsWarning, const string& strLog)
{
    if (_pLogger) {

        _pLogger->log(bIsWarning, strLog);
    }
}

CParameterHandle* CParameterMgrFullConnector::createParameterHandle(const string& strPath,
                                                                    string& strError)
{
    return _pParameterMgr->createParameterHandle(strPath, strError);
}

ISelectionCriterionTypeInterface* CParameterMgrFullConnector::createSelectionCriterionType(
        bool bIsInclusive)
{
    return _pParameterMgr->createSelectionCriterionType(bIsInclusive);
}

ISelectionCriterionInterface* CParameterMgrFullConnector::createSelectionCriterion(
        const string& strName,
        const ISelectionCriterionTypeInterface* pSelectionCriterionType)
{
    return _pParameterMgr->createSelectionCriterion(strName,
            static_cast<const CSelectionCriterionType*>(pSelectionCriterionType));
}

ISelectionCriterionInterface* CParameterMgrFullConnector::getSelectionCriterion(
        const string& strName)
{
    return _pParameterMgr->getSelectionCriterion(strName);
}

bool CParameterMgrFullConnector::getForceNoRemoteInterface() const
{
    return _pParameterMgr->getForceNoRemoteInterface();
}

void CParameterMgrFullConnector::setForceNoRemoteInterface(bool bForceNoRemoteInterface)
{
    _pParameterMgr->setForceNoRemoteInterface(bForceNoRemoteInterface);
}

void CParameterMgrFullConnector::applyConfigurations()
{
    return _pParameterMgr->applyConfigurations();
}

void CParameterMgrFullConnector::setFailureOnMissingSubsystem(bool bFail)
{
    _pParameterMgr->setFailureOnMissingSubsystem(bFail);
}

bool CParameterMgrFullConnector::getFailureOnMissingSubsystem() const
{
    return _pParameterMgr->getFailureOnMissingSubsystem();
}

void CParameterMgrFullConnector::setFailureOnFailedSettingsLoad(bool bFail)
{
    _pParameterMgr->setFailureOnFailedSettingsLoad(bFail);
}

bool CParameterMgrFullConnector::getFailureOnFailedSettingsLoad()
{
    return _pParameterMgr->getFailureOnFailedSettingsLoad();
}

const string& CParameterMgrFullConnector::getSchemaFolderLocation() const
{
    return _pParameterMgr->getSchemaFolderLocation();
}

void CParameterMgrFullConnector::setSchemaFolderLocation(const string& strSchemaFolderLocation)
{
    _pParameterMgr->setSchemaFolderLocation(strSchemaFolderLocation);
}

void CParameterMgrFullConnector::setValidateSchemasOnStart(bool bValidate)
{
    _pParameterMgr->setValidateSchemasOnStart(bValidate);
}

bool CParameterMgrFullConnector::getValidateSchemasOnStart() const
{
    return _pParameterMgr->getValidateSchemasOnStart();
}

bool CParameterMgrFullConnector::setTuningMode(bool bOn, string& strError)
{
    return _pParameterMgr->setTuningMode(bOn, strError);
}

bool CParameterMgrFullConnector::isTuningModeOn() const
{
    return _pParameterMgr->tuningModeOn();
}

void CParameterMgrFullConnector::setValueSpace(bool bIsRaw)
{
    return _pParameterMgr->setValueSpace(bIsRaw);
}

bool CParameterMgrFullConnector::isValueSpaceRaw() const
{
    return _pParameterMgr->valueSpaceIsRaw();
}

void CParameterMgrFullConnector::setOutputRawFormat(bool bIsHex)
{
    return _pParameterMgr->setOutputRawFormat(bIsHex);
}

bool CParameterMgrFullConnector::isOutputRawFormatHex() const
{
    return _pParameterMgr->outputRawFormatIsHex();
}

bool CParameterMgrFullConnector::setAutoSync(bool bAutoSyncOn, string& strError)
{
    return _pParameterMgr->setAutoSync(bAutoSyncOn, strError);
}

bool CParameterMgrFullConnector::isAutoSyncOn() const
{
    return _pParameterMgr->autoSyncOn();
}

bool CParameterMgrFullConnector::sync(string& strError)
{
    return _pParameterMgr->sync(strError);
}

bool CParameterMgrFullConnector::accessParameterValue(const string& strPath, string& strValue,
                                                      bool bSet, string& strError)
{
    return _pParameterMgr->accessParameterValue(strPath, strValue, bSet, strError);
}

bool CParameterMgrFullConnector::accessConfigurationValue(const string &strDomain,
                                                          const string &strConfiguration,
                                                          const string& strPath, string& strValue,
                                                          bool bSet, string& strError)
{
    return _pParameterMgr->accessConfigurationValue(strDomain, strConfiguration, strPath, strValue,
            bSet, strError);
}

bool CParameterMgrFullConnector::getParameterMapping(const string& strPath, string& strValue) const
{
    return _pParameterMgr->getParameterMapping(strPath, strValue);
}

bool CParameterMgrFullConnector::createDomain(const string& strName, string& strError)
{
    return _pParameterMgr->createDomain(strName, strError);
}

bool CParameterMgrFullConnector::deleteDomain(const string& strName, string& strError)
{
    return _pParameterMgr->deleteDomain(strName, strError);
}

bool CParameterMgrFullConnector::renameDomain(const string& strName, const string& strNewName,
                                              string& strError)
{
    return _pParameterMgr->renameDomain(strName, strNewName, strError);
}

bool CParameterMgrFullConnector::deleteAllDomains(string& strError)
{
    return _pParameterMgr->deleteAllDomains(strError);
}

bool CParameterMgrFullConnector::createConfiguration(const string& strDomain,
                                                     const string& strConfiguration,
                                                     string& strError)
{
    return _pParameterMgr->createConfiguration(strDomain, strConfiguration, strError);
}

bool CParameterMgrFullConnector::deleteConfiguration(const string& strDomain,
                                                     const string& strConfiguration,
                                                     string& strError)
{
    return _pParameterMgr->deleteConfiguration(strDomain, strConfiguration, strError);
}

bool CParameterMgrFullConnector::renameConfiguration(const string& strDomain,
                                                     const string& strConfiguration,
                                                     const string& strNewConfiguration,
                                                     string& strError)
{
    return _pParameterMgr->renameConfiguration(strDomain, strConfiguration, strNewConfiguration,
            strError);
}

bool CParameterMgrFullConnector::saveConfiguration(const string& strDomain,
                                                   const string& strConfiguration, string& strError)
{
    return _pParameterMgr->saveConfiguration(strDomain, strConfiguration, strError);
}

bool CParameterMgrFullConnector::restoreConfiguration(const string& strDomain,
                                                      const string& strConfiguration,
                                                      std::list<string>& lstrError)
{
    return _pParameterMgr->restoreConfiguration(strDomain, strConfiguration, lstrError);
}

bool CParameterMgrFullConnector::setSequenceAwareness(const string& strName, bool bSequenceAware,
                                                      string& strResult)
{
    return _pParameterMgr->setSequenceAwareness(strName, bSequenceAware, strResult);
}

bool CParameterMgrFullConnector::getSequenceAwareness(const string& strName, bool& bSequenceAware,
                                                      string& strResult)
{
    return _pParameterMgr->getSequenceAwareness(strName, bSequenceAware, strResult);
}

bool CParameterMgrFullConnector::addConfigurableElementToDomain(const string& strDomain,
        const string& strConfigurableElementPath, string& strError)
{
    return _pParameterMgr->addConfigurableElementToDomain(strDomain, strConfigurableElementPath,
            strError);
}

bool CParameterMgrFullConnector::removeConfigurableElementFromDomain(const string& strDomain,
        const string& strConfigurableElementPath, string& strError)
{
    return _pParameterMgr->removeConfigurableElementFromDomain(strDomain,
            strConfigurableElementPath, strError);
}

bool CParameterMgrFullConnector::split(const string& strDomain, 
                                       const string& strConfigurableElementPath, string& strError)
{
    return _pParameterMgr->split(strDomain, strConfigurableElementPath, strError);
}

bool CParameterMgrFullConnector::setElementSequence(const string& strDomain,
        const string& strConfiguration,
        const std::vector<string>& astrNewElementSequence,
        string& strError)
{
    return _pParameterMgr->setElementSequence(strDomain, strConfiguration, astrNewElementSequence,
            strError);
}

bool CParameterMgrFullConnector::setApplicationRule(const string& strDomain,
                                                    const string& strConfiguration,
                                                    const string& strApplicationRule,
                                                    string& strError)
{
    return _pParameterMgr->setApplicationRule(strDomain, strConfiguration, strApplicationRule,
            strError);
}


bool CParameterMgrFullConnector::getApplicationRule(const string& strDomain,
                                                    const string& strConfiguration,
                                                    string& strResult)
{
    return _pParameterMgr->getApplicationRule(strDomain, strConfiguration, strResult);
}
bool CParameterMgrFullConnector::clearApplicationRule(const string& strDomain,
                                                      const string& strConfiguration,
                                                      string& strError)
{
    return _pParameterMgr->clearApplicationRule(strDomain, strConfiguration, strError);
}


bool CParameterMgrFullConnector::importDomainsXml(const string& strXmlSource, bool bWithSettings,
                                                  bool bFromFile, string& strError)
{
    return _pParameterMgr->importDomainsXml(strXmlSource, bWithSettings, bFromFile, strError);
}

bool CParameterMgrFullConnector::exportDomainsXml(string& strXmlDest, bool bWithSettings,
                                                  bool bToFile, string& strError) const
{
    return _pParameterMgr->exportDomainsXml(strXmlDest, bWithSettings, bToFile, strError);
}

// deprecated, use the other version of importSingleDomainXml instead
bool CParameterMgrFullConnector::importSingleDomainXml(const string& strXmlSource, bool bOverwrite,
                                                       string& strError)
{
    return importSingleDomainXml(strXmlSource, bOverwrite, true, false, strError);
}

bool CParameterMgrFullConnector::importSingleDomainXml(const string& xmlSource, bool overwrite,
                                                       bool withSettings, bool fromFile,
                                                       string& errorMsg)
{
    return _pParameterMgr->importSingleDomainXml(xmlSource, overwrite, withSettings, fromFile,
                                                 errorMsg);
}

bool CParameterMgrFullConnector::exportSingleDomainXml(string& strXmlDest,
                                                       const string& strDomainName,
                                                       bool bWithSettings, bool bToFile,
                                                       string& strError) const
{
    return _pParameterMgr->exportSingleDomainXml(strXmlDest, strDomainName, bWithSettings, bToFile,
            strError);
}
