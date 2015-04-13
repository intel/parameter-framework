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
#include "XmlElement.h"
#include <libxml/tree.h>
#include <stdlib.h>
#include <sstream>

using std::string;
using std::ostringstream;

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
    if (pucXmlValue == NULL) {
        return "";
    }

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
    if (pucXmlContent == NULL) {
        return "";
    }

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

size_t CXmlElement::getNbChildElements() const
{
    CXmlElement childElement;
    size_t uiNbChildren = 0;

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

void CXmlElement::setAttributeInteger(const string& strAttributeName, uint32_t uiValue)
{
   ostringstream strStream;
   strStream << uiValue;
   setAttributeString(strAttributeName, strStream.str());
}

void CXmlElement::setAttributeSignedInteger(const string& strAttributeName, int32_t iValue)
{
   ostringstream strStream;
   strStream << iValue;
   setAttributeString(strAttributeName, strStream.str());
}

void CXmlElement::setNameAttribute(const string& strValue)
{
    setAttributeString("Name", strValue);
}

void CXmlElement::setTextContent(const string& strContent)
{
    xmlAddChild(_pXmlElement, xmlNewText(BAD_CAST strContent.c_str()));
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

