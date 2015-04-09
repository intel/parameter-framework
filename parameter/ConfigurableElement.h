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

#include "Element.h"

#include <list>

class CConfigurableDomain;
class CSyncerSet;
class ISyncer;
class CSubsystem;
class CConfigurationAccessContext;
class CParameterAccessContext;
class CAreaConfiguration;

class CConfigurableElement : public CElement
{
    friend class CConfigurableDomain;
    friend class CDomainConfiguration;
    typedef std::list<const CConfigurableDomain*>::const_iterator ConfigurableDomainListConstIterator;
public:
    CConfigurableElement(const std::string& strName = "");
    virtual ~CConfigurableElement();

    // Offset in main blackboard
    void setOffset(uint32_t uiOffset);
    uint32_t getOffset() const;

    // Allocation
    virtual uint32_t getFootPrint() const;

    // Syncer set (me, ascendant or descendant ones)
    void fillSyncerSet(CSyncerSet& syncerSet) const;

    // Belonging domain
    bool belongsTo(const CConfigurableDomain* pConfigurableDomain) const;

    // Belonging domains
    void listBelongingDomains(std::string& strResult, bool bVertical = true) const;

    // Matching check for domain association
    bool hasNoDomainAssociated() const;

    // Matching check for no valid associated domains
    bool hasNoValidDomainAssociated() const;

    // Owning domains
    void listAssociatedDomains(std::string& strResult, bool bVertical = true) const;
    size_t getBelongingDomainCount() const;

    // Elements with no domains
    void listRogueElements(std::string& strResult) const;

    // Belonging to no domains
    bool isRogue() const;

    // Footprint as string
    std::string getFootprintAsString() const;

    // Belonging subsystem
    virtual const CSubsystem* getBelongingSubsystem() const;

    // Check element is a parameter
    virtual bool isParameter() const;

    // AreaConfiguration creation
    virtual CAreaConfiguration* createAreaConfiguration(const CSyncerSet* pSyncerSet) const;

    // Parameter access
    virtual bool accessValue(CPathNavigator& pathNavigator, std::string& strValue, bool bSet, CParameterAccessContext& parameterAccessContext) const;

    /**
     * Get the list of all the ancestors that have a mapping.
     *
     * The mapping is represented as a std::string of all the mapping data (key:value) defined in the
     * context of the element.
     * In this class, the method is generic and calls its parent getListOfElementsWithMappings(...)
     * method.
     *
     * @param[in:out] configurableElementPath List of all the ConfigurableElements found
     * that have a mapping. Elements are added at the end of the list, so the root Element will be
     * the last one.
     *
     */
    virtual void getListOfElementsWithMapping(std::list<const CConfigurableElement*>&
                                               configurableElementPath) const;

    // Used for simulation and virtual subsystems
    virtual void setDefaultValues(CParameterAccessContext& parameterAccessContext) const;

    // Element properties
    virtual void showProperties(std::string& strResult) const;

    // XML configuration settings parsing
    virtual bool serializeXmlSettings(CXmlElement& xmlConfigurationSettingsElementContent, CConfigurationAccessContext& configurationAccessContext) const;
protected:
    // Syncer (me or ascendant)
    virtual ISyncer* getSyncer() const;
    // Syncer set (descendant)
    virtual void fillSyncerSetFromDescendant(CSyncerSet& syncerSet) const;
    // Configuration Domain local search
    bool containsConfigurableDomain(const CConfigurableDomain* pConfigurableDomain) const;

private:
    // Configurable domain association
    void addAttachedConfigurableDomain(const CConfigurableDomain* pConfigurableDomain);
    void removeAttachedConfigurableDomain(const CConfigurableDomain* pConfigurableDomain);

    // Belonging domain ascending search
    bool belongsToDomainAscending(const CConfigurableDomain* pConfigurableDomain) const;

    // Belonging domains
    void getBelongingDomains(std::list<const CConfigurableDomain*>& configurableDomainList) const;
    void listDomains(const std::list<const CConfigurableDomain*>& configurableDomainList, std::string& strResult, bool bVertical) const;

    // Check parent is still of current type (by structure knowledge)
    bool isOfConfigurableElementType(const CElement* pParent) const;

    // Offset in main blackboard
    uint32_t _uiOffset;

    // Associated configurable domains
    std::list<const CConfigurableDomain*> _configurableDomainList;
};

