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
#pragma once

#include "parameter_export.h"

#include <string>
#include <vector>
#include <stdint.h>
#include "XmlSink.h"
#include "XmlSource.h"

#include "PathNavigator.h"

class CXmlElementSerializingContext;
class CErrorContext;

class CElement : public IXmlSink, public IXmlSource
{
public:
    PARAMETER_EXPORT
    CElement(const std::string& strName = "");
    PARAMETER_EXPORT
    virtual ~CElement();

    // Description
    PARAMETER_EXPORT
    void setDescription(const std::string& strDescription);
    PARAMETER_EXPORT
    const std::string& getDescription() const;

    // Name / Path
    PARAMETER_EXPORT
    const std::string& getName() const;
    PARAMETER_EXPORT
    void setName(const std::string& strName);
    PARAMETER_EXPORT
    bool rename(const std::string& strName, std::string& strError);
    PARAMETER_EXPORT
    std::string getPath() const;
    PARAMETER_EXPORT
    std::string getQualifiedPath() const;

    // Creation / build
    PARAMETER_EXPORT
    virtual bool init(std::string& strError);
    PARAMETER_EXPORT
    virtual void clean();

    // Children management
    PARAMETER_EXPORT
    void addChild(CElement* pChild);
    PARAMETER_EXPORT
    bool removeChild(CElement* pChild);
    PARAMETER_EXPORT
    void listChildren(std::string& strChildList) const;
    PARAMETER_EXPORT
    std::string listQualifiedPaths(bool bDive, size_t level = 0) const;
    PARAMETER_EXPORT
    void listChildrenPaths(std::string& strChildPathList) const;

    // Hierarchy query
    PARAMETER_EXPORT
    size_t getNbChildren() const;
    PARAMETER_EXPORT
    CElement* findChildOfKind(const std::string& strKind);
    PARAMETER_EXPORT
    const CElement* findChildOfKind(const std::string& strKind) const;
    PARAMETER_EXPORT
    const CElement* getParent() const;

    /**
     * Get a child element (const)
     *
     * Note: this method will assert if given a wrong child index (>= number of children)
     *
     * @param[in] index the index of the child element from 0 to number of children - 1
     * @return the child element
     */
    PARAMETER_EXPORT
    const CElement* getChild(size_t index) const;

    /**
     * Get a child element
     *
     * Note: this method will assert if given a wrong child index (>= number of children)
     *
     * @param[in] index the index of the child element from 0 to number of children - 1
     * @return the child element
     */
    PARAMETER_EXPORT
    CElement* getChild(size_t index);

    PARAMETER_EXPORT
    const CElement* findChild(const std::string& strName) const;
    PARAMETER_EXPORT
    CElement* findChild(const std::string& strName);
    PARAMETER_EXPORT
    const CElement* findDescendant(CPathNavigator& pathNavigator) const;
    PARAMETER_EXPORT
    CElement* findDescendant(CPathNavigator& pathNavigator);
    PARAMETER_EXPORT
    bool isDescendantOf(const CElement* pCandidateAscendant) const;

    // From IXmlSink
    PARAMETER_EXPORT
    virtual bool fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext);

    // From IXmlSource
    PARAMETER_EXPORT
    virtual void toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const;

    /**
     * Serialize the children to XML
     *
     * This method is virtual, to be derived in case a special treatment is
     * needed before doing so.
     *
     * @param[in,out] xmlElement the XML Element below which the children must
     *                be serialized (which may or may not be the CElement
     *                object upon which this method is called)
     * @param[in,out] serializingContext information about the serialization
     */
    PARAMETER_EXPORT
    virtual void childrenToXml(CXmlElement& xmlElement,
                               CXmlSerializingContext& serializingContext) const;

    // Content structure dump
    PARAMETER_EXPORT
    void dumpContent(std::string& strContent, CErrorContext& errorContext, const size_t depth = 0) const;

    // Element properties
    PARAMETER_EXPORT
    virtual void showProperties(std::string& strResult) const;

    // Class kind
    PARAMETER_EXPORT
    virtual std::string getKind() const = 0;

    /**
     * Fill the Description field of the Xml Element during XML composing.
     *
     * @param[in,out] xmlElement to fill with the description
     */
    PARAMETER_EXPORT
    void setXmlDescriptionAttribute(CXmlElement& xmlElement) const;

    /**
     * Appends if found human readable description property.
     *
     * @param[out] strResult in which the description is wished to be appended.
     */
    PARAMETER_EXPORT
    void showDescriptionProperty(std::string &strResult) const;

protected:
    // Content dumping
    PARAMETER_EXPORT
    virtual void logValue(std::string& strValue, CErrorContext& errorContext) const;

    // Hierarchy
    PARAMETER_EXPORT
    CElement* getParent();

    /**
     * Creates a child CElement from a child XML Element
     *
     * @param[in] childElement the XML element to create CElement from
     * @param[in] elementSerializingContext the serializing context
     *
     * @return child a pointer on the CElement object that has been added to the tree
     */
    PARAMETER_EXPORT
    CElement* createChild(const CXmlElement& childElement,
                          CXmlSerializingContext& elementSerializingContext);

    PARAMETER_EXPORT
    static const std::string gDescriptionPropertyName;

private:
    // Returns Name or Kind if no Name
    std::string getPathName() const;
    // Returns true if children dynamic creation is to be dealt with
    PARAMETER_EXPORT
    virtual bool childrenAreDynamic() const;
    // House keeping
    void removeChildren();
    // Fill XmlElement during XML composing
    void setXmlNameAttribute(CXmlElement& xmlElement) const;

    // Name
    std::string _strName;

    // Description
    std::string _strDescription;

    // Child iterators
    typedef std::vector<CElement*>::iterator ChildArrayIterator;
    typedef std::vector<CElement*>::reverse_iterator ChildArrayReverseIterator;
    // Children
    std::vector<CElement*> _childArray;
    // Parent
    CElement* _pParent;
};
