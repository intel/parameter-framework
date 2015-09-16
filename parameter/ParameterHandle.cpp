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
#include "ParameterHandle.h"
#include "ParameterAccessContext.h"
#include "BaseParameter.h"
#include "Subsystem.h"
#include <assert.h>
#include "ParameterMgr.h"

#include <mutex>

using std::string;
using std::mutex;
using std::lock_guard;

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
size_t CParameterHandle::getArrayLength() const
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
    if (!checkSetValidity(0, strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    lock_guard<mutex> autoLock(_pParameterMgr->getBlackboardMutex());

    // When in tuning mode, silently skip the request
    if (_pParameterMgr->tuningModeOn()) {

        return true;
    }

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    return _pBaseParameter->accessAsBoolean(bValue, true, parameterAccessContext);
}

bool CParameterHandle::getAsBoolean(bool& bValue, string& strError) const
{
    // Check operation validity
    if (!checkGetValidity(false, strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    lock_guard<mutex> autoLock(_pParameterMgr->getBlackboardMutex());

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    return _pBaseParameter->accessAsBoolean(bValue, false, parameterAccessContext);
}

bool CParameterHandle::setAsBooleanArray(const std::vector<bool>& abValues, string& strError)
{
    // Check operation validity
    if (!checkSetValidity(abValues.size(), strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    lock_guard<mutex> autoLock(_pParameterMgr->getBlackboardMutex());

    // When in tuning mode, silently skip the request
    if (_pParameterMgr->tuningModeOn()) {

        return true;
    }

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    // Copy values for type adaptation
    std::vector<bool> abUserValues = abValues;

    return _pBaseParameter->accessAsBooleanArray(abUserValues, true, parameterAccessContext);
}

bool CParameterHandle::getAsBooleanArray(std::vector<bool>& abValues, string& strError) const
{
    // Check operation validity
    if (!checkGetValidity(true, strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    lock_guard<mutex> autoLock(_pParameterMgr->getBlackboardMutex());

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    return _pBaseParameter->accessAsBooleanArray(abValues, false, parameterAccessContext);
}

// Integer Access
bool CParameterHandle::setAsInteger(uint32_t uiValue, string& strError)
{
    // Check operation validity
    if (!checkSetValidity(0, strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    lock_guard<mutex> autoLock(_pParameterMgr->getBlackboardMutex());

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
    if (!checkGetValidity(false, strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    lock_guard<mutex> autoLock(_pParameterMgr->getBlackboardMutex());

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    return _pBaseParameter->accessAsInteger(uiValue, false, parameterAccessContext);
}

bool CParameterHandle::setAsIntegerArray(const std::vector<uint32_t>& auiValues, string& strError)
{
    // Check operation validity
    if (!checkSetValidity(auiValues.size(), strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    lock_guard<mutex> autoLock(_pParameterMgr->getBlackboardMutex());

    // When in tuning mode, silently skip the request
    if (_pParameterMgr->tuningModeOn()) {

        return true;
    }

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    // Copy values for type adaptation
    std::vector<uint32_t> auiUserValues = auiValues;

    return _pBaseParameter->accessAsIntegerArray(auiUserValues, true, parameterAccessContext);
}

bool CParameterHandle::getAsIntegerArray(std::vector<uint32_t>& auiValues, string& strError) const
{
    // Check operation validity
    if (!checkGetValidity(true, strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    lock_guard<mutex> autoLock(_pParameterMgr->getBlackboardMutex());

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    return _pBaseParameter->accessAsIntegerArray(auiValues, false, parameterAccessContext);
}

// Signed Integer Access
bool CParameterHandle::setAsSignedInteger(int32_t iValue, string& strError)
{
    // Check operation validity
    if (!checkSetValidity(0, strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    lock_guard<mutex> autoLock(_pParameterMgr->getBlackboardMutex());

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
    if (!checkGetValidity(false, strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    lock_guard<mutex> autoLock(_pParameterMgr->getBlackboardMutex());

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    return _pBaseParameter->accessAsSignedInteger(iValue, false, parameterAccessContext);
}

bool CParameterHandle::setAsSignedIntegerArray(const std::vector<int32_t>& aiValues, string& strError)
{
    // Check operation validity
    if (!checkSetValidity(aiValues.size(), strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    lock_guard<mutex> autoLock(_pParameterMgr->getBlackboardMutex());

    // When in tuning mode, silently skip the request
    if (_pParameterMgr->tuningModeOn()) {

        return true;
    }

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    // Copy values for type adaptation
    std::vector<int32_t> aiUserValues = aiValues;

    return _pBaseParameter->accessAsSignedIntegerArray(aiUserValues, true, parameterAccessContext);
}

bool CParameterHandle::getAsSignedIntegerArray(std::vector<int32_t>& aiValues, string& strError) const
{
    // Check operation validity
    if (!checkGetValidity(true, strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    lock_guard<mutex> autoLock(_pParameterMgr->getBlackboardMutex());

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    return _pBaseParameter->accessAsSignedIntegerArray(aiValues, false, parameterAccessContext);
}

// Double Access
bool CParameterHandle::setAsDouble(double dValue, string& strError)
{
    // Check operation validity
    if (!checkSetValidity(0, strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    lock_guard<mutex> autoLock(_pParameterMgr->getBlackboardMutex());

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
    if (!checkGetValidity(false, strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    lock_guard<mutex> autoLock(_pParameterMgr->getBlackboardMutex());

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    return _pBaseParameter->accessAsDouble(dValue, false, parameterAccessContext);
}

bool CParameterHandle::setAsDoubleArray(const std::vector<double>& adValues, string& strError)
{
    // Check operation validity
    if (!checkSetValidity(adValues.size(), strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    lock_guard<mutex> autoLock(_pParameterMgr->getBlackboardMutex());

    // When in tuning mode, silently skip the request
    if (_pParameterMgr->tuningModeOn()) {

        return true;
    }

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    // Copy values for type adaptation
    std::vector<double> adUserValues = adValues;

    return _pBaseParameter->accessAsDoubleArray(adUserValues, true, parameterAccessContext);
}

bool CParameterHandle::getAsDoubleArray(std::vector<double>& adValues, string& strError) const
{
    // Check operation validity
    if (!checkGetValidity(true, strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    lock_guard<mutex> autoLock(_pParameterMgr->getBlackboardMutex());

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    return _pBaseParameter->accessAsDoubleArray(adValues, false, parameterAccessContext);
}

// String Access
bool CParameterHandle::setAsString(const string& strValue, string& strError)
{
    // Check operation validity
    if (!checkSetValidity(0, strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    lock_guard<mutex> autoLock(_pParameterMgr->getBlackboardMutex());

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
    if (!checkGetValidity(false, strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    lock_guard<mutex> autoLock(_pParameterMgr->getBlackboardMutex());

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    return _pBaseParameter->accessAsString(strValue, false, parameterAccessContext);
}

bool CParameterHandle::setAsStringArray(const std::vector<string>& astrValues, string& strError)
{
    // Check operation validity
    if (!checkSetValidity(astrValues.size(), strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    lock_guard<mutex> autoLock(_pParameterMgr->getBlackboardMutex());

    // When in tuning mode, silently skip the request
    if (_pParameterMgr->tuningModeOn()) {

        return true;
    }

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    // Copy values for type adaptation
    std::vector<string> astrUserValues = astrValues;

    return _pBaseParameter->accessAsStringArray(astrUserValues, true, parameterAccessContext);
}

bool CParameterHandle::getAsStringArray(std::vector<string>& astrValues, string& strError) const
{
    // Check operation validity
    if (!checkGetValidity(true, strError)) {

        return false;
    }
    // Ensure we're safe against blackboard foreign access
    lock_guard<mutex> autoLock(_pParameterMgr->getBlackboardMutex());

    // Define access context
    CParameterAccessContext parameterAccessContext(strError, _bBigEndianSubsystem, _pParameterMgr->getParameterBlackboard());

    return _pBaseParameter->accessAsStringArray(astrValues, false, parameterAccessContext);
}

bool CParameterHandle::checkGetValidity(bool asArray, string& error) const
{
    if (asArray != isArray()) {

        auto toStr = [](bool array) { return array ? "an array" : "a scalar"; };
        error = "Can not get \"" + getPath() + "\" as " + toStr(asArray) +
                   " because it is " + toStr(isArray());
        return false;
    }

    return true;
}

// Access validity
bool CParameterHandle::checkSetValidity(size_t arrayLength, string& error) const
{
    // Settings a parameter necessitates the right to get it
    if (not checkGetValidity(arrayLength != 0, error)) {
        return false;
    }

    if (!isRogue()) {

        error = "Can not set parameter \"" + getPath() + "\" as it is not rogue.";
        return false;
    }

    if (arrayLength && (arrayLength != getArrayLength())) {

        using std::to_string;
        error = "Array length mismatch for \"" + getPath() +
                  "\", expected: " + to_string(getArrayLength()) +
                  ", got: " + to_string(arrayLength);
        return false;
    }

    return true;
}
