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
#include "XmlElement.h"
#include <libxml/tree.h>
#include <stdlib.h>

CXmlElement::CXmlElement(_xmlNode* pXmlElement) : _pXmlElement(pXmlElement)
{
}

CXmlElement::CXmlElement() : _pXmlElement(NULL)
{
}

void CXmlElement::setXmlElement(_xmlNode* pXmlElement)
{
    _pXmlElement = pXmlElement;
}

string CXmlElement::getType() const
{
    return (const char*)_pXmlElement->name;
}

string CXmlElement::getPath() const
{
    string strPathElement = "/" + getType();

    if (hasAttribute("Name")) {

        strPathElement += "[@Name=" + getNameAttribute() + "]";
    }

    CXmlElement parentElement;

    if (getParentElement(parentElement)) {

        // Done
        return parentElement.getPath() + strPathElement;
    }
    return strPathElement;
}

string CXmlElement::getNameAttribute() const
{
    return getAttributeString("Name");
}

bool CXmlElement::hasAttribute(const string& strAttributeName) const
{
    return xmlHasProp(_pXmlElement, (const xmlChar*)strAttributeName.c_str()) != NULL;
}

string CXmlElement::getAttributeString(const string &strAttributeName) const
{
    if (!hasAttribute(strAttributeName)) {

        return "";
    }
    xmlChar* pucXmlValue = xmlGetProp((xmlNode*)_pXmlElement, (const xmlChar*)strAttributeName.c_str());

    string strValue((const char*)pucXmlValue);

    xmlFree(pucXmlValue);

    return strValue;
}

bool CXmlElement::getAttributeBoolean(const string& strAttributeName, const string& strTrueValue) const
{
    return getAttributeString(strAttributeName) == strTrueValue;
}

bool CXmlElement::getAttributeBoolean(const string& strAttributeName) const
{
    string strAttributeValue(getAttributeString(strAttributeName));

    return strAttributeValue == "true" || strAttributeValue == "1";
}

uint32_t CXmlElement::getAttributeInteger(const string &strAttributeName) const
{
    string strAttributeValue(getAttributeString(strAttributeName));

    return strtoul(strAttributeValue.c_str(), NULL, 0);
}

int32_t CXmlElement::getAttributeSignedInteger(const string &strAttributeName) const
{
    string strAttributeValue(getAttributeString(strAttributeName));

    return strtol(strAttributeValue.c_str(), NULL, 0);
}

double CXmlElement::getAttributeDouble(const string &strAttributeName) const
{
    string strAttributeValue(getAttributeString(strAttributeName));

    return strtod(strAttributeValue.c_str(), NULL);
}

string CXmlElement::getTextContent() const
{
    xmlChar* pucXmlContent = xmlNodeGetContent(_pXmlElement);

    string strContent((const char*)pucXmlContent);

    xmlFree(pucXmlContent);

    return strContent;
}

bool CXmlElement::getChildElement(const string& strType, CXmlElement& childElement) const
{
    CChildIterator childIterator(*this);

    while (childIterator.next(childElement)) {

        if (childElement.getType() == strType) {

            return true;
        }
    }
    return false;
}

bool CXmlElement::getChildElement(const string& strType, const string& strNameAttribute, CXmlElement& childElement) const
{
    CChildIterator childIterator(*this);

    while (childIterator.next(childElement)) {

        if ((childElement.getType() == strType) && (childElement.getNameAttribute() == strNameAttribute)) {

            return true;
        }
    }
    return false;
}

uint32_t CXmlElement::getNbChildElements() const
{
    CXmlElement childElement;
    uint32_t uiNbChildren = 0;

    CChildIterator childIterator(*this);

    while (childIterator.next(childElement)) {

        uiNbChildren++;
    }
    return uiNbChildren;
}

bool CXmlElement::getParentElement(CXmlElement& parentElement) const
{
    _xmlNode* pXmlNode = _pXmlElement->parent;

    if (pXmlNode->type == XML_ELEMENT_NODE) {

        parentElement.setXmlElement(pXmlNode);

        return true;
    }
    return false;
}

// Setters
void CXmlElement::setAttributeBoolean(const string& strAttributeName, bool bValue)
{
    setAttributeString(strAttributeName, bValue ? "true" : "false");
}


void CXmlElement::setAttributeString(const string& strAttributeName, const string& strValue)
{
    xmlNewProp(_pXmlElement, BAD_CAST strAttributeName.c_str(), BAD_CAST strValue.c_str());
}

void CXmlElement::setNameAttribute(const string& strValue)
{
    setAttributeString("Name", strValue);
}

void CXmlElement::setTextContent(const string& strContent)
{
    xmlAddChild(_pXmlElement, xmlNewText(BAD_CAST strContent.c_str()));
}

void CXmlElement::setComment(const string& strComment)
{
    xmlAddChild(_pXmlElement, xmlNewComment(BAD_CAST strComment.c_str()));
}

// Child creation
void CXmlElement::createChild(CXmlElement& childElement, const string& strType)
{
#ifdef LIBXML_TREE_ENABLED
    xmlNodePtr pChildNode = xmlNewChild(_pXmlElement, NULL, BAD_CAST strType.c_str(), NULL);

    childElement.setXmlElement(pChildNode);
#endif
}

// Child iteration
CXmlElement::CChildIterator::CChildIterator(const CXmlElement& xmlElement) : _pCurNode(xmlElement._pXmlElement->children)
{
}

bool CXmlElement::CChildIterator::next(CXmlElement& xmlChildElement)
{
    while (_pCurNode) {

        if (_pCurNode->type == XML_ELEMENT_NODE) {

            xmlChildElement.setXmlElement(_pCurNode);

            _pCurNode = _pCurNode->next;

            return true;
        }
        _pCurNode = _pCurNode->next;
    }

    return false;
}

