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
#include "EnumValuePair.h"

#define base CElement

CEnumValuePair::CEnumValuePair() : _iNumerical(0)
{
}

// CElement
string CEnumValuePair::getKind() const
{
    return "ValuePair";
}

// Numerical
int32_t CEnumValuePair::getNumerical() const
{
    return _iNumerical;
}

string CEnumValuePair::getNumericalAsString() const
{
    return toString(_iNumerical);
}

// From IXmlSink
bool CEnumValuePair::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    // Literal
    setName(xmlElement.getAttributeString("Literal"));

    // Numerical
    _iNumerical = xmlElement.getAttributeSignedInteger("Numerical");

    // Base
    return base::fromXml(xmlElement, serializingContext);
}

// Content dumping
void CEnumValuePair::logValue(string& strValue, CErrorContext& errorContext) const
{
    (void)errorContext;
    // Convert value
    strValue = getNumericalAsString();
}

// From IXmlSource
void CEnumValuePair::toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const
{
    // Literal
    xmlElement.setAttributeString("Literal", this->getName());

    // Numerical
    xmlElement.setAttributeString("Numerical", getNumericalAsString());

    base::toXml(xmlElement, serializingContext);
}
