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
    std::string getType() const;
    std::string getPath() const;
    std::string getNameAttribute() const;
    bool hasAttribute(const std::string& strAttributeName) const;
    bool getAttributeBoolean(const std::string& strAttributeName, const std::string& strTrueValue) const;
    bool getAttributeBoolean(const std::string& strAttributeName) const;
    std::string getAttributeString(const std::string& strAttributeName) const;
    uint32_t getAttributeInteger(const std::string& strAttributeName) const;
    int32_t getAttributeSignedInteger(const std::string& strAttributeName) const;
    double getAttributeDouble(const std::string& strAttributeName) const;
    std::string getTextContent() const;

    // Navigation
    bool getChildElement(const std::string& strType, CXmlElement& childElement) const;
    bool getChildElement(const std::string& strType, const std::string& strNameAttribute, CXmlElement& childElement) const;
    size_t getNbChildElements() const;
    bool getParentElement(CXmlElement& parentElement) const;

    // Setters
    void setAttributeBoolean(const std::string& strAttributeName, bool bValue);
    void setAttributeString(const std::string& strAttributeName, const std::string& strValue);
    void setNameAttribute(const std::string& strValue);
    void setTextContent(const std::string& strContent);
    void setAttributeInteger(const std::string& strAttributeName, uint32_t uiValue);
    /**
      * Set attribute with signed integer
      *
      * @param[in] strAttributeName The attribute name
      * @param[in] iValue The attribute value
      *
      */
    void setAttributeSignedInteger(const std::string& strAttributeName, int32_t iValue);

    // Child creation
    void createChild(CXmlElement& childElement, const std::string& strType);

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

