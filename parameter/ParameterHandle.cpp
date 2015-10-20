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
    : _pBaseParameter(pParameter), _pParameterMgr(pParameterMgr)
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

template <class T>
struct isVector : std::false_type {};
template <class T>
struct isVector<std::vector<T>> : std::true_type {};

template <class T>
size_t CParameterHandle::getSize(T /*value*/) {
    return 0;
}

template <class T>
size_t CParameterHandle::getSize(std::vector<T> &values) {
    return values.size();
}

template <class T>
bool CParameterHandle::setAs(const T value, string &error) const
{
    if (not checkSetValidity(getSize(value), error)) {
        return false;
    }

    // Ensure we're safe against blackboard foreign access
    lock_guard<mutex> autoLock(_pParameterMgr->getBlackboardMutex());

    // When in tuning mode, silently skip "set" requests
    if (_pParameterMgr->tuningModeOn()) {

        return true;
    }

    CParameterAccessContext parameterAccessContext(error, _pParameterMgr->getParameterBlackboard());

    // BaseParameret::access takes a non-const argument - therefore we need to
    // copy the value
    T copy = value;
    return _pBaseParameter->access(copy, true, parameterAccessContext);
}

template <class T>
bool CParameterHandle::getAs(T &value, string &error) const
{
    if (not checkGetValidity(isVector<T>::value, error)) {
        return false;
    }

    // Ensure we're safe against blackboard foreign access
    lock_guard<mutex> autoLock(_pParameterMgr->getBlackboardMutex());

    CParameterAccessContext parameterAccessContext(error, _pParameterMgr->getParameterBlackboard());

    return _pBaseParameter->access(value, false, parameterAccessContext);
}

// Boolean access
bool CParameterHandle::setAsBoolean(bool bValue, string& error)
{
    return setAs(bValue, error);
}

bool CParameterHandle::getAsBoolean(bool& bValue, string& error) const
{
    return getAs(bValue, error);
}

bool CParameterHandle::setAsBooleanArray(const std::vector<bool>& abValues, string& error)
{
    return setAs(abValues, error);
}

bool CParameterHandle::getAsBooleanArray(std::vector<bool>& abValues, string& error) const
{
    return getAs(abValues, error);
}

// Integer Access
bool CParameterHandle::setAsInteger(uint32_t uiValue, string& error)
{
    return setAs(uiValue, error);
}

bool CParameterHandle::getAsInteger(uint32_t& uiValue, string& error) const
{
    return getAs(uiValue, error);
}

bool CParameterHandle::setAsIntegerArray(const std::vector<uint32_t>& auiValues, string& error)
{
    return setAs(auiValues, error);
}

bool CParameterHandle::getAsIntegerArray(std::vector<uint32_t>& auiValues, string& error) const
{
    return getAs(auiValues, error);
}

// Signed Integer Access
bool CParameterHandle::setAsSignedInteger(int32_t iValue, string& error)
{
    return setAs(iValue, error);
}

bool CParameterHandle::getAsSignedInteger(int32_t& iValue, string& error) const
{
    return getAs(iValue, error);
}

bool CParameterHandle::setAsSignedIntegerArray(const std::vector<int32_t>& aiValues, string& error)
{
    return setAs(aiValues, error);
}

bool CParameterHandle::getAsSignedIntegerArray(std::vector<int32_t>& aiValues, string& error) const
{
    return getAs(aiValues, error);
}

// Double Access
bool CParameterHandle::setAsDouble(double dValue, string& error)
{
    return setAs(dValue, error);
}

bool CParameterHandle::getAsDouble(double& dValue, string& error) const
{
    return getAs(dValue, error);
}

bool CParameterHandle::setAsDoubleArray(const std::vector<double>& adValues, string& error)
{
    return setAs(adValues, error);
}

bool CParameterHandle::getAsDoubleArray(std::vector<double>& adValues, string& error) const
{
    return getAs(adValues, error);
}

// String Access
bool CParameterHandle::setAsString(const string& strValue, string& error)
{
    return setAs(strValue, error);
}

bool CParameterHandle::getAsString(string& strValue, string& error) const
{
    return getAs(strValue, error);
}

bool CParameterHandle::setAsStringArray(const std::vector<string>& astrValues, string& error)
{
    return setAs(astrValues, error);
}

bool CParameterHandle::getAsStringArray(std::vector<string>& astrValues, string& error) const
{
    return getAs(astrValues, error);
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
