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
#include "ParameterType.h"
#include "Parameter.h"
#include "ArrayParameter.h"

#define base CTypeElement

CParameterType::CParameterType(const string& strName) : base(strName), _uiSize(0)
{
}

CParameterType::~CParameterType()
{
}

// Size
void CParameterType::setSize(uint32_t uiSize)
{
    _uiSize = uiSize;
}

uint32_t CParameterType::getSize() const
{
    return _uiSize;
}

// Unit
string CParameterType::getUnit() const
{
    return _strUnit;
}

// From IXmlSink
bool CParameterType::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    // Unit
    _strUnit = xmlElement.getAttributeString("Unit");

    return base::fromXml(xmlElement, serializingContext);
}

// XML Serialization value space handling
// Value space handling for configuration import
void CParameterType::handleValueSpaceAttribute(CXmlElement& xmlConfigurableElementSettingsElement, CConfigurationAccessContext& configurationAccessContext) const
{
    (void)xmlConfigurableElementSettingsElement;
    (void)configurationAccessContext;
    // Do nothing by default
}

// Default value handling (simulation only)
uint32_t CParameterType::getDefaultValue() const
{
    return 0;
}

// Parameter instantiation
CInstanceConfigurableElement* CParameterType::doInstantiate() const
{
    if (isScalar()) {
        // Scalar parameter
        return new CParameter(getName(), this);
    } else {
        // Array Parameter
        return new CArrayParameter(getName(), this, getArrayLength());
    }
}

