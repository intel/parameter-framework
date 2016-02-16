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

#include "Element.h"

#include <list>
#include <vector>

class CConfigurableDomain;
class CSyncerSet;
class ISyncer;
class CSubsystem;
class CConfigurationAccessContext;
class CParameterAccessContext;
class CAreaConfiguration;

class PARAMETER_EXPORT CConfigurableElement : public CElement
{
    friend class CConfigurableDomain;
    friend class CDomainConfiguration;
    typedef std::list<const CConfigurableDomain *>::const_iterator
        ConfigurableDomainListConstIterator;

public:
    CConfigurableElement(const std::string &strName = "");
    virtual ~CConfigurableElement() = default;

    // Offset in main blackboard
    void setOffset(size_t offset);
    size_t getOffset() const;

    // Allocation
    virtual size_t getFootPrint() const;

    // Syncer set (me, ascendant or descendant ones)
    void fillSyncerSet(CSyncerSet &syncerSet) const;

    // Belonging domain
    bool belongsTo(const CConfigurableDomain *pConfigurableDomain) const;

    // Belonging domains
    void listBelongingDomains(std::string &strResult, bool bVertical = true) const;

    // Matching check for domain association
    bool hasNoDomainAssociated() const;

    // Matching check for no valid associated domains
    bool hasNoValidDomainAssociated() const;

    // Owning domains
    void listAssociatedDomains(std::string &strResult, bool bVertical = true) const;
    size_t getBelongingDomainCount() const;

    // Elements with no domains
    void listRogueElements(std::string &strResult) const;

    /** @return true if element is rogue, false otherwise
     *
     * An element is rogue if it is disjoint with all domains.
     *
     * Ie: An element is rogue if neither its descendants, ascendants
     *     nor itself are associated with any domain.
     *
     * Ie: An element is *not* rogue if any of its descendants, ascendants
     *     or itself are associated with at least one domain.
     */
    bool isRogue() const;

    // Footprint as string
    std::string getFootprintAsString() const;

    // Belonging subsystem
    virtual const CSubsystem *getBelongingSubsystem() const;

    // Check element is a parameter
    virtual bool isParameter() const;

    // AreaConfiguration creation
    virtual CAreaConfiguration *createAreaConfiguration(const CSyncerSet *pSyncerSet) const;

    // Parameter access
    virtual bool accessValue(CPathNavigator &pathNavigator, std::string &strValue, bool bSet,
                             CParameterAccessContext &parameterAccessContext) const;

    /** Gets the element as an array of bytes.
     *
     * This is like having a direct access to the blackboard.
     *
     * @param[out] bytes Where to store the result.
     * @param[in] parameterAccessContext Context containing the blackboard to
     *            read from.
     */
    void getSettingsAsBytes(std::vector<uint8_t> &bytes,
                            CParameterAccessContext &parameterAccessContext) const;
    /** Sets the element as if it was an array of bytes.
     *
     * This is like having a direct access to the blackboard.
     *
     * @param[out] bytes The content to be set.
     * @param[in] parameterAccessContext Context containing the blackboard to
     *            write to.
     */
    bool setSettingsAsBytes(const std::vector<uint8_t> &bytes,
                            CParameterAccessContext &parameterAccessContext) const;

    /** @return List of all ConfigurableElements that have a mapping relevant in this context.
     *          Ie: return self and CConfigurableElement ancestor of this node.
     *
     */
    std::list<const CConfigurableElement *> getConfigurableElementContext() const;

    // Used for simulation and virtual subsystems
    virtual void setDefaultValues(CParameterAccessContext &parameterAccessContext) const;

    // Element properties
    virtual void showProperties(std::string &strResult) const;

    /**
     * Get the value associated to a mapping key in the object's mapping
     *
     * @param[in] strKey the mapping key
     * @param[out] pStrValue the associated value
     *
     * @return true if @p strKey is found in the object's mapping, false if not
     */
    virtual bool getMappingData(const std::string &strKey, const std::string *&pStrValue) const = 0;
    /** Get the string representation of the mapping
     *
     * If applicable, amend values are applied to the leaf element.
     */
    virtual std::string getFormattedMapping() const = 0;

    // XML configuration settings parsing
    virtual bool serializeXmlSettings(
        CXmlElement &xmlConfigurationSettingsElementContent,
        CConfigurationAccessContext &configurationAccessContext) const;

    bool fromXml(const CXmlElement &xmlElement,
                 CXmlSerializingContext &serializingContext) override final;

    void toXml(CXmlElement &xmlElement,
               CXmlSerializingContext &serializingContext) const override final;

    /** Deserialize the structure from xml. */
    virtual bool structureFromXml(const CXmlElement &xmlElement,
                                  CXmlSerializingContext &serializingContext)
    {
        // Forward to Element::fromXml.
        // This is unfortunate as Element::fromXml will call back
        // fromXml on each children.
        // Thus on each non leaf node of the tree, the code will test if
        // the setting or the structure are to be serialized.
        // This test could be avoided by several ways including:
        //  - split 2 roles fromXml in two function
        //    1) construct the elements
        //    2) recursive call on children
        //  - dispatch in with a virtual method. This would not not remove
        //    the branching rather hide it behind a virtual method override.
        return CElement::fromXml(xmlElement, serializingContext);
    }

    /** Serialize the structure to xml. */
    virtual void structureToXml(CXmlElement &xmlElement,
                                CXmlSerializingContext &serializingContext) const
    {
        // See structureFromXml implementation comment.
        CElement::toXml(xmlElement, serializingContext);
    }

protected:
    // Syncer (me or ascendant)
    virtual ISyncer *getSyncer() const;
    // Syncer set (descendant)
    virtual void fillSyncerSetFromDescendant(CSyncerSet &syncerSet) const;
    // Configuration Domain local search
    bool containsConfigurableDomain(const CConfigurableDomain *pConfigurableDomain) const;

private:
    // Content dumping. Override and stop further deriving: Configurable
    // Elements should be called with the overloaded version taking a
    // "Parameter Access Context" (The name is misleading as it is actually
    // used to access any Configurable Element).
    std::string logValue(utility::ErrorContext &errorContext) const override final;
    virtual std::string logValue(CParameterAccessContext &context) const;

    // Configurable domain association
    void addAttachedConfigurableDomain(const CConfigurableDomain *pConfigurableDomain);
    void removeAttachedConfigurableDomain(const CConfigurableDomain *pConfigurableDomain);

    // Belonging domain ascending search
    bool belongsToDomainAscending(const CConfigurableDomain *pConfigurableDomain) const;

    // Belonging domains
    void getBelongingDomains(std::list<const CConfigurableDomain *> &configurableDomainList) const;
    void listDomains(const std::list<const CConfigurableDomain *> &configurableDomainList,
                     std::string &strResult, bool bVertical) const;

    // Check parent is still of current type (by structure knowledge)
    bool isOfConfigurableElementType(const CElement *pParent) const;

    // Offset in main blackboard
    size_t _offset{0};

    // Associated configurable domains
    std::list<const CConfigurableDomain *> _configurableDomainList;
};
