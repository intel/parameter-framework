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
#include "CompoundRule.h"

#define base CRule

CCompoundRule::CCompoundRule() : _bTypeAll(false)
{
}

// Class kind
string CCompoundRule::getKind() const
{
    return "CompoundRule";
}

// Returns true if children dynamic creation is to be dealt with
bool CCompoundRule::childrenAreDynamic() const
{
    return true;
}

// Rule check
bool CCompoundRule::matches() const
{
    uint32_t uiChild;
    uint32_t uiNbChildren = getNbChildren();

    for (uiChild = 0; uiChild < uiNbChildren; uiChild++) {

        const CRule* pRule = static_cast<const CRule*>(getChild(uiChild));

        if (pRule->matches() ^ _bTypeAll) {

            return !_bTypeAll;
        }
    }
    return _bTypeAll;
}

// From IXmlSink
bool CCompoundRule::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    // Get type
    _bTypeAll = xmlElement.getAttributeBoolean("Type", "All");

    // Base
    return base::fromXml(xmlElement, serializingContext);
}

// From IXmlSource
void CCompoundRule::toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const
{
    // Set type
    xmlElement.setAttributeString("Type", _bTypeAll ? "All" : "Any");

    // Base
    base::toXml(xmlElement, serializingContext);
}
