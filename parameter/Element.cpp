/*
 * Copyright (c) 2011-2015, Intel Corporation
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
#include "Element.h"
#include "XmlElementSerializingContext.h"
#include "ElementLibrary.h"
#include "ErrorContext.hpp"
#include "PfError.hpp"
#include <algorithm>
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

using std::string;

const std::string CElement::gDescriptionPropertyName = "Description";

CElement::CElement(const string &strName) : _strName(strName)
{
}

CElement::~CElement()
{
    removeChildren();
}

void CElement::setDescription(const string &strDescription)
{
    _strDescription = strDescription;
}

const string &CElement::getDescription() const
{
    return _strDescription;
}

bool CElement::childrenAreDynamic() const
{
    // By default, children are searched and not created during xml parsing
    return false;
}

bool CElement::init(string &strError)
{

    for (CElement *child : _childArray) {

        if (!child->init(strError)) {

            return false;
        }
    }

    return true;
}

string CElement::dumpContent(utility::ErrorContext &errorContext, const size_t depth) const
{
    string output;
    string strIndent;

    // Level
    size_t indents = depth;

    while (indents--) {

        strIndent += "    ";
    }
    // Type
    output += strIndent + "- " + getKind();

    // Name
    if (!_strName.empty()) {

        output += ": " + getName();
    }

    // Value
    string strValue = logValue(errorContext);

    if (!strValue.empty()) {

        output += " = " + strValue;
    }

    output += "\n";

    for (CElement *pChild : _childArray) {

        output += pChild->dumpContent(errorContext, depth + 1);
    }

    return output;
}

// Element properties
void CElement::showProperties(string &strResult) const
{
    strResult += "Kind: " + getKind() + "\n";
    showDescriptionProperty(strResult);
}

void CElement::showDescriptionProperty(std::string &strResult) const
{
    if (!getDescription().empty()) {
        strResult += gDescriptionPropertyName + ": " + getDescription() + "\n";
    }
}

// Content dumping
string CElement::logValue(utility::ErrorContext & /*ctx*/) const
{
    return "";
}

// From IXmlSink
bool CElement::fromXml(const CXmlElement &xmlElement,
                       CXmlSerializingContext &serializingContext) try {
    xmlElement.getAttribute(gDescriptionPropertyName, _strDescription);

    // Propagate through children
    CXmlElement::CChildIterator childIterator(xmlElement);

    CXmlElement childElement;

    while (childIterator.next(childElement)) {

        CElement *pChild;

        if (!childrenAreDynamic()) {

            pChild = findChildOfKind(childElement.getType());

            if (!pChild) {

                serializingContext.setError("Unable to handle XML element: " +
                                            childElement.getPath());

                return false;
            }

        } else {
            // Child needs creation
            pChild = createChild(childElement, serializingContext);

            if (!pChild) {

                return false;
            }
        }

        // Dig
        if (!pChild->fromXml(childElement, serializingContext)) {

            return false;
        }
    }

    return true;
} catch (const PfError &e) {
    serializingContext.appendLineToError(e.what());
    return false;
}

void CElement::childrenToXml(CXmlElement &xmlElement,
                             CXmlSerializingContext &serializingContext) const
{
    // Browse children and propagate
    for (CElement *pChild : _childArray) {

        // Create corresponding child element
        CXmlElement xmlChildElement;

        xmlElement.createChild(xmlChildElement, pChild->getXmlElementName());

        // Propagate
        pChild->toXml(xmlChildElement, serializingContext);
    }
}

void CElement::toXml(CXmlElement &xmlElement, CXmlSerializingContext &serializingContext) const
{
    setXmlNameAttribute(xmlElement);
    setXmlDescriptionAttribute(xmlElement);
    childrenToXml(xmlElement, serializingContext);
}

void CElement::setXmlDescriptionAttribute(CXmlElement &xmlElement) const
{
    const string &description = getDescription();
    if (!description.empty()) {
        xmlElement.setAttribute(gDescriptionPropertyName, description);
    }
}

void CElement::setXmlNameAttribute(CXmlElement &xmlElement) const
{
    // By default, set Name attribute if any
    string strName = getName();

    if (!strName.empty()) {

        xmlElement.setNameAttribute(strName);
    }
}

// Name
void CElement::setName(const string &strName)
{
    _strName = strName;
}

const string &CElement::getName() const
{
    return _strName;
}

bool CElement::rename(const string &strName, string &strError)
{
    // Check for conflict with brotherhood if relevant
    if (_pParent && _pParent->childrenAreDynamic()) {

        for (CElement *pParentChild : _pParent->_childArray) {

            if (pParentChild != this && pParentChild->getName() == strName) {

                // Conflict
                strError = "Name conflicts with brother element";

                return false;
            }
        }
    }
    // Change name
    setName(strName);

    return true;
}

string CElement::getPathName() const
{
    if (!_strName.empty()) {

        return _strName;
    } else {

        return getKind();
    }
}

// Hierarchy
void CElement::addChild(CElement *pChild)
{
    _childArray.push_back(pChild);

    pChild->_pParent = this;
}

CElement *CElement::getChild(size_t index)
{
    assert(index <= _childArray.size());

    return _childArray[index];
}

const CElement *CElement::getChild(size_t index) const
{
    assert(index <= _childArray.size());

    return _childArray[index];
}

CElement *CElement::createChild(const CXmlElement &childElement,
                                CXmlSerializingContext &serializingContext)
{
    // Context
    CXmlElementSerializingContext &elementSerializingContext =
        static_cast<CXmlElementSerializingContext &>(serializingContext);

    // Child needs creation
    CElement *pChild = elementSerializingContext.getElementLibrary()->createElement(childElement);

    if (!pChild) {

        elementSerializingContext.setError("Unable to create XML element " +
                                           childElement.getPath());

        return nullptr;
    }
    // Store created child!
    addChild(pChild);

    return pChild;
}

bool CElement::removeChild(CElement *pChild)
{
    auto childIt = find(begin(_childArray), end(_childArray), pChild);
    if (childIt != end(_childArray)) {

        _childArray.erase(childIt);
        return true;
    }
    return false;
}

void CElement::listChildren(string &strChildList) const
{
    // Get list of children names
    for (CElement *pChild : _childArray) {

        strChildList += pChild->getName() + "\n";
    }
}

string CElement::listQualifiedPaths(bool bDive, size_t level) const
{
    string strResult;

    // Dive Will cause only leaf nodes to be printed
    if (!bDive || !getNbChildren()) {

        strResult = getQualifiedPath() + "\n";
    }

    if (bDive || !level) {
        // Get list of children paths
        for (CElement *pChild : _childArray) {

            strResult += pChild->listQualifiedPaths(bDive, level + 1);
        }
    }
    return strResult;
}

void CElement::listChildrenPaths(string &strChildList) const
{
    // Get list of children paths
    for (CElement *pChild : _childArray) {

        strChildList += pChild->getPath() + "\n";
    }
}

size_t CElement::getNbChildren() const
{
    return _childArray.size();
}

const CElement *CElement::getParent() const
{
    return _pParent;
}

CElement *CElement::getParent()
{
    return _pParent;
}

void CElement::clean()
{
    if (childrenAreDynamic()) {

        removeChildren();
    } else {
        // Just propagate
        for (CElement *pChild : _childArray) {

            pChild->clean();
        }
    }
}

void CElement::removeChildren()
{
    // Delete in reverse order
    ChildArrayReverseIterator it;

    for (it = _childArray.rbegin(); it != _childArray.rend(); ++it) {

        delete *it;
    }
    _childArray.clear();
}

const CElement *CElement::findDescendant(CPathNavigator &pathNavigator) const
{
    string *pStrChildName = pathNavigator.next();

    if (!pStrChildName) {

        return this;
    }

    const CElement *pChild = findChild(*pStrChildName);

    if (!pChild) {

        return nullptr;
    }

    return pChild->findDescendant(pathNavigator);
}

CElement *CElement::findDescendant(CPathNavigator &pathNavigator)
{
    string *pStrChildName = pathNavigator.next();

    if (!pStrChildName) {

        return this;
    }

    CElement *pChild = findChild(*pStrChildName);

    if (!pChild) {

        return nullptr;
    }

    return pChild->findDescendant(pathNavigator);
}

bool CElement::isDescendantOf(const CElement *pCandidateAscendant) const
{
    if (!_pParent) {

        return false;
    }
    if (_pParent == pCandidateAscendant) {

        return true;
    }
    return _pParent->isDescendantOf(pCandidateAscendant);
}

CElement *CElement::findChild(const string &strName)
{
    for (CElement *pChild : _childArray) {

        if (pChild->getPathName() == strName) {

            return pChild;
        }
    }

    return nullptr;
}

const CElement *CElement::findChild(const string &strName) const
{
    for (CElement *pChild : _childArray) {

        if (pChild->getPathName() == strName) {

            return pChild;
        }
    }

    return nullptr;
}

CElement *CElement::findChildOfKind(const string &strKind)
{
    for (CElement *pChild : _childArray) {

        if (pChild->getKind() == strKind) {

            return pChild;
        }
    }

    return nullptr;
}

const CElement *CElement::findChildOfKind(const string &strKind) const
{
    for (CElement *pChild : _childArray) {

        if (pChild->getKind() == strKind) {

            return pChild;
        }
    }

    return nullptr;
}

string CElement::getPath() const
{
    // Take out root element from the path
    if (_pParent && _pParent->_pParent) {

        return _pParent->getPath() + "/" + getPathName();
    }
    return "/" + getPathName();
}

string CElement::getQualifiedPath() const
{
    return getPath() + " [" + getKind() + "]";
}

string CElement::getXmlElementName() const
{
    // Default to element kind
    return getKind();
}
