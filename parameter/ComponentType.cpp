/*
 * Copyright (c) 2011-2014, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "ComponentType.h"
#include "ComponentLibrary.h"
#include "TypeElement.h"
#include "XmlParameterSerializingContext.h"
#include <assert.h>

#define base CTypeElement

CComponentType::CComponentType(const std::string& strName) : base(strName), _pExtendsComponentType(NULL)
{
}

std::string CComponentType::getKind() const
{
    return "ComponentType";
}

bool CComponentType::childrenAreDynamic() const
{
    return true;
}

bool CComponentType::getMappingData(const std::string& strKey, const std::string*& pStrValue) const
{
    // Try myself first then extended component type
    return base::getMappingData(strKey, pStrValue) || (_pExtendsComponentType && _pExtendsComponentType->getMappingData(strKey, pStrValue));
}

bool CComponentType::hasMappingData() const
{
    // Try myself first then extended component type
    return base::hasMappingData() || (_pExtendsComponentType && _pExtendsComponentType->hasMappingData());
}

std::string CComponentType::getFormattedMapping() const
{
    // Try myself first then associated component type
    std::string strValue = base::getFormattedMapping();
    if (_pExtendsComponentType) {

        strValue += _pExtendsComponentType->getFormattedMapping();
    }

    return strValue;
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

        std::string strExtendsType = xmlElement.getAttributeString("Extends");

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
