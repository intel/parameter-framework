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
#include "ComponentInstance.h"
#include "ComponentLibrary.h"
#include "ComponentType.h"
#include "Component.h"
#include "ParameterBlock.h" // for "array" instantiation
#include "XmlParameterSerializingContext.h"

#define base CTypeElement

CComponentInstance::CComponentInstance(const std::string &strName) : base(strName)
{
}

std::string CComponentInstance::getKind() const
{
    return "ComponentInstance";
}

std::string CComponentInstance::getXmlElementName() const
{
    // Once instantiated components are reflected as parameter blocks
    // in XML documents
    return "ParameterBlock";
}

bool CComponentInstance::childrenAreDynamic() const
{
    return true;
}

bool CComponentInstance::getMappingData(const std::string &strKey,
                                        const std::string *&pStrValue) const
{
    // Try myself first then associated component type
    return base::getMappingData(strKey, pStrValue) ||
           (_pComponentType && _pComponentType->getMappingData(strKey, pStrValue));
}

bool CComponentInstance::hasMappingData() const
{
    // Try myself first then extended component type
    return base::hasMappingData() || (_pComponentType && _pComponentType->hasMappingData());
}

std::string CComponentInstance::getFormattedMapping() const
{
    return base::getFormattedMapping(_pComponentType);
}

bool CComponentInstance::fromXml(const CXmlElement &xmlElement,
                                 CXmlSerializingContext &serializingContext)
{
    // Context
    CXmlParameterSerializingContext &parameterBuildContext =
        static_cast<CXmlParameterSerializingContext &>(serializingContext);

    const CComponentLibrary *pComponentLibrary = parameterBuildContext.getComponentLibrary();

    std::string strComponentType;
    xmlElement.getAttribute("Type", strComponentType);

    _pComponentType = pComponentLibrary->getComponentType(strComponentType);

    if (!_pComponentType) {

        serializingContext.setError("Unable to create Component " + xmlElement.getPath() +
                                    ". ComponentType " + strComponentType + " not found!");

        return false;
    }
    if (_pComponentType == getParent()) {

        serializingContext.setError("Recursive definition of " + _pComponentType->getName() +
                                    " due to " + xmlElement.getPath() +
                                    " referring to one of its own type.");

        return false;
    }

    return base::fromXml(xmlElement, serializingContext);
}

CInstanceConfigurableElement *CComponentInstance::doInstantiate() const
{
    if (isScalar()) {
        return new CComponent(getName(), this);
    } else {
        return new CParameterBlock(getName(), this);
    }
}

void CComponentInstance::populate(CElement *pElement) const
{
    size_t arrayLength = getArrayLength();

    if (arrayLength != 0) {

        // Create child elements
        for (size_t child = 0; child < arrayLength; child++) {

            CComponent *pChildComponent = new CComponent(std::to_string(child), this);

            pElement->addChild(pChildComponent);

            base::populate(pChildComponent);

            _pComponentType->populate(pChildComponent);
        }
    } else {
        base::populate(pElement);

        _pComponentType->populate(static_cast<CComponent *>(pElement));
    }
}
