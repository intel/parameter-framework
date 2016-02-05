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

#include "ParameterFramework.hpp"
#include "FailureWrapper.hpp"

#include <ElementHandle.h>

namespace parameterFramework
{
/** Wrapper around ::ElementHandle to throw exceptions on errors and have more
 * user friendly methods.
 * Contrary to ::ElementHandle, is constructed through it's constructor
 * and not a factory method.
 * @see parameterFramework::ParameterFramework for the main PF interface.
 */
class ElementHandle : private FailureWrapper<::ElementHandle>
{
    ElementHandle(const ElementHandle &other) = delete;
    ElementHandle &operator=(const ElementHandle &other) = delete;

private:
    using EH = ::ElementHandle;

public:
    ElementHandle(ParameterFramework &pf, const std::string &path)
        : FailureWrapper(pf.createElementHandle(path))
    {
    }

    /** Wrap EH::getSize.
     *
     * @note: can not use `using EH::getSize` as getSize has private overloads in EH.
     */
    size_t getSize() const { return EH::getSize(); }

    std::string getMappingData(const std::string &key)
    {
        std::string value;
        if (not EH::getMappingData(key, value)) {
            throw Exception("Could not find mapping key \"" + key + "\" in " + EH::getPath());
        }
        return value;
    }

    /** Wrap EH::setAsDouble to throw an exception on failure. */
    void setAsDouble(double value) { mayFailCall(&EH::setAsDouble, value); }
    /** Wrap EH::getAsDouble to throw an exception on failure. */
    void getAsDouble(double &value) const { mayFailCall(&EH::getAsDouble, value); }

    void setAsInteger(uint32_t value) { mayFailCall(&EH::setAsInteger, value); }
    void getAsInteger(uint32_t &value) const { mayFailCall(&EH::getAsInteger, value); }
    void setAsIntegerArray(const std::vector<uint32_t> &value)
    {
        mayFailCall(&EH::setAsIntegerArray, value);
    }
    void getAsIntegerArray(std::vector<uint32_t> &value) const
    {
        mayFailCall(&EH::getAsIntegerArray, value);
    }

    void setAsSignedInteger(int32_t value) { mayFailCall(&EH::setAsSignedInteger, value); }
    void getAsSignedInteger(int32_t &value) const { mayFailCall(&EH::getAsSignedInteger, value); }
    void setAsSignedIntegerArray(const std::vector<int32_t> &value)
    {
        mayFailCall(&EH::setAsSignedIntegerArray, value);
    }
    void getAsSignedIntegerArray(std::vector<int32_t> &value) const
    {
        mayFailCall(&EH::getAsSignedIntegerArray, value);
    }

    std::string getStructureAsXML() const { return mayFailGet(&EH::getStructureAsXML); }

    std::string getAsXML() const { return mayFailGet(&EH::getAsXML); }
    void setAsXML(const std::string &settings) { mayFailSet(&EH::setAsXML, settings); }

    std::vector<uint8_t> getAsBytes() const
    {
        std::vector<uint8_t> settings(getSize());
        mayFailCall(&EH::getAsBytes, settings);
        return settings;
    }
    void setAsBytes(const std::vector<uint8_t> &settings) { mayFailSet(&EH::setAsBytes, settings); }
};

} // parameterFramework
