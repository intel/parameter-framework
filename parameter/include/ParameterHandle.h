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
#pragma once

#include <stdint.h>
#include <string>
#include <vector>

class CBaseParameter;
class CParameterMgr;

class CParameterHandle
{
public:
    CParameterHandle(const CBaseParameter* pParameter, CParameterMgr* pParameterMgr);

    // Parameter features
    bool isRogue() const;
    bool isArray() const;
    // Array Length
    uint32_t getArrayLength() const; // Returns 0 for scalar
    // Parameter path
    std::string getPath() const;
    // Parameter kind
    std::string getKind() const;

    // Boolean access
    bool setAsBoolean(bool bValue, std::string& strError);

    /**
     * Fetch the parameter value as a boolean.
     *
     * @param bValue Reference to a boolean variable where the value will be stored
     * @param strError Error message if a problem occured
     *
     * @return true on success, false otherwise
     */
    bool getAsBoolean(bool& bValue, std::string& strError) const;
    bool setAsBooleanArray(const std::vector<bool>& abValues, std::string& strError);
    bool getAsBooleanArray(std::vector<bool>& abValues, std::string& strError) const;

    // Integer Access
    bool setAsInteger(uint32_t uiValue, std::string& strError);
    bool getAsInteger(uint32_t& uiValue, std::string& strError) const;
    bool setAsIntegerArray(const std::vector<uint32_t>& auiValues, std::string& strError);
    bool getAsIntegerArray(std::vector<uint32_t>& auiValues, std::string& strError) const;

    // Signed Integer Access
    bool setAsSignedInteger(int32_t iValue, std::string& strError);
    bool getAsSignedInteger(int32_t& iValue, std::string& strError) const;
    bool setAsSignedIntegerArray(const std::vector<int32_t>& aiValues, std::string& strError);
    bool getAsSignedIntegerArray(std::vector<int32_t>& aiValues, std::string& strError) const;

    // Double Access
    bool setAsDouble(double dValue, std::string& strError);
    bool getAsDouble(double& dValue, std::string& strError) const;
    bool setAsDoubleArray(const std::vector<double>& adValues, std::string& strError);
    bool getAsDoubleArray(std::vector<double>& adValues, std::string& strError) const;

    // String Access
    bool setAsString(const std::string& strValue, std::string& strError);
    bool getAsString(std::string& strValue, std::string& strError) const;
    bool setAsStringArray(const std::vector<std::string>& astrValues, std::string& strError);
    bool getAsStringArray(std::vector<std::string>& astrValues, std::string& strError) const;

private:
    // Access validity
    bool checkAccessValidity(bool bSet, size_t uiArrayLength, std::string& strError) const;

    // Accessed parameter instance
    const CBaseParameter* _pBaseParameter;
    // Parameter Mgr
    CParameterMgr* _pParameterMgr;
    // Subsystem endianness
    bool _bBigEndianSubsystem;
};
