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
#include "Element.h"
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <sstream>
#include "XmlElementSerializingContext.h"
#include "ElementLibrary.h"
#include "ErrorContext.h"

CElement::CElement(const string& strName) : _strName(strName), _pParent(NULL)
{
}

CElement::~CElement()
{
    removeChildren();
}

// Logging
void CElement::log(const string& strMessage, ...) const
{
    char acBuffer[512];
    va_list listPointer;

    va_start(listPointer, strMessage);

    vsnprintf(acBuffer, sizeof(acBuffer), strMessage.c_str(), listPointer);

    va_end(listPointer);

    doLog(acBuffer);
}

void CElement::doLog(const string& strLog) const
{
    assert(_pParent);

    // Propagate till root
    _pParent->doLog(strLog);
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
}

// Conversion utilities
string CElement::toString(uint32_t uiValue)
{
    ostringstream ostr;

    ostr << uiValue;

    return ostr.str();
}

string CElement::toString(uint64_t uiValue)
{
    ostringstream ostr;

    ostr << uiValue;

    return ostr.str();
}

string CElement::toString(int32_t iValue)
{
    ostringstream ostr;

    ostr << iValue;

    return ostr.str();
}

string CElement::toString(double dValue)
{
    ostringstream ostr;

    ostr << dValue;

    return ostr.str();
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
            pChild = elementSerializingContext.getElementLibrary()->createElement(childElement);

            if (pChild) {

                // Store created child!
                addChild(pChild);
            } else {

                elementSerializingContext.setError("Unable to create XML element " + childElement.getPath());

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

// From IXmlSource
void CElement::toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const
{
    // Browse children and propagate
    uint32_t uiNbChildren = getNbChildren();
    uint32_t uiChild;

    for (uiChild = 0; uiChild < uiNbChildren; uiChild++) {

        const CElement* pChild = _childArray[uiChild];

        // Create corresponding child element
        CXmlElement xmlChildElement;

        xmlElement.createChild(xmlChildElement, pChild->getKind());

        // Set attributes
        pChild->setXmlNameAttribute(xmlChildElement);


        // Propagate
        pChild->toXml(xmlChildElement, serializingContext);
    }
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

        uint32_t uiParentChild;
        uint32_t uiParentNbChildren = _pParent->getNbChildren();

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

CElement* CElement::getChild(uint32_t uiIndex)
{
    assert(uiIndex <= _childArray.size());

    return _childArray[uiIndex];
}

const CElement* CElement::getChild(uint32_t uiIndex) const
{
    assert(uiIndex <= _childArray.size());

    return _childArray[uiIndex];
}

CElement* CElement::getLastChild()
{
    uint32_t uiNbChildren = getNbChildren();

    assert(uiNbChildren);

    return _childArray[uiNbChildren - 1];
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
    uint32_t uiNbChildren = getNbChildren();
    uint32_t uiChild;

    for (uiChild = 0; uiChild < uiNbChildren; uiChild++) {

        const CElement* pChild = _childArray[uiChild];

        strChildList += pChild->getName() + "\n";
    }
}

string CElement::listQualifiedPaths(bool bDive, uint32_t uiLevel) const
{
    uint32_t uiNbChildren = getNbChildren();
    string strResult;

    // Dive Will cause only leaf nodes to be printed
    if (!bDive || !uiNbChildren) {

        strResult = getQualifiedPath() + "\n";
    }

    if (bDive || !uiLevel) {
        // Get list of children paths
        uint32_t uiChild;

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
    uint32_t uiNbChildren = getNbChildren();
    uint32_t uiChild;

    for (uiChild = 0; uiChild < uiNbChildren; uiChild++) {

        const CElement* pChild = _childArray[uiChild];

        strChildList += pChild->getPath() + "\n";
    }
}

uint32_t CElement::getNbChildren() const
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

CElement* CElement::findAscendantOfKind(const string& strKind)
{
    if (!_pParent) {

        return NULL;
    }

    if (_pParent->getKind() == strKind) {

        return _pParent;
    }
    return _pParent->findAscendantOfKind(strKind);
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

CElement* CElement::getRoot()
{
    if (!_pParent) {

        return this;
    }
    return _pParent->getRoot();
}

const CElement* CElement::getRoot() const
{
    if (!_pParent) {

        return this;
    }
    return _pParent->getRoot();
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

// Utility to underline
void CElement::appendTitle(string& strTo, const string& strTitle)
{
    strTo += "\n" + strTitle + "\n";

    uint32_t uiLength = strTitle.size();

    while (uiLength--) {

        strTo += "=";
    }
    strTo += "\n";
}
