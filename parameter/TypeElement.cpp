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
#include "TypeElement.h"
#include "MappingData.h"
#include "Tokenizer.h"
#include "InstanceConfigurableElement.h"
#include <assert.h>

#define base CElement

CTypeElement::CTypeElement(const std::string& strName) : base(strName), _uiArrayLength(0), _pMappingData(NULL)
{
}

CTypeElement::~CTypeElement()
{
    delete _pMappingData;
}

bool CTypeElement::isScalar() const
{
    return !_uiArrayLength;
}

uint32_t CTypeElement::getArrayLength() const
{
    return _uiArrayLength;
}

int CTypeElement::toPlainInteger(int iSizeOptimizedData) const
{
    return iSizeOptimizedData;
}

bool CTypeElement::getMappingData(const std::string& strKey, const std::string*& pStrValue) const
{
    if (_pMappingData) {

        return _pMappingData->getValue(strKey, pStrValue);
    }
    return false;
}

bool CTypeElement::hasMappingData() const
{
    return !!_pMappingData;
}

// Element properties
void CTypeElement::showProperties(std::string& strResult) const
{
    // The description attribute may be found in the type and not from instance.
    showDescriptionProperty(strResult);
    // Prevent base from being called from the Parameter Type context as
    // it would lead to duplicate the name attribute (duplicated in the type and instance
    // which have a common base Element)
}

void CTypeElement::populate(CElement* pElement) const
{
    // Populate children
    size_t uiChild;
    size_t uiNbChildren = getNbChildren();

    for (uiChild = 0; uiChild < uiNbChildren; uiChild++) {

        const CTypeElement* pChildTypeElement = static_cast<const CTypeElement*>(getChild(uiChild));

        CInstanceConfigurableElement* pInstanceConfigurableChildElement = pChildTypeElement->instantiate();

        // Affiliate
        pElement->addChild(pInstanceConfigurableChildElement);
    }
}

bool CTypeElement::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    // Array Length attribute
    if (xmlElement.hasAttribute("ArrayLength")) {

        _uiArrayLength = xmlElement.getAttributeInteger("ArrayLength");
    } else {
        _uiArrayLength = 0; // Scalar
    }
    // Manage mapping attribute
    if (xmlElement.hasAttribute("Mapping")) {

        if (!getMappingData()->fromXml(xmlElement, serializingContext)) {

            return false;
        }
    }
    return base::fromXml(xmlElement, serializingContext);
}

CInstanceConfigurableElement* CTypeElement::instantiate() const
{
    CInstanceConfigurableElement* pInstanceConfigurableElement = doInstantiate();

    // Populate
    populate(pInstanceConfigurableElement);

    return pInstanceConfigurableElement;
}

CMappingData* CTypeElement::getMappingData()
{
    if (!_pMappingData) {

        _pMappingData = new CMappingData;
    }
    return _pMappingData;
}

std::string CTypeElement::getFormattedMapping() const
{
    if (_pMappingData) {

        return _pMappingData->asString();
    }
    return "";
}

// From IXmlSource
void CTypeElement::toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const
{
    if (!isScalar()) {

        xmlElement.setAttributeInteger("ArrayLength", getArrayLength());
    }

    base::toXml(xmlElement, serializingContext);
}
