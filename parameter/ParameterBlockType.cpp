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
#include "ParameterBlockType.h"
#include "ParameterBlock.h"
#include <sstream>

#define base CTypeElement

CParameterBlockType::CParameterBlockType(const string& strName) : base(strName)
{
}

string CParameterBlockType::getKind() const
{
    return "ParameterBlock";
}

bool CParameterBlockType::childrenAreDynamic() const
{
    return true;
}

CInstanceConfigurableElement* CParameterBlockType::doInstantiate() const
{
    return new CParameterBlock(getName(), this);
}

void CParameterBlockType::populate(CElement* pElement) const
{
    uint32_t uiArrayLength = getArrayLength();

    if (uiArrayLength) {

        // Create child elements
        uint32_t uiChild;

        for (uiChild = 0; uiChild < uiArrayLength; uiChild++) {

            CParameterBlock* pChildParameterBlock = new CParameterBlock(computeChildName(uiChild), this);

            pElement->addChild(pChildParameterBlock);

            base::populate(pChildParameterBlock);
        }
    } else {
        // Regular block
        base::populate(pElement);
    }
}

string CParameterBlockType::computeChildName(uint32_t uiChild)
{
    ostringstream strStream;

    strStream << uiChild;

    return strStream.str();
}
