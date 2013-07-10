/* 
 * INTEL CONFIDENTIAL
 * Copyright © 2011 Intel 
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
 * disclosed in any way without Intel’s prior express written permission.
 * 
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Intel in writing.
 * 
 * CREATED: 2011-06-01
 * UPDATED: 2011-07-27
 */
#include "ParameterMgrPlatformConnector.h"
#include "ParameterMgr.h"
#include "ParameterMgrLogger.h"
#include <assert.h>

// Construction
CParameterMgrPlatformConnector::CParameterMgrPlatformConnector(
        const string& strConfigurationFilePath) :
    _pParameterMgr(new CParameterMgr(strConfigurationFilePath)), _bStarted(false), _pLogger(NULL)
{
    // Logging
    _pParameterMgrLogger = new CParameterMgrLogger(this);
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

// Start
bool CParameterMgrPlatformConnector::start(string& strError)
{
    // Create data structure
    if (!_pParameterMgr->load(strError)) {

        return false;
    }

    // Init flow
    if (!_pParameterMgr->init(strError)) {

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
