/* <auto_header>
 * <FILENAME>
 *
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
 *  AUTHOR: Patrick Benavoli (patrickx.benavoli@intel.com)
 * CREATED: 2011-06-01
 * UPDATED: 2011-07-27
 *
 *
 * </auto_header>
 */
#include "ParameterHandle.h"
#include "ParameterAccessContext.h"
#include "BaseParameter.h"
#include "Subsystem.h"
#include <assert.h>
#include "ParameterMgr.h"
#include "AutoLock.h"

CParameterHandle::CParameterHandle(const CBaseParameter* pParameter, CParameterMgr* pParameterMgr)
    : _pBaseParameter(pParameter), _pParameterMgr(pParameterMgr), _bBigEndianSubsystem(pParameter->getBelongingSubsystem()->isBigEndian())
{
}

// Parameter features
bool CParameterHandle::isRogue() const
{
    return _pBaseParameter->isRogue();
}

bool CParameterHandle::isArray() const
{
    return !_pBaseParameter->isScalar();
}

// Array Length
uint32_t CParameterHandle::getArrayLength() const
{
    return _pBaseParameter->getArrayLength();
}

// Parameter path
string CParameterHandle::getPath() const
{
    return _pBaseParameter->getPath();
}

// Parameter kind
string CParameterHandle::getKind() const
{
    return _pBaseParameter->getKind();
}

// Boolean access
bool CParameterHandle::setAsBoolean(bool bValue, string& strError)
{
    // Check operation validity
    if (!checkAccessValidity(true, 0, strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    CAutoLock autoLock(_pParameterMgr->getBlackboardMutex());

    // When in tuning mode, silently skip the request
    if (_pParameterMgr->tuningModeOn()) {

        return true;
    }

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    return _pBaseParameter->accessAsBoolean(bValue, true, parameterAccessContext);
}

bool CParameterHandle::getAsBoolean(bool bValue, string& strError) const
{
    // Check operation validity
    if (!checkAccessValidity(false, 0, strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    CAutoLock autoLock(_pParameterMgr->getBlackboardMutex());

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    return _pBaseParameter->accessAsBoolean(bValue, false, parameterAccessContext);
}

bool CParameterHandle::setAsBooleanArray(const vector<bool>& abValues, string& strError)
{
    // Check operation validity
    if (!checkAccessValidity(true, abValues.size(), strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    CAutoLock autoLock(_pParameterMgr->getBlackboardMutex());

    // When in tuning mode, silently skip the request
    if (_pParameterMgr->tuningModeOn()) {

        return true;
    }

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    // Copy values for type adaptation
    vector<bool> abUserValues = abValues;

    return _pBaseParameter->accessAsBooleanArray(abUserValues, true, parameterAccessContext);
}

bool CParameterHandle::getAsBooleanArray(vector<bool>& abValues, string& strError) const
{
    // Check operation validity
    if (!checkAccessValidity(false, -1, strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    CAutoLock autoLock(_pParameterMgr->getBlackboardMutex());

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    return _pBaseParameter->accessAsBooleanArray(abValues, false, parameterAccessContext);
}

// Integer Access
bool CParameterHandle::setAsInteger(uint32_t uiValue, string& strError)
{
    // Check operation validity
    if (!checkAccessValidity(true, 0, strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    CAutoLock autoLock(_pParameterMgr->getBlackboardMutex());

    // When in tuning mode, silently skip the request
    if (_pParameterMgr->tuningModeOn()) {

        return true;
    }

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    return _pBaseParameter->accessAsInteger(uiValue, true, parameterAccessContext);
}

bool CParameterHandle::getAsInteger(uint32_t& uiValue, string& strError) const
{
    // Check operation validity
    if (!checkAccessValidity(false, 0, strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    CAutoLock autoLock(_pParameterMgr->getBlackboardMutex());

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    return _pBaseParameter->accessAsInteger(uiValue, false, parameterAccessContext);
}

bool CParameterHandle::setAsIntegerArray(const vector<uint32_t>& auiValues, string& strError)
{
    // Check operation validity
    if (!checkAccessValidity(true, auiValues.size(), strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    CAutoLock autoLock(_pParameterMgr->getBlackboardMutex());

    // When in tuning mode, silently skip the request
    if (_pParameterMgr->tuningModeOn()) {

        return true;
    }

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    // Copy values for type adaptation
    vector<uint32_t> auiUserValues = auiValues;

    return _pBaseParameter->accessAsIntegerArray(auiUserValues, true, parameterAccessContext);
}

bool CParameterHandle::getAsIntegerArray(vector<uint32_t>& auiValues, string& strError) const
{
    // Check operation validity
    if (!checkAccessValidity(false, -1, strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    CAutoLock autoLock(_pParameterMgr->getBlackboardMutex());

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    return _pBaseParameter->accessAsIntegerArray(auiValues, false, parameterAccessContext);
}

// Signed Integer Access
bool CParameterHandle::setAsSignedInteger(int32_t iValue, string& strError)
{
    // Check operation validity
    if (!checkAccessValidity(true, 0, strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    CAutoLock autoLock(_pParameterMgr->getBlackboardMutex());

    // When in tuning mode, silently skip the request
    if (_pParameterMgr->tuningModeOn()) {

        return true;
    }

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    return _pBaseParameter->accessAsSignedInteger(iValue, true, parameterAccessContext);
}

bool CParameterHandle::getAsSignedInteger(int32_t& iValue, string& strError) const
{
    // Check operation validity
    if (!checkAccessValidity(false, 0, strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    CAutoLock autoLock(_pParameterMgr->getBlackboardMutex());

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    return _pBaseParameter->accessAsSignedInteger(iValue, false, parameterAccessContext);
}

bool CParameterHandle::setAsSignedIntegerArray(const vector<int32_t>& aiValues, string& strError)
{
    // Check operation validity
    if (!checkAccessValidity(true, aiValues.size(), strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    CAutoLock autoLock(_pParameterMgr->getBlackboardMutex());

    // When in tuning mode, silently skip the request
    if (_pParameterMgr->tuningModeOn()) {

        return true;
    }

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    // Copy values for type adaptation
    vector<int32_t> aiUserValues = aiValues;

    return _pBaseParameter->accessAsSignedIntegerArray(aiUserValues, true, parameterAccessContext);
}

bool CParameterHandle::getAsSignedIntegerArray(vector<int32_t>& aiValues, string& strError) const
{
    // Check operation validity
    if (!checkAccessValidity(false, -1, strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    CAutoLock autoLock(_pParameterMgr->getBlackboardMutex());

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    return _pBaseParameter->accessAsSignedIntegerArray(aiValues, false, parameterAccessContext);
}

// Double Access
bool CParameterHandle::setAsDouble(double dValue, string& strError)
{
    // Check operation validity
    if (!checkAccessValidity(true, 0, strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    CAutoLock autoLock(_pParameterMgr->getBlackboardMutex());

    // When in tuning mode, silently skip the request
    if (_pParameterMgr->tuningModeOn()) {

        return true;
    }

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    return _pBaseParameter->accessAsDouble(dValue, true, parameterAccessContext);
}

bool CParameterHandle::getAsDouble(double& dValue, string& strError) const
{
    // Check operation validity
    if (!checkAccessValidity(false, 0, strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    CAutoLock autoLock(_pParameterMgr->getBlackboardMutex());

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    return _pBaseParameter->accessAsDouble(dValue, false, parameterAccessContext);
}

bool CParameterHandle::setAsDoubleArray(const vector<double>& adValues, string& strError)
{
    // Check operation validity
    if (!checkAccessValidity(true, adValues.size(), strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    CAutoLock autoLock(_pParameterMgr->getBlackboardMutex());

    // When in tuning mode, silently skip the request
    if (_pParameterMgr->tuningModeOn()) {

        return true;
    }

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    // Copy values for type adaptation
    vector<double> adUserValues = adValues;

    return _pBaseParameter->accessAsDoubleArray(adUserValues, true, parameterAccessContext);
}

bool CParameterHandle::getAsDoubleArray(vector<double>& adValues, string& strError) const
{
    // Check operation validity
    if (!checkAccessValidity(false, -1, strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    CAutoLock autoLock(_pParameterMgr->getBlackboardMutex());

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    return _pBaseParameter->accessAsDoubleArray(adValues, false, parameterAccessContext);
}

// String Access
bool CParameterHandle::setAsString(const string& strValue, string& strError)
{
    // Check operation validity
    if (!checkAccessValidity(true, 0, strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    CAutoLock autoLock(_pParameterMgr->getBlackboardMutex());

    // When in tuning mode, silently skip the request
    if (_pParameterMgr->tuningModeOn()) {

        return true;
    }

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    // Copy value for type adaptation
    string strUserValue = strValue;

    return _pBaseParameter->accessAsString(strUserValue, true, parameterAccessContext);
}

bool CParameterHandle::getAsString(string& strValue, string& strError) const
{
    // Check operation validity
    if (!checkAccessValidity(false, 0, strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    CAutoLock autoLock(_pParameterMgr->getBlackboardMutex());

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    return _pBaseParameter->accessAsString(strValue, false, parameterAccessContext);
}

bool CParameterHandle::setAsStringArray(const vector<string>& astrValues, string& strError)
{
    // Check operation validity
    if (!checkAccessValidity(true, astrValues.size(), strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    CAutoLock autoLock(_pParameterMgr->getBlackboardMutex());

    // When in tuning mode, silently skip the request
    if (_pParameterMgr->tuningModeOn()) {

        return true;
    }

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    // Copy values for type adaptation
    vector<string> astrUserValues = astrValues;

    return _pBaseParameter->accessAsStringArray(astrUserValues, true, parameterAccessContext);
}

bool CParameterHandle::getAsStringArray(vector<string>& astrValues, string& strError) const
{
    // Check operation validity
    if (!checkAccessValidity(false, -1, strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    CAutoLock autoLock(_pParameterMgr->getBlackboardMutex());

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    return _pBaseParameter->accessAsStringArray(astrValues, false, parameterAccessContext);
}

// Access validity
bool CParameterHandle::checkAccessValidity(bool bSet, uint32_t uiArrayLength, string& strError) const
{
    if (bSet && !isRogue()) {

        strError = "Parameter is not rogue: ";

        strError += getPath();

        return false;
    }

    if (uiArrayLength && !isArray()) {

        strError = "Parameter is scalar: ";

        strError += getPath();

        return false;
    }

    if (!uiArrayLength && isArray()) {

        strError = "Parameter is an array: ";

        strError += getPath();

        return false;
    }

    if (bSet && uiArrayLength && (uiArrayLength != getArrayLength())) {

        strError = "Array length mismatch: ";

        strError += getPath();

        return false;
    }

    return true;
}
