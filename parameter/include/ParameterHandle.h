/*  ParameterHandle.h
 **
 ** Copyright © 2011 Intel
 **
 ** Licensed under the Apache License, Version 2.0 (the "License");
 ** you may not use this file except in compliance with the License.
 ** You may obtain a copy of the License at
 **
 **     http://www.apache.org/licenses/LICENSE-2.0
 **
 ** Unless required by applicable law or agreed to in writing, software
 ** distributed under the License is distributed on an "AS IS" BASIS,
 ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 ** See the License for the specific language governing permissions and
 ** limitations under the License.
 **
 ** AUTHOR: Patrick Benavoli (patrickx.benavoli@intel.com)
 ** CREATED: 2011-06-01
 ** UPDATED: 2011-07-27
 **
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
    bool checkAccessValidity(bool bSet, uint32_t uiArrayLength, std::string& strError) const;

    // Accessed parameter instance
    const CBaseParameter* _pBaseParameter;
    // Parameter Mgr
    CParameterMgr* _pParameterMgr;
    // Subsystem endianness
    bool _bBigEndianSubsystem;
};
