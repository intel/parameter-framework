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
#include "TypeElement.h"
#include "MappingData.h"
#include "Tokenizer.h"
#include "InstanceConfigurableElement.h"
#include <assert.h>

#define base CElement

CTypeElement::CTypeElement(const string& strName) : base(strName), _uiArrayLength(0), _pMappingData(NULL)
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

bool CTypeElement::getMappingData(const string& strKey, const string*& pStrValue) const
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
void CTypeElement::showProperties(string& strResult) const
{
    (void)strResult;
    // Prevent base from being called in that context!
}

void CTypeElement::populate(CElement* pElement) const
{
    // Populate children
    uint32_t uiChild;
    uint32_t uiNbChildren = getNbChildren();

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

// From IXmlSource
void CTypeElement::toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const
{
    if (!isScalar()) {

        xmlElement.setAttributeInteger("ArrayLength", getArrayLength());
    }

    base::toXml(xmlElement, serializingContext);
}
