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
#include "ComponentType.h"
#include "ComponentLibrary.h"
#include "TypeElement.h"
#include "XmlParameterSerializingContext.h"
#include <assert.h>

#define base CTypeElement

CComponentType::CComponentType(const string& strName) : base(strName), _pExtendsComponentType(NULL)
{
}

string CComponentType::getKind() const
{
    return "ComponentType";
}

bool CComponentType::childrenAreDynamic() const
{
    return true;
}

bool CComponentType::getMappingData(const string& strKey, const string*& pStrValue) const
{
    // Try myself first then extended component type
    return base::getMappingData(strKey, pStrValue) || (_pExtendsComponentType && _pExtendsComponentType->getMappingData(strKey, pStrValue));
}

bool CComponentType::hasMappingData() const
{
    // Try myself first then extended component type
    return base::hasMappingData() || (_pExtendsComponentType && _pExtendsComponentType->hasMappingData());
}

bool CComponentType::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    // Context
    CXmlParameterSerializingContext& parameterBuildContext = static_cast<CXmlParameterSerializingContext&>(serializingContext);

    const CComponentLibrary* pComponentLibrary = parameterBuildContext.getComponentLibrary();

    // Populate children
    if (!base::fromXml(xmlElement, serializingContext)) {

        return false;
    }

    // Check for Extends attribute (extensions will be populated after and not before)
    if (xmlElement.hasAttribute("Extends")) {

        string strExtendsType = xmlElement.getAttributeString("Extends");

        _pExtendsComponentType = pComponentLibrary->getComponentType(strExtendsType);

        if (!_pExtendsComponentType) {

            serializingContext.setError("ComponentType " + strExtendsType + " referred to by " + xmlElement.getPath() + " not found!");

            return false;
        }

        if (_pExtendsComponentType == this) {

            serializingContext.setError("Recursive ComponentType definition of " + xmlElement.getPath());

            return false;
        }
    }

    return true;
}

void CComponentType::populate(CElement* pElement) const
{
    // Populate children
    base::populate(pElement);

    // Manage extended type
    if (_pExtendsComponentType) {

        // Populate from extended type
        _pExtendsComponentType->populate(pElement);
    }
}

CInstanceConfigurableElement* CComponentType::doInstantiate() const
{
    // Not supposed to be called directly (instantiation made through CComponentInstance object)
    assert(0);

    return NULL;
}
