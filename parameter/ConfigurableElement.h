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
    PARAMETER_EXPORT
    CConfigurableElement(const std::string& strName = "");
    PARAMETER_EXPORT
    virtual ~CConfigurableElement();

    // Offset in main blackboard
    PARAMETER_EXPORT
    void setOffset(size_t offset);
    PARAMETER_EXPORT
    size_t getOffset() const;

    // Allocation
    PARAMETER_EXPORT
    virtual size_t getFootPrint() const;

    // Syncer set (me, ascendant or descendant ones)
    PARAMETER_EXPORT
    void fillSyncerSet(CSyncerSet& syncerSet) const;

    // Belonging domain
    PARAMETER_EXPORT
    bool belongsTo(const CConfigurableDomain* pConfigurableDomain) const;

    // Belonging domains
    PARAMETER_EXPORT
    void listBelongingDomains(std::string& strResult, bool bVertical = true) const;

    // Matching check for domain association
    PARAMETER_EXPORT
    bool hasNoDomainAssociated() const;

    // Matching check for no valid associated domains
    PARAMETER_EXPORT
    bool hasNoValidDomainAssociated() const;

    // Owning domains
    PARAMETER_EXPORT
    void listAssociatedDomains(std::string& strResult, bool bVertical = true) const;
    PARAMETER_EXPORT
    size_t getBelongingDomainCount() const;

    // Elements with no domains
    PARAMETER_EXPORT
    void listRogueElements(std::string& strResult) const;

    // Belonging to no domains
    PARAMETER_EXPORT
    bool isRogue() const;

    // Footprint as string
    PARAMETER_EXPORT
    std::string getFootprintAsString() const;

    // Belonging subsystem
    PARAMETER_EXPORT
    virtual const CSubsystem* getBelongingSubsystem() const;

    // Check element is a parameter
    PARAMETER_EXPORT
    virtual bool isParameter() const;

    // AreaConfiguration creation
    PARAMETER_EXPORT
    virtual CAreaConfiguration* createAreaConfiguration(const CSyncerSet* pSyncerSet) const;

    // Parameter access
    PARAMETER_EXPORT
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
    PARAMETER_EXPORT
    virtual void getListOfElementsWithMapping(std::list<const CConfigurableElement*>&
                                               configurableElementPath) const;

    // Used for simulation and virtual subsystems
    PARAMETER_EXPORT
    virtual void setDefaultValues(CParameterAccessContext& parameterAccessContext) const;

    // Element properties
    PARAMETER_EXPORT
    virtual void showProperties(std::string& strResult) const;

    // XML configuration settings parsing
    PARAMETER_EXPORT
    virtual bool serializeXmlSettings(CXmlElement& xmlConfigurationSettingsElementContent, CConfigurationAccessContext& configurationAccessContext) const;
protected:
    // Syncer (me or ascendant)
    PARAMETER_EXPORT
    virtual ISyncer* getSyncer() const;
    // Syncer set (descendant)
    PARAMETER_EXPORT
    virtual void fillSyncerSetFromDescendant(CSyncerSet& syncerSet) const;
    // Configuration Domain local search
    PARAMETER_EXPORT
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
    size_t _offset;

    // Associated configurable domains
    std::list<const CConfigurableDomain*> _configurableDomainList;
};

