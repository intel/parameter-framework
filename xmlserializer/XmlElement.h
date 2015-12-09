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
    CXmlElement(_xmlNode *pXmlElement);
    CXmlElement();

    // Xml element
    void setXmlElement(_xmlNode *pXmlElement);

    // Getters
    std::string getType() const;
    std::string getPath() const;
    std::string getNameAttribute() const;
    bool hasAttribute(const std::string &strAttributeName) const;

    /** Get attribute
     *
     * If the attribute does not exists or there is a libxml2 error while
     * reading it or conversion from string to T fails, false is returned. In
     * case of failure, the content of value is the same as before calling
     * this method.
     *
     * Note: if T==string, no conversion takes place.
     *
     * @tparam T the type of the value to retrieve
     * @param[in] name The attribute name
     * @param[out] value The attribute value
     * @return true if success, false otherwise
     */
    template <typename T>
    bool getAttribute(const std::string &name, T &value) const;

    std::string getTextContent() const;

    // Navigation
    bool getChildElement(const std::string &strType, CXmlElement &childElement) const;
    bool getChildElement(const std::string &strType, const std::string &strNameAttribute,
                         CXmlElement &childElement) const;
    size_t getNbChildElements() const;
    bool getParentElement(CXmlElement &parentElement) const;

    /** Set attribute
     *
     * @tparam T the type of the value to retrieve
     * @param[in] name The attribute name
     * @param[in] value The attribute value
     */
    template <typename T>
    void setAttribute(const std::string &name, const T &value);
    /** Set attribute - special case for C-style strings
     *
     * @param[in] name The attribute name
     * @param[in] value The attribute value
     */
    void setAttribute(const std::string &name, const char *value);

    void setNameAttribute(const std::string &strValue);
    void setTextContent(const std::string &strContent);

    // Child creation
    void createChild(CXmlElement &childElement, const std::string &strType);

public:
    // Child iteration
    class CChildIterator
    {
    public:
        CChildIterator(const CXmlElement &xmlElement);

        bool next(CXmlElement &xmlChildElement);

    private:
        _xmlNode *_pCurNode;
    };

private:
    _xmlNode *_pXmlElement;
};
