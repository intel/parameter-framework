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
#pragma once

#include <stdint.h>
#include <limits>

#include "TypeElement.h"

class CParameterAccessContext;
class CConfigurationAccessContext;

class CParameterType : public CTypeElement
{
public:
    CParameterType(const string& strName);
    virtual ~CParameterType();

    // Size
    uint32_t getSize() const;

    // Unit
    string getUnit() const;

    // From IXmlSink
    virtual bool fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext);

    /// Conversions
    // String
    virtual bool toBlackboard(const string& strValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const = 0;
    virtual bool fromBlackboard(string& strValue, const uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const = 0;
    // Boolean
    virtual bool toBlackboard(bool bUserValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const;
    virtual bool fromBlackboard(bool& bUserValue, uint32_t uiValue, CParameterAccessContext& parameterAccessContext) const;
    // Integer
    virtual bool toBlackboard(uint32_t uiUserValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const;
    virtual bool fromBlackboard(uint32_t& uiUserValue, uint32_t uiValue, CParameterAccessContext& parameterAccessContext) const;
    // Signed Integer
    virtual bool toBlackboard(int32_t iUserValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const;
    virtual bool fromBlackboard(int32_t& iUserValue, uint32_t uiValue, CParameterAccessContext& parameterAccessContext) const;
    // Double
    virtual bool toBlackboard(double dUserValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const;
    virtual bool fromBlackboard(double& dUserValue, uint32_t uiValue, CParameterAccessContext& parameterAccessContext) const;

    // XML Serialization value space handling
    // Value space handling for configuration import/export
    virtual void handleValueSpaceAttribute(CXmlElement& xmlConfigurableElementSettingsElement, CConfigurationAccessContext& configurationAccessContext) const;

    // Element properties
    virtual void showProperties(string& strResult) const;

    // Default value handling (simulation only)
    virtual uint32_t getDefaultValue() const;
protected:
    // Object creation
    virtual void populate(CElement* pElement) const;
    // Size
    void setSize(uint32_t uiSize);
    // Sign extension
    void signExtend(int32_t& iData) const;
    void signExtend(int64_t& iData) const;
    // Check data has no bit set outside available range (based on byte size) and
    // check data is consistent with available range, with respect to its sign
    bool isEncodable(uint32_t uiData, bool bIsSigned) const;
    bool isEncodable(uint64_t uiData, bool bIsSigned) const;
    // Remove all bits set outside available range
    uint32_t makeEncodable(uint32_t uiData) const;

    /** Compute max value according to the parameter type */
    template <typename type>
    type getMaxValue() const
    {
        return getSize() < sizeof(type) ?
                    (static_cast<type>(1) << (getSize() * numeric_limits<unsigned char>::digits - 1)) - 1 :
                    numeric_limits<type>::max();
    }

private:
    // Instantiation
    virtual CInstanceConfigurableElement* doInstantiate() const;
    // Generic Access
    template <typename type>
    void doSignExtend(type& data) const;
    template <typename type>
    bool doIsEncodable(type data, bool bIsSigned) const;

    // Size in bytes
    uint32_t _uiSize;
    // Unit
    string _strUnit;
};
