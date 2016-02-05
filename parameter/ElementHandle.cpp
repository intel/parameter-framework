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
#include "ElementHandle.h"
#include "ParameterAccessContext.h"
#include "BaseParameter.h"
#include "XmlParameterSerializingContext.h"
#include "Subsystem.h"
#include <assert.h>
#include "ParameterMgr.h"

#include <mutex>

using std::string;
using std::mutex;
using std::lock_guard;

/** @return 0 by default, ie for non overloaded types. */
template <class T>
static size_t getUserInputSize(const T & /*scalar*/)
{
    return 0;
}

/** @return the vector's size. */
template <class T>
static size_t getUserInputSize(const std::vector<T> &vector)
{
    return vector.size();
}

ElementHandle::ElementHandle(CConfigurableElement &element, CParameterMgr &parameterMgr)
    : mElement(element), mParameterMgr(parameterMgr)
{
}

string ElementHandle::getName() const
{
    return mElement.getName();
}

size_t ElementHandle::getSize() const
{
    return mElement.getFootPrint();
}

bool ElementHandle::isParameter() const
{
    return mElement.isParameter();
}

string ElementHandle::getDescription() const
{
    return mElement.getDescription();
}

// Parameter features
bool ElementHandle::isRogue() const
{
    return mElement.isRogue();
}

bool ElementHandle::isArray() const
{
    return getArrayLength() != 0;
}

size_t ElementHandle::getArrayLength() const
{
    // Only instances can be arrays, SystemClass can not, nor subsystems
    auto *instance = dynamic_cast<CInstanceConfigurableElement *>(&mElement);
    if (instance == nullptr) {
        return 0;
    }
    return instance->getArrayLength();
}

string ElementHandle::getPath() const
{
    return mElement.getPath();
}

string ElementHandle::getKind() const
{
    return mElement.getKind();
}

std::vector<ElementHandle> ElementHandle::getChildren()
{
    size_t nbChildren = mElement.getNbChildren();

    std::vector<ElementHandle> children;
    children.reserve(nbChildren);

    for (size_t childIndex = 0; childIndex < nbChildren; ++childIndex) {
        auto *child = static_cast<CConfigurableElement *>(mElement.getChild(childIndex));
        // Can not use emplace back as the constructor is private
        children.push_back({*child, mParameterMgr});
    }
    return children;
}

bool ElementHandle::getMappingData(const string &strKey, string &strValue) const
{
    const std::string *pStrValue;

    // Seach for the key in self and ancestors
    auto elements = mElement.getConfigurableElementContext();

    for (auto *element : elements)
        if (element->getMappingData(strKey, pStrValue)) {
            strValue = *pStrValue;
            return true;
        }

    return false;
}

bool ElementHandle::getStructureAsXML(std::string &xmlSettings, std::string &error) const
{
    // Use default access context for structure export
    CParameterAccessContext accessContext(error);
    return mParameterMgr.exportElementToXMLString(
        &mElement, mElement.getXmlElementName(),
        CXmlParameterSerializingContext{accessContext, error}, xmlSettings);
}

template <class T>
struct isVector : std::false_type
{
};
template <class T>
struct isVector<std::vector<T>> : std::true_type
{
};

bool ElementHandle::getAsXML(std::string &xmlValue, std::string &error) const
{
    std::string result;
    if (not mParameterMgr.getSettingsAsXML(&mElement, result)) {
        error = result;
        return false;
    }

    xmlValue = result;
    return true;
}

bool ElementHandle::setAsXML(const std::string &xmlValue, std::string &error)
{
    return mParameterMgr.setSettingsAsXML(&mElement, xmlValue, error);
}

bool ElementHandle::getAsBytes(std::vector<uint8_t> &bytesValue, std::string & /*error*/) const
{
    mParameterMgr.getSettingsAsBytes(mElement, bytesValue);

    // Currently this operation can not fail.
    // Nevertheless this is more a design than intrinsic property.
    // Use the same error reporting pattern to avoid breaking the api in future
    // release if an error need to be reported (and be consistent with all other getAs*).
    return true;
}

bool ElementHandle::setAsBytes(const std::vector<uint8_t> &bytesValue, std::string &error)
{
    return mParameterMgr.setSettingsAsBytes(mElement, bytesValue, error);
}

template <class T>
bool ElementHandle::setAs(const T value, string &error) const
{
    if (not checkSetValidity(getUserInputSize(value), error)) {
        return false;
    }
    // Safe downcast thanks to isParameter check in checkSetValidity
    auto &parameter = static_cast<CBaseParameter &>(mElement);

    // When in tuning mode, silently skip "set" requests
    if (mParameterMgr.tuningModeOn()) {

        return true;
    }

    CParameterAccessContext parameterAccessContext(error, mParameterMgr.getParameterBlackboard());

    // BaseParamere::access takes a non-const argument - therefore we need to
    // copy the value
    T copy = value;

    // Ensure we're safe against blackboard foreign access
    lock_guard<mutex> autoLock(mParameterMgr.getBlackboardMutex());

    return parameter.access(copy, true, parameterAccessContext);
}

template <class T>
bool ElementHandle::getAs(T &value, string &error) const
{
    if (not checkGetValidity(isVector<T>::value, error)) {
        return false;
    }
    // Safe downcast thanks to isParameter check in checkGetValidity
    auto &parameter = static_cast<const CBaseParameter &>(mElement);

    // Ensure we're safe against blackboard foreign access
    lock_guard<mutex> autoLock(mParameterMgr.getBlackboardMutex());

    CParameterAccessContext parameterAccessContext(error, mParameterMgr.getParameterBlackboard());

    return parameter.access(value, false, parameterAccessContext);
}

// Boolean access
bool ElementHandle::setAsBoolean(bool value, string &error)
{
    return setAs(value, error);
}

bool ElementHandle::getAsBoolean(bool &value, string &error) const
{
    return getAs(value, error);
}

bool ElementHandle::setAsBooleanArray(const std::vector<bool> &value, string &error)
{
    return setAs(value, error);
}

bool ElementHandle::getAsBooleanArray(std::vector<bool> &value, string &error) const
{
    return getAs(value, error);
}

// Integer Access
bool ElementHandle::setAsInteger(uint32_t value, string &error)
{
    return setAs(value, error);
}

bool ElementHandle::getAsInteger(uint32_t &value, string &error) const
{
    return getAs(value, error);
}

bool ElementHandle::setAsIntegerArray(const std::vector<uint32_t> &value, string &error)
{
    return setAs(value, error);
}

bool ElementHandle::getAsIntegerArray(std::vector<uint32_t> &value, string &error) const
{
    return getAs(value, error);
}

// Signed Integer Access
bool ElementHandle::setAsSignedInteger(int32_t value, string &error)
{
    return setAs(value, error);
}

bool ElementHandle::getAsSignedInteger(int32_t &value, string &error) const
{
    return getAs(value, error);
}

bool ElementHandle::setAsSignedIntegerArray(const std::vector<int32_t> &value, string &error)
{
    return setAs(value, error);
}

bool ElementHandle::getAsSignedIntegerArray(std::vector<int32_t> &value, string &error) const
{
    return getAs(value, error);
}

// Double Access
bool ElementHandle::setAsDouble(double value, string &error)
{
    return setAs(value, error);
}

bool ElementHandle::getAsDouble(double &value, string &error) const
{
    return getAs(value, error);
}

bool ElementHandle::setAsDoubleArray(const std::vector<double> &value, string &error)
{
    return setAs(value, error);
}

bool ElementHandle::getAsDoubleArray(std::vector<double> &value, string &error) const
{
    return getAs(value, error);
}

// String Access
bool ElementHandle::setAsString(const string &value, string &error)
{
    return setAs(value, error);
}

bool ElementHandle::getAsString(string &value, string &error) const
{
    return getAs(value, error);
}

bool ElementHandle::setAsStringArray(const std::vector<string> &value, string &error)
{
    return setAs(value, error);
}

bool ElementHandle::getAsStringArray(std::vector<string> &value, string &error) const
{
    return getAs(value, error);
}

bool ElementHandle::checkGetValidity(bool asArray, string &error) const
{
    if (not isParameter()) {
        error = "Can not set element " + getPath() + " as it is not a parameter.";
        return false;
    }

    if (asArray != isArray()) {

        auto toStr = [](bool array) { return array ? "an array" : "a scalar"; };
        error = "Can not get \"" + getPath() + "\" as " + toStr(asArray) + " because it is " +
                toStr(isArray());
        return false;
    }

    return true;
}

// Access validity
bool ElementHandle::checkSetValidity(size_t arrayLength, string &error) const
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
        error = "Array length mismatch for \"" + getPath() + "\", expected: " +
                to_string(getArrayLength()) + ", got: " + to_string(arrayLength);
        return false;
    }

    return true;
}
