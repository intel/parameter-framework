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
#include "LinearParameterAdaptation.h"

#define base CParameterAdaptation

CLinearParameterAdaptation::CLinearParameterAdaptation() : base("Linear"), _dSlopeNumerator(1), _dSlopeDenominator(1)
{
}

// Element properties
void CLinearParameterAdaptation::showProperties(string& strResult) const
{
    base::showProperties(strResult);

    // SlopeNumerator
    strResult += " - SlopeNumerator: ";
    strResult += toString(_dSlopeNumerator);
    strResult += "\n";

    // SlopeDenominator
    strResult += " - SlopeDenominator: ";
    strResult += toString(_dSlopeDenominator);
    strResult += "\n";
}

// From IXmlSink
bool CLinearParameterAdaptation::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    // Get SlopeNumerator
    if (xmlElement.hasAttribute("SlopeNumerator")) {

        _dSlopeNumerator = xmlElement.getAttributeDouble("SlopeNumerator");

    } else {
        // Default
        _dSlopeNumerator = 1;
    }
    // Get SlopeDenominator
    if (xmlElement.hasAttribute("SlopeDenominator")) {

        _dSlopeDenominator = xmlElement.getAttributeDouble("SlopeDenominator");

        // Avoid by 0 division errors
        if (_dSlopeDenominator == 0) {

            serializingContext.setError("SlopeDenominator attribute can't be 0 on element" + xmlElement.getPath());

            return false;
        }

    } else {
        // Default
        _dSlopeDenominator = 1;
    }

    // Base
    return base::fromXml(xmlElement, serializingContext);
}

// Conversions
int64_t CLinearParameterAdaptation::fromUserValue(double dValue) const
{
    return base::fromUserValue(dValue * _dSlopeNumerator / _dSlopeDenominator);
}

double CLinearParameterAdaptation::toUserValue(int64_t iValue) const
{
    return base::toUserValue(iValue) * _dSlopeDenominator / _dSlopeNumerator;
}
