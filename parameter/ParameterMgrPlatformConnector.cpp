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
#include "ParameterMgrPlatformConnector.h"
#include "ParameterMgr.h"
#include "ParameterMgrLogger.h"
#include <assert.h>

using std::string;

// Construction
CParameterMgrPlatformConnector::CParameterMgrPlatformConnector(
        const string& strConfigurationFilePath) :
    _pParameterMgr(new CParameterMgr(strConfigurationFilePath)), _bStarted(false), _pLogger(NULL)
{
    // Logging
    _pParameterMgrLogger = new CParameterMgrLogger<CParameterMgrPlatformConnector>(*this);
    _pParameterMgr->setLogger(_pParameterMgrLogger);
}

CParameterMgrPlatformConnector::~CParameterMgrPlatformConnector()
{
    delete _pParameterMgr;
    delete _pParameterMgrLogger;
}

// Selection Criteria interface. Beware returned objects are lent, clients shall not delete them!
ISelectionCriterionTypeInterface* CParameterMgrPlatformConnector::createSelectionCriterionType(bool bIsInclusive)
{
    assert(!_bStarted);

    return _pParameterMgr->createSelectionCriterionType(bIsInclusive);
}

ISelectionCriterionInterface* CParameterMgrPlatformConnector::createSelectionCriterion(const string& strName, const ISelectionCriterionTypeInterface* pSelectionCriterionType)
{
    assert(!_bStarted);

    return _pParameterMgr->createSelectionCriterion(strName, static_cast<const CSelectionCriterionType*>(pSelectionCriterionType));
}

// Selection criterion retrieval
ISelectionCriterionInterface* CParameterMgrPlatformConnector::getSelectionCriterion(const string& strName) const
{
    return _pParameterMgr->getSelectionCriterion(strName);
}

// Configuration application
void CParameterMgrPlatformConnector::applyConfigurations()
{
    assert(_bStarted);

    _pParameterMgr->applyConfigurations();
}

// Dynamic parameter handling
CParameterHandle* CParameterMgrPlatformConnector::createParameterHandle(const string& strPath, string& strError) const
{
    assert(_bStarted);

    return _pParameterMgr->createParameterHandle(strPath, strError);
}

// Logging
void CParameterMgrPlatformConnector::setLogger(CParameterMgrPlatformConnector::ILogger* pLogger)
{
    _pLogger = pLogger;
}

bool CParameterMgrPlatformConnector::getForceNoRemoteInterface() const
{
    return _pParameterMgr->getForceNoRemoteInterface();
}

void CParameterMgrPlatformConnector::setForceNoRemoteInterface(bool bForceNoRemoteInterface)
{
    _pParameterMgr->setForceNoRemoteInterface(bForceNoRemoteInterface);
}

bool CParameterMgrPlatformConnector::setFailureOnMissingSubsystem(bool bFail, string &strError)
{
    if (_bStarted) {

        strError = "Can not set missing subsystem policy while running";
        return false;
    }

    _pParameterMgr->setFailureOnMissingSubsystem(bFail);
    return true;
}

bool CParameterMgrPlatformConnector::getFailureOnMissingSubsystem()
{
    return _pParameterMgr->getFailureOnMissingSubsystem();
}

bool CParameterMgrPlatformConnector::setFailureOnFailedSettingsLoad(
        bool bFail, std::string& strError)
{
    if (_bStarted) {

        strError = "Can not set failure on failed settings load policy while running";
        return false;
    }

    _pParameterMgr->setFailureOnFailedSettingsLoad(bFail);
    return true;
}

bool CParameterMgrPlatformConnector::getFailureOnFailedSettingsLoad()
{
    return _pParameterMgr->getFailureOnFailedSettingsLoad();
}

const string& CParameterMgrPlatformConnector::getSchemaFolderLocation() const
{
    return _pParameterMgr->getSchemaFolderLocation();
}

void CParameterMgrPlatformConnector::setSchemaFolderLocation(const string& strSchemaFolderLocation)
{
    _pParameterMgr->setSchemaFolderLocation(strSchemaFolderLocation);
}

bool CParameterMgrPlatformConnector::setValidateSchemasOnStart(
    bool bValidate, std::string& strError)
{
    if (_bStarted) {

        strError = "Can not enable xml validation after the start of the parameter-framework";
        return false;
    }

    _pParameterMgr->setValidateSchemasOnStart(bValidate);
    return true;
}

bool CParameterMgrPlatformConnector::getValidateSchemasOnStart()
{
    return _pParameterMgr->getValidateSchemasOnStart();
}

// Start
bool CParameterMgrPlatformConnector::start(string& strError)
{
    // Create data structure
    if (!_pParameterMgr->load(strError)) {

        return false;
    }

    _bStarted = true;

    return true;
}

// Started state
bool CParameterMgrPlatformConnector::isStarted() const
{
    return _bStarted;
}

// Private logging
void CParameterMgrPlatformConnector::doLog(bool bIsWarning, const string& strLog)
{
    if (_pLogger) {

        _pLogger->log(bIsWarning, strLog);
    }
}
