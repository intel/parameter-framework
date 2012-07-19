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

#include <stdint.h>
#include <string>

using namespace std;

struct _xmlNode;
struct _xmlDoc;

class CXmlElement
{
    friend class CChildIterator;
public:
    CXmlElement(_xmlNode* pXmlElement);
    CXmlElement();

    // Xml element
    void setXmlElement(_xmlNode* pXmlElement);

    // Getters
    string getType() const;
    string getPath() const;
    string getNameAttribute() const;
    bool hasAttribute(const string& strAttributeName) const;
    bool getAttributeBoolean(const string& strAttributeName, const string& strTrueValue) const;
    bool getAttributeBoolean(const string& strAttributeName) const;
    string getAttributeString(const string& strAttributeName) const;
    uint32_t getAttributeInteger(const string& strAttributeName) const;
    int32_t getAttributeSignedInteger(const string& strAttributeName) const;
    double getAttributeDouble(const string& strAttributeName) const;
    string getTextContent() const;

    // Navigation
    bool getChildElement(const string& strType, CXmlElement& childElement) const;
    bool getChildElement(const string& strType, const string& strNameAttribute, CXmlElement& childElement) const;
    uint32_t getNbChildElements() const;
    bool getParentElement(CXmlElement& parentElement) const;

    // Setters
    void setAttributeBoolean(const string& strAttributeName, bool bValue);
    void setAttributeString(const string& strAttributeName, const string& strValue);
    void setNameAttribute(const string& strValue);
    void setTextContent(const string& strContent);
    void setComment(const string& strComment);

    // Child creation
    void createChild(CXmlElement& childElement, const string& strType);

public:
    // Child iteration
    class CChildIterator
    {
    public:
        CChildIterator(const CXmlElement& xmlElement);

        bool next(CXmlElement& xmlChildElement);
    private:
        _xmlNode* _pCurNode;
    };

private:
    _xmlNode* _pXmlElement;
};

