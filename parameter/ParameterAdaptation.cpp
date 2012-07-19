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
#include "ParameterAdaptation.h"

#define base CElement

CParameterAdaptation::CParameterAdaptation(const string& strType) : base(strType), _iOffset(0)
{
}
// CElement
string CParameterAdaptation::getKind() const
{
    return "Adaptation";
}

// Attributes
int32_t CParameterAdaptation::getOffset() const
{
    return _iOffset;
}

// Element properties
void CParameterAdaptation::showProperties(string& strResult) const
{
    // Adaptation type
    strResult += " - Type: ";
    strResult += getName();
    strResult += "\n";

    // Offset
    strResult += " - Offset: ";
    strResult += toString(_iOffset);
    strResult += "\n";
}

// From IXmlSink
bool CParameterAdaptation::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    // Get offset
    if (xmlElement.hasAttribute("Offset")) {

        _iOffset = xmlElement.getAttributeSignedInteger("Offset");

    } else {
        // Default
        _iOffset = 0;
    }

    // Base
    return base::fromXml(xmlElement, serializingContext);
}

// Conversions
int64_t CParameterAdaptation::fromUserValue(double dValue) const
{
    return (int64_t)dValue + _iOffset;
}

double CParameterAdaptation::toUserValue(int64_t iValue) const
{
    return (double)(iValue - _iOffset);
}
