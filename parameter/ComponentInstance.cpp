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
#include "ComponentInstance.h"
#include "ComponentLibrary.h"
#include "ComponentType.h"
#include "Component.h"
#include "XmlParameterSerializingContext.h"

#define base CTypeElement

CComponentInstance::CComponentInstance(const string& strName) : base(strName), _pComponentType(NULL)
{
}

string CComponentInstance::getKind() const
{
    return "Component";
}

bool CComponentInstance::childrenAreDynamic() const
{
    return true;
}

bool CComponentInstance::getMappingData(const string& strKey, string& strValue) const
{
    // Try myself first then associated component type
    return base::getMappingData(strKey, strValue) || (_pComponentType && _pComponentType->getMappingData(strKey, strValue));
}

bool CComponentInstance::hasMappingData() const
{
    // Try myself first then extended component type
    return base::hasMappingData() || (_pComponentType && _pComponentType->hasMappingData());
}

bool CComponentInstance::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    // Context
    CXmlParameterSerializingContext& parameterBuildContext = static_cast<CXmlParameterSerializingContext&>(serializingContext);

    const CComponentLibrary* pComponentLibrary = parameterBuildContext.getComponentLibrary();

    string strComponentType = xmlElement.getAttributeString("Type");

    _pComponentType = pComponentLibrary->getComponentType(strComponentType);

    if (!_pComponentType) {

        serializingContext.setError("Unable to create Component " + xmlElement.getPath() + ". ComponentType " + strComponentType + " not found!");

        return false;
    }
    if (_pComponentType == getParent()) {

        serializingContext.setError("Recursive definition of " + _pComponentType->getName() + " due to " + xmlElement.getPath() + " referring to one of its own type.");

        return false;
    }

    return base::fromXml(xmlElement, serializingContext);
}

CInstanceConfigurableElement* CComponentInstance::doInstantiate() const
{
    return new CComponent(getName(), this);
}

void CComponentInstance::populate(CElement* pElement) const
{
    base::populate(pElement);

    _pComponentType->populate(static_cast<CComponent*>(pElement));
}
