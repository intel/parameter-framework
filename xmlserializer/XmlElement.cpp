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
#include "convert.hpp"
#include <stdlib.h>

using std::string;

CXmlElement::CXmlElement(_xmlNode *pXmlElement) : _pXmlElement(pXmlElement)
{
}

CXmlElement::CXmlElement() : _pXmlElement(NULL)
{
}

void CXmlElement::setXmlElement(_xmlNode *pXmlElement)
{
    _pXmlElement = pXmlElement;
}

string CXmlElement::getType() const
{
    return (const char *)_pXmlElement->name;
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

bool CXmlElement::hasAttribute(const string &strAttributeName) const
{
    return xmlHasProp(_pXmlElement, (const xmlChar *)strAttributeName.c_str()) != NULL;
}

template <>
bool CXmlElement::getAttribute<std::string>(const string &name, string &value) const
{
    if (!hasAttribute(name)) {
        return false;
    }

    string backup = value;
    xmlChar *pucXmlValue = xmlGetProp((xmlNode *)_pXmlElement, (const xmlChar *)name.c_str());
    if (pucXmlValue == NULL) {
        value = backup;
        return false;
    }

    value = (const char *)pucXmlValue;

    xmlFree(pucXmlValue);

    return true;
}

template <typename T>
bool CXmlElement::getAttribute(const std::string &name, T &value) const
{
    std::string rawValue;
    if (!getAttribute(name, rawValue)) {
        return false;
    }

    T backup = value;
    if (!convertTo<T>(rawValue, value)) {
        value = backup;
        return false;
    }

    return true;
}

string CXmlElement::getNameAttribute() const
{
    string attribute;
    getAttribute("Name", attribute);
    return attribute;
}

string CXmlElement::getTextContent() const
{
    xmlChar *pucXmlContent = xmlNodeGetContent(_pXmlElement);
    if (pucXmlContent == NULL) {
        return "";
    }

    string strContent((const char *)pucXmlContent);

    xmlFree(pucXmlContent);

    return strContent;
}

bool CXmlElement::getChildElement(const string &strType, CXmlElement &childElement) const
{
    CChildIterator childIterator(*this);

    while (childIterator.next(childElement)) {

        if (childElement.getType() == strType) {

            return true;
        }
    }
    return false;
}

bool CXmlElement::getChildElement(const string &strType, const string &strNameAttribute,
                                  CXmlElement &childElement) const
{
    CChildIterator childIterator(*this);

    while (childIterator.next(childElement)) {

        if ((childElement.getType() == strType) &&
            (childElement.getNameAttribute() == strNameAttribute)) {

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

bool CXmlElement::getParentElement(CXmlElement &parentElement) const
{
    _xmlNode *pXmlNode = _pXmlElement->parent;

    if (pXmlNode->type == XML_ELEMENT_NODE) {

        parentElement.setXmlElement(pXmlNode);

        return true;
    }
    return false;
}

template <>
void CXmlElement::setAttribute<bool>(const string &name, const bool &value)
{
    setAttribute(name, value ? "true" : "false");
}

template <>
void CXmlElement::setAttribute<std::string>(const string &name, const string &value)
{
    setAttribute(name, value.c_str());
}

// This method exists for 2 reasons:
//  - at link time, all calls to setAttribute(const string&, const char [N])
//    for any value of N will all resolve to this method; this prevents the
//    need for one template instance per value of N.
//  - the libxml2 API takes a C-style string anyway.
void CXmlElement::setAttribute(const string &name, const char *value)
{
    xmlNewProp(_pXmlElement, BAD_CAST name.c_str(), BAD_CAST value);
}

template <typename T>
void CXmlElement::setAttribute(const std::string &name, const T &value)
{
    setAttribute(name, std::to_string(value).c_str());
}

void CXmlElement::setNameAttribute(const string &strValue)
{
    setAttribute("Name", strValue);
}

void CXmlElement::setTextContent(const string &strContent)
{
    xmlAddChild(_pXmlElement, xmlNewText(BAD_CAST strContent.c_str()));
}

// Child creation
void CXmlElement::createChild(CXmlElement &childElement, const string &strType)
{
#ifdef LIBXML_TREE_ENABLED
    xmlNodePtr pChildNode = xmlNewChild(_pXmlElement, NULL, BAD_CAST strType.c_str(), NULL);

    childElement.setXmlElement(pChildNode);
#endif
}

// Child iteration
CXmlElement::CChildIterator::CChildIterator(const CXmlElement &xmlElement)
    : _pCurNode(xmlElement._pXmlElement->children)
{
}

bool CXmlElement::CChildIterator::next(CXmlElement &xmlChildElement)
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

template bool CXmlElement::getAttribute(const std::string &name, std::string &value) const;
template bool CXmlElement::getAttribute(const std::string &name, bool &value) const;
template bool CXmlElement::getAttribute(const std::string &name, short &value) const;
template bool CXmlElement::getAttribute(const std::string &name, unsigned short &value) const;
template bool CXmlElement::getAttribute(const std::string &name, int &value) const;
template bool CXmlElement::getAttribute(const std::string &name, unsigned int &value) const;
template bool CXmlElement::getAttribute(const std::string &name, long &value) const;
template bool CXmlElement::getAttribute(const std::string &name, unsigned long &value) const;
template bool CXmlElement::getAttribute(const std::string &name, long long &value) const;
template bool CXmlElement::getAttribute(const std::string &name, unsigned long long &value) const;
template bool CXmlElement::getAttribute(const std::string &name, float &value) const;
template bool CXmlElement::getAttribute(const std::string &name, double &value) const;

template void CXmlElement::setAttribute(const std::string &name, const std::string &value);
template void CXmlElement::setAttribute(const std::string &name, const bool &value);
template void CXmlElement::setAttribute(const std::string &name, const short &value);
template void CXmlElement::setAttribute(const std::string &name, const unsigned short &value);
template void CXmlElement::setAttribute(const std::string &name, const int &value);
template void CXmlElement::setAttribute(const std::string &name, const unsigned int &value);
template void CXmlElement::setAttribute(const std::string &name, const long &value);
template void CXmlElement::setAttribute(const std::string &name, const unsigned long &value);
template void CXmlElement::setAttribute(const std::string &name, const long long &value);
template void CXmlElement::setAttribute(const std::string &name, const unsigned long long &value);
template void CXmlElement::setAttribute(const std::string &name, const float &value);
template void CXmlElement::setAttribute(const std::string &name, const double &value);
