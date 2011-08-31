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
#include "ComputedSizeParameterType.h"
#include "ComputedSizeParameter.h"
#include <sstream>
#include <stdlib.h>

#define base CParameterType

CComputedSizeParameterType::CComputedSizeParameterType(const string& strName) : base(strName)
{
}

string CComputedSizeParameterType::getKind() const
{
    return "ComputedSizeParameter";
}

bool CComputedSizeParameterType::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    // Size
    setSize(xmlElement.getAttributeInteger("Size") / 8);

    _strReferredElementName = xmlElement.getAttributeString("Parameter");

    return base::fromXml(xmlElement, serializingContext);
}

bool CComputedSizeParameterType::asInteger(const string& strValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const
{
    (void)parameterAccessContext;

    uiValue = strtoul(strValue.c_str(), NULL, 0);

    return true;
}

void CComputedSizeParameterType::asString(const uint32_t& uiValue, string& strValue, CParameterAccessContext& parameterAccessContext) const
{
    (void)parameterAccessContext;

    // Format
    ostringstream strStream;

    strStream << uiValue;

    strValue = strStream.str();
}

const string& CComputedSizeParameterType::getReferredElementName() const
{
    return _strReferredElementName;
}

CInstanceConfigurableElement* CComputedSizeParameterType::doInstantiate() const
{
    // Scalar parameter
    return new CComputedSizeParameter(getName(), this);
}

