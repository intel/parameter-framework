/*
 * Copyright (c) 2011-2015, Intel Corporation
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

#include <stdint.h>
#include <string>
#include <vector>

class CBaseParameter;
class CParameterMgr;

class CParameterHandle
{
public:
    PARAMETER_EXPORT
    CParameterHandle(const CBaseParameter* pParameter, CParameterMgr* pParameterMgr);

    // Parameter features
    PARAMETER_EXPORT
    bool isRogue() const;
    PARAMETER_EXPORT
    bool isArray() const;
    // Array Length
    PARAMETER_EXPORT
    size_t getArrayLength() const; // Returns 0 for scalar
    // Parameter path
    PARAMETER_EXPORT
    std::string getPath() const;
    // Parameter kind
    PARAMETER_EXPORT
    std::string getKind() const;

    // Boolean access
    PARAMETER_EXPORT
    bool setAsBoolean(bool bValue, std::string& strError);

    /**
     * Fetch the parameter value as a boolean.
     *
     * @param bValue Reference to a boolean variable where the value will be stored
     * @param strError Error message if a problem occured
     *
     * @return true on success, false otherwise
     */
    PARAMETER_EXPORT
    bool getAsBoolean(bool& bValue, std::string& strError) const;
    PARAMETER_EXPORT
    bool setAsBooleanArray(const std::vector<bool>& abValues, std::string& strError);
    PARAMETER_EXPORT
    bool getAsBooleanArray(std::vector<bool>& abValues, std::string& strError) const;

    // Integer Access
    PARAMETER_EXPORT
    bool setAsInteger(uint32_t uiValue, std::string& strError);
    PARAMETER_EXPORT
    bool getAsInteger(uint32_t& uiValue, std::string& strError) const;
    PARAMETER_EXPORT
    bool setAsIntegerArray(const std::vector<uint32_t>& auiValues, std::string& strError);
    PARAMETER_EXPORT
    bool getAsIntegerArray(std::vector<uint32_t>& auiValues, std::string& strError) const;

    // Signed Integer Access
    PARAMETER_EXPORT
    bool setAsSignedInteger(int32_t iValue, std::string& strError);
    PARAMETER_EXPORT
    bool getAsSignedInteger(int32_t& iValue, std::string& strError) const;
    PARAMETER_EXPORT
    bool setAsSignedIntegerArray(const std::vector<int32_t>& aiValues, std::string& strError);
    PARAMETER_EXPORT
    bool getAsSignedIntegerArray(std::vector<int32_t>& aiValues, std::string& strError) const;

    // Double Access
    PARAMETER_EXPORT
    bool setAsDouble(double dValue, std::string& strError);
    PARAMETER_EXPORT
    bool getAsDouble(double& dValue, std::string& strError) const;
    PARAMETER_EXPORT
    bool setAsDoubleArray(const std::vector<double>& adValues, std::string& strError);
    PARAMETER_EXPORT
    bool getAsDoubleArray(std::vector<double>& adValues, std::string& strError) const;

    // String Access
    PARAMETER_EXPORT
    bool setAsString(const std::string& strValue, std::string& strError);
    PARAMETER_EXPORT
    bool getAsString(std::string& strValue, std::string& strError) const;
    PARAMETER_EXPORT
    bool setAsStringArray(const std::vector<std::string>& astrValues, std::string& strError);
    PARAMETER_EXPORT
    bool getAsStringArray(std::vector<std::string>& astrValues, std::string& strError) const;

private:
    /** Check that the parameter value can be modify.
     *
     * @param arrayLength[in] If accessing as an array: the new value array length
     *                        Otherwise: 0
     * @param error[out] If access is forbidden: a human readable message explaining why
     *                   Otherwise: not modified.
     *
     * @return true if the parameter value can be retrieved, false otherwise.
     */
    bool checkSetValidity(size_t arrayLength, std::string& error) const;

    /** Check that the parameter value can be retrieved.
     *
     * @param asArray[in] true if accessing as an array, false otherwise.
     * @param error[out] If access is forbidden, a human readable message explaining why
     *                   Otherwise, not modified.
     *
     * @return true if the parameter value can be retrieved, false otherwise.
     */
    bool checkGetValidity(bool asArray, std::string& error) const;

    // Accessed parameter instance
    const CBaseParameter* _pBaseParameter;
    // Parameter Mgr
    CParameterMgr* _pParameterMgr;
    // Subsystem endianness
    bool _bBigEndianSubsystem;
};
