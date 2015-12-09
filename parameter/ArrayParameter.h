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

#include "Parameter.h"

class CArrayParameter : public CParameter
{
public:
    CArrayParameter(const std::string &strName, const CTypeElement *pTypeElement);

    // Instantiation, allocation
    virtual size_t getFootPrint() const;

    /// Value access
    using CBaseParameter::access;
    bool access(std::vector<bool> &abValues, bool bSet,
                CParameterAccessContext &parameterAccessContext) const override final;
    bool access(std::vector<uint32_t> &auiValues, bool bSet,
                CParameterAccessContext &parameterAccessContext) const override final;
    bool access(std::vector<int32_t> &aiValues, bool bSet,
                CParameterAccessContext &parameterAccessContext) const override final;
    bool access(std::vector<double> &adValues, bool bSet,
                CParameterAccessContext &parameterAccessContext) const override final;
    bool access(std::vector<std::string> &astrValues, bool bSet,
                CParameterAccessContext &parameterAccessContext) const override final;

protected:
    // User set/get
    virtual bool accessValue(CPathNavigator &pathNavigator, std::string &strValue, bool bSet,
                             CParameterAccessContext &parameterAccessContext) const;
    // Used for simulation and virtual subsystems
    virtual void setDefaultValues(CParameterAccessContext &parameterAccessContext) const;

    // Element properties
    virtual void showProperties(std::string &strResult) const;

private:
    // Array length
    size_t getArrayLength() const;
    // Common set value processing
    bool setValues(size_t uiStartIndex, size_t offset, const std::string &strValue,
                   CParameterAccessContext &parameterAccessContext) const;
    // Log / get values common
    std::string getValues(size_t baseOffset, CParameterAccessContext &parameterAccessContext) const;
    std::string logValue(CParameterAccessContext &context) const override;
    // Index retrieval from user set/get request
    bool getIndex(CPathNavigator &pathNavigator, size_t &index,
                  CParameterAccessContext &parameterAccessContext) const;

    /** Access whole array.
     *
     * @param[in] offset Offset of the array in the context blackboard.
     * @{
     */
    bool doSetValue(const std::string &strValue, size_t offset,
                    CParameterAccessContext &parameterAccessContext) const override;
    void doGetValue(std::string &strValue, size_t offset,
                    CParameterAccessContext &parameterAccessContext) const override;
    /** @} */

    /// Value access
    // Generic Access
    template <typename type>
    bool accessValues(std::vector<type> &values, bool bSet,
                      CParameterAccessContext &parameterAccessContext) const;
    template <typename type>
    bool setValues(const std::vector<type> &values,
                   CParameterAccessContext &parameterAccessContext) const;
    template <typename type>
    bool getValues(std::vector<type> &values,
                   CParameterAccessContext &parameterAccessContext) const;
    template <typename type>
    bool doSet(type value, size_t offset, CParameterAccessContext &parameterAccessContext) const;
    template <typename type>
    bool doGet(type &value, size_t offset, CParameterAccessContext &parameterAccessContext) const;
};
