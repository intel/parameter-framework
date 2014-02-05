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
    void setAttributeInteger(const string& strAttributeName, uint32_t uiValue);

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

