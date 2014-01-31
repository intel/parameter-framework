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
#pragma once

#include <string>
#include <vector>
#include <stdint.h>
#include <list>
#include "XmlSink.h"
#include "XmlSource.h"

#include "PathNavigator.h"

using namespace std;

class CXmlElementSerializingContext;
class CErrorContext;

class CElement : public IXmlSink, public IXmlSource
{
    friend class CAutoLog;
public:
    CElement(const string& strName = "");
    virtual ~CElement();

    // Logging
    void log_info(const string& strMessage, ...) const;
    void log_warning(const string& strMessage, ...) const;
    void log_table(bool bIsWarning, const list<string> lstrMessage) const;

    // Description
    void setDescription(const string& strDescription);
    const string& getDescription() const;

    // Name / Path
    const string& getName() const;
    void setName(const string& strName);
    bool rename(const string& strName, string& strError);
    string getPath() const;
    string getQualifiedPath() const;

    // Creation / build
    virtual bool init(string& strError);
    virtual void clean();

    // Children management
    void addChild(CElement* pChild);
    bool removeChild(CElement* pChild);
    void listChildren(string& strChildList) const;
    string listQualifiedPaths(bool bDive, uint32_t uiLevel = 0) const;
    void listChildrenPaths(string& strChildPathList) const;

    // Hierarchy query
    uint32_t getNbChildren() const;
    CElement* findChildOfKind(const string& strKind);
    const CElement* findChildOfKind(const string& strKind) const;
    const CElement* getParent() const;
    const CElement* getChild(uint32_t uiIndex) const;
    CElement* getChild(uint32_t uiIndex);
    const CElement* findChild(const string& strName) const;
    CElement* findChild(const string& strName);
    const CElement* findDescendant(CPathNavigator& pathNavigator) const;
    CElement* findDescendant(CPathNavigator& pathNavigator);
    bool isDescendantOf(const CElement* pCandidateAscendant) const;

    // From IXmlSink
    virtual bool fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext);

    // From IXmlSource
    virtual void toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const;

    // Content structure dump
    void dumpContent(string& strContent, CErrorContext& errorContext, const uint32_t uiDepth = 0) const;

    // Element properties
    virtual void showProperties(string& strResult) const;

    // Conversion utilities
    static string toString(uint32_t uiValue);
    static string toString(uint64_t uiValue);
    static string toString(int32_t iValue);
    static string toString(double dValue);

    // Checksum for integrity checks
    uint8_t computeStructureChecksum() const;

    // Class kind
    virtual string getKind() const = 0;
protected:
    // Content dumping
    virtual void logValue(string& strValue, CErrorContext& errorContext) const;
    // Utility to underline
    static void appendTitle(string& strTo, const string& strTitle);

    // Hierarchy
    CElement* getLastChild();
    CElement* getParent();
    CElement* findAscendantOfKind(const string& strKind);
    CElement* getRoot();
    const CElement* getRoot() const;

    /**
     * Creates a child CElement from a child XML Element
     *
     * @param[in] childElement the XML element to create CElement from
     * @param[in] elementSerializingContext the serializing context
     *
     * @return child a pointer on the CElement object that has been added to the tree
     */
    CElement* createChild(const CXmlElement& childElement,
                          CXmlSerializingContext& elementSerializingContext);

private:
    // Logging (done by root)
    virtual void doLog(bool bIsWarning, const string& strLog) const;
    virtual void nestLog() const;
    virtual void unnestLog() const;
    // Returns Name or Kind if no Name
    string getPathName() const;
    // Returns true if children dynamic creation is to be dealt with
    virtual bool childrenAreDynamic() const;
    // House keeping
    void removeChildren();
    // For logging
    uint32_t getDepth() const;
    // Fill XmlElement during XML composing
    void setXmlNameAttribute(CXmlElement& xmlElement) const;

    // Name
    string _strName;

    // Description
    string _strDescription;

    // Child iterators
    typedef vector<CElement*>::iterator ChildArrayIterator;
    typedef vector<CElement*>::reverse_iterator ChildArrayReverseIterator;
    // Children
    vector<CElement*> _childArray;
    // Parent
    CElement* _pParent;
};
