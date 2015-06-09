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
#include "Element.h"
#include "XmlElementSerializingContext.h"
#include "ElementLibrary.h"
#include "ErrorContext.h"
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

using std::string;

const std::string CElement::gDescriptionPropertyName = "Description";

CElement::CElement(const string& strName) : _strName(strName), _pParent(NULL)
{
}

CElement::~CElement()
{
    removeChildren();
}

// Logging
void CElement::log_info(const char* strMessage, ...) const
{
    char *pacBuffer;
    va_list listPointer;

    va_start(listPointer, strMessage);

    vasprintf(&pacBuffer,  strMessage, listPointer);

    va_end(listPointer);

    if (pacBuffer != NULL) {
        doLog(false, pacBuffer);
    }

    free(pacBuffer);
}

void CElement::log_warning(const char* strMessage, ...) const
{
    char *pacBuffer;
    va_list listPointer;

    va_start(listPointer, strMessage);

    vasprintf(&pacBuffer,  strMessage, listPointer);

    va_end(listPointer);

    if (pacBuffer != NULL) {
        doLog(true, pacBuffer);
    }

    free(pacBuffer);
}

// Log each element of the string list
void CElement::log_table(bool bIsWarning, const std::list<string> lstrMessage) const
{
    std::list<string>::const_iterator iterator(lstrMessage.begin());
    std::list<string>::const_iterator end(lstrMessage.end());

    while (iterator != end) {
        // Log current list element
        doLog(bIsWarning, iterator->c_str());
        ++iterator;
    }
}

void CElement::doLog(bool bIsWarning, const string& strLog) const
{
    assert(_pParent);

    // Propagate till root
    _pParent->doLog(bIsWarning, strLog);
}

void CElement::nestLog() const
{
    assert(_pParent);

    // Propagate till root
    _pParent->nestLog();
}

void CElement::unnestLog() const
{
    assert(_pParent);

    // Propagate till root
    _pParent->unnestLog();
}


void CElement::setDescription(const string& strDescription)
{
    _strDescription = strDescription;
}

const string& CElement::getDescription() const
{
    return _strDescription;
}

bool CElement::childrenAreDynamic() const
{
    // By default, children are searched and not created during xml parsing
    return false;
}

bool CElement::init(string& strError)
{
    uint32_t uiIndex;

    for (uiIndex = 0; uiIndex < _childArray.size(); uiIndex++) {

        CElement* pElement = _childArray[uiIndex];;

        if (!pElement->init(strError)) {

            return false;
        }
    }

    return true;
}

void CElement::dumpContent(string& strContent, CErrorContext& errorContext, const uint32_t uiDepth) const
{
    string strIndent;

    // Level
    uint32_t uiNbIndents = uiDepth;

    while (uiNbIndents--) {

        strIndent += "    ";
    }
    // Type
    strContent += strIndent + "- " + getKind();

    // Name
    if (!_strName.empty()) {

        strContent += ": " + getName();
    }

    // Value
    string strValue;
    logValue(strValue, errorContext);

    if (!strValue.empty()) {

        strContent += " = " + strValue;
    }

    strContent += "\n";

    uint32_t uiIndex;

    for (uiIndex = 0; uiIndex < _childArray.size(); uiIndex++) {

        _childArray[uiIndex]->dumpContent(strContent, errorContext, uiDepth + 1);
    }
}

// Element properties
void CElement::showProperties(string& strResult) const
{
    strResult = "\n";
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
void CElement::logValue(string& strValue, CErrorContext& errorContext) const
{
    (void)strValue;
    (void)errorContext;
}

// From IXmlSink
bool CElement::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    setDescription(getXmlDescriptionAttribute(xmlElement));

    // Propagate through children
    CXmlElement::CChildIterator childIterator(xmlElement);

    // Context
    CXmlElementSerializingContext& elementSerializingContext = static_cast<CXmlElementSerializingContext&>(serializingContext);

    CXmlElement childElement;

    while (childIterator.next(childElement)) {

        CElement* pChild;

        if (!childrenAreDynamic()) {

            pChild = findChildOfKind(childElement.getType());

            if (!pChild) {

                elementSerializingContext.setError("Unable to handle XML element: " + childElement.getPath());

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
        if (!pChild->fromXml(childElement, elementSerializingContext)) {

            return false;
        }
    }

    return true;
}

void CElement::childrenToXml(CXmlElement& xmlElement,
                             CXmlSerializingContext& serializingContext) const
{
    // Browse children and propagate
    size_t uiNbChildren = getNbChildren();
    size_t uiChild;

    for (uiChild = 0; uiChild < uiNbChildren; uiChild++) {

        const CElement* pChild = _childArray[uiChild];

        // Create corresponding child element
        CXmlElement xmlChildElement;

        xmlElement.createChild(xmlChildElement, pChild->getKind());

        // Propagate
        pChild->toXml(xmlChildElement, serializingContext);
    }
}

void CElement::toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const
{
    setXmlNameAttribute(xmlElement);
    setXmlDescriptionAttribute(xmlElement);
    childrenToXml(xmlElement, serializingContext);
}

void CElement::setXmlDescriptionAttribute(CXmlElement& xmlElement) const
{
    const string &description = getDescription();
    if (!description.empty()) {
        xmlElement.setAttributeString(gDescriptionPropertyName, description);
    }
}

string CElement::getXmlDescriptionAttribute(const CXmlElement& xmlElement) const
{
    return xmlElement.getAttributeString(gDescriptionPropertyName);
}

void CElement::setXmlNameAttribute(CXmlElement& xmlElement) const
{
    // By default, set Name attribute if any
    string strName = getName();

    if (!strName.empty()) {

        xmlElement.setNameAttribute(strName);
    }
}

// Name
void CElement::setName(const string& strName)
{
    _strName = strName;
}

const string& CElement::getName() const
{
    return _strName;
}

bool CElement::rename(const string& strName, string& strError)
{
    // Check for conflict with brotherhood if relevant
    if (_pParent && _pParent->childrenAreDynamic()) {

        size_t uiParentChild;
        size_t uiParentNbChildren = _pParent->getNbChildren();

        for (uiParentChild = 0; uiParentChild < uiParentNbChildren; uiParentChild++) {

            const CElement* pParentChild = _pParent->getChild(uiParentChild);

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
void CElement::addChild(CElement* pChild)
{
    _childArray.push_back(pChild);

    pChild->_pParent = this;
}

CElement* CElement::getChild(size_t uiIndex)
{
    assert(uiIndex <= _childArray.size());

    return _childArray[uiIndex];
}

const CElement* CElement::getChild(size_t uiIndex) const
{
    assert(uiIndex <= _childArray.size());

    return _childArray[uiIndex];
}

CElement* CElement::createChild(const CXmlElement& childElement,
                                CXmlSerializingContext& serializingContext)
{
    // Context
    CXmlElementSerializingContext& elementSerializingContext =
            static_cast<CXmlElementSerializingContext&>(serializingContext);

    // Child needs creation
    CElement* pChild = elementSerializingContext.getElementLibrary()->createElement(childElement);

    if (!pChild) {

        elementSerializingContext.setError(
                    "Unable to create XML element " + childElement.getPath());

        return NULL;
    }
    // Store created child!
    addChild(pChild);

    return pChild;
}

bool CElement::removeChild(CElement* pChild)
{
    ChildArrayIterator it;

    for (it = _childArray.begin(); it != _childArray.end(); ++it) {

        CElement* pElement = *it;

        if (pElement == pChild) {

            _childArray.erase(it);

            return true;
        }
    }
    return false;
}

void CElement::listChildren(string& strChildList) const
{
    strChildList = "\n";

    // Get list of children names
    size_t uiNbChildren = getNbChildren();
    size_t uiChild;

    for (uiChild = 0; uiChild < uiNbChildren; uiChild++) {

        const CElement* pChild = _childArray[uiChild];

        strChildList += pChild->getName() + "\n";
    }
}

string CElement::listQualifiedPaths(bool bDive, uint32_t uiLevel) const
{
    size_t uiNbChildren = getNbChildren();
    string strResult;

    // Dive Will cause only leaf nodes to be printed
    if (!bDive || !uiNbChildren) {

        strResult = getQualifiedPath() + "\n";
    }

    if (bDive || !uiLevel) {
        // Get list of children paths
        size_t uiChild;

        for (uiChild = 0; uiChild < uiNbChildren; uiChild++) {

            const CElement* pChild = _childArray[uiChild];

            strResult += pChild->listQualifiedPaths(bDive, uiLevel + 1);
        }
    }
    return strResult;
}

void CElement::listChildrenPaths(string& strChildList) const
{
    // Get list of children paths
    size_t uiNbChildren = getNbChildren();
    size_t uiChild;

    for (uiChild = 0; uiChild < uiNbChildren; uiChild++) {

        const CElement* pChild = _childArray[uiChild];

        strChildList += pChild->getPath() + "\n";
    }
}

size_t CElement::getNbChildren() const
{
    return _childArray.size();
}

const CElement* CElement::getParent() const
{
    return _pParent;
}

CElement* CElement::getParent()
{
    return _pParent;
}

void CElement::clean()
{
    if (childrenAreDynamic()) {

        removeChildren();
    } else {
        // Just propagate
        uint32_t uiIndex;

        for (uiIndex = 0; uiIndex < _childArray.size(); uiIndex++) {

            _childArray[uiIndex]->clean();
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

const CElement* CElement::findDescendant(CPathNavigator& pathNavigator) const
{
    string* pStrChildName = pathNavigator.next();

    if (!pStrChildName) {

        return this;
    }

    const CElement* pChild = findChild(*pStrChildName);

    if (!pChild) {

        return NULL;
    }

    return pChild->findDescendant(pathNavigator);
}

CElement* CElement::findDescendant(CPathNavigator& pathNavigator)
{
    string* pStrChildName = pathNavigator.next();

    if (!pStrChildName) {

        return this;
    }

    CElement* pChild = findChild(*pStrChildName);

    if (!pChild) {

        return NULL;
    }

    return pChild->findDescendant(pathNavigator);
}

bool CElement::isDescendantOf(const CElement* pCandidateAscendant) const
{
    if (!_pParent) {

        return false;
    }
    if (_pParent == pCandidateAscendant) {

        return true;
    }
    return _pParent->isDescendantOf(pCandidateAscendant);
}

CElement* CElement::findChild(const string& strName)
{
    uint32_t uiIndex;

    for (uiIndex = 0; uiIndex < _childArray.size(); uiIndex++) {

        CElement* pElement = _childArray[uiIndex];

        if (pElement->getPathName() == strName) {

            return pElement;
        }
    }

    return NULL;
}

const CElement* CElement::findChild(const string& strName) const
{
    uint32_t uiIndex;

    for (uiIndex = 0; uiIndex < _childArray.size(); uiIndex++) {

        const CElement* pElement = _childArray[uiIndex];

        if (pElement->getPathName() == strName) {

            return pElement;
        }
    }

    return NULL;
}

CElement* CElement::findChildOfKind(const string& strKind)
{
    uint32_t uiIndex;

    for (uiIndex = 0; uiIndex < _childArray.size(); uiIndex++) {

        CElement* pElement = _childArray[uiIndex];

        if (pElement->getKind() == strKind) {

            return pElement;
        }
    }

    return NULL;
}

const CElement* CElement::findChildOfKind(const string& strKind) const
{
    uint32_t uiIndex;

    for (uiIndex = 0; uiIndex < _childArray.size(); uiIndex++) {

        const CElement* pElement = _childArray[uiIndex];;

        if (pElement->getKind() == strKind) {

            return pElement;
        }
    }

    return NULL;
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

uint32_t CElement::getDepth() const
{
    if (_pParent) {

        return _pParent->getDepth() + 1;
    }

    return 0;
}

// Checksum for integrity checks
uint8_t CElement::computeStructureChecksum() const
{
    // Base checksum computation on element kind
    string strKind = getKind();

    // Get element kind
    const char* pcData = strKind.c_str();

    // Cumulate
    uint8_t uiChecksum = 0;

    while (*pcData) {

        uiChecksum += *pcData++;
    }

    // Propagate
    uint32_t uiIndex;
    for (uiIndex = 0; uiIndex < _childArray.size(); uiIndex++) {

        const CElement* pChild = _childArray[uiIndex];

        uiChecksum += pChild->computeStructureChecksum();
    }

    return uiChecksum;
}
