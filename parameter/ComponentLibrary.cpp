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
#include "ComponentLibrary.h"
#include "ComponentType.h"
#include <assert.h>

CComponentLibrary::CComponentLibrary()
{
}

bool CComponentLibrary::childrenAreDynamic() const
{
    return true;
}

string CComponentLibrary::getKind() const
{
    return "ComponentLibrary";
}

const CComponentType* CComponentLibrary::getComponentType(const string& strName) const
{
    return static_cast<const CComponentType*>(findChild(strName));
}

bool CComponentLibrary::fromXml(const CXmlElement& xmlElement,
                                CXmlSerializingContext& serializingContext)
{
    CXmlElement childElement;

    CXmlElement::CChildIterator it(xmlElement);

    // XML populate all component libraries
    while (it.next(childElement)) {

        // Filter component library/type set elements
        if (childElement.getType() == "ComponentLibrary" ||
            childElement.getType() == "ComponentTypeSet") {

            if (!fromXml(childElement, serializingContext)) {

                return false;
            }
        } else {
            // Regular child creation and populating
            CElement* pChild = createChild(childElement, serializingContext);

            if (!pChild || !pChild->fromXml(childElement, serializingContext)) {

                return false;
            }
        }
    }

    return true;
}

