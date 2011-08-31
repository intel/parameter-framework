/* <auto_header>
 * <FILENAME>
 * 
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
 *  AUTHOR: Patrick Benavoli (patrickx.benavoli@intel.com)
 * CREATED: 2011-06-01
 * UPDATED: 2011-07-27
 * 
 * 
 * </auto_header>
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

class CConfigurableElement : public CElement
{
    friend class CConfigurableDomain;
    typedef list<const CConfigurableDomain*>::const_iterator ConfigurableDomainListConstIterator;
public:
    CConfigurableElement(const string& strName);
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
    void listBelongingDomains(string& strResult, bool bVertical = true) const;

    // Matching check for domain association
    bool hasNoDomainAssociated() const;

    // Matching check for no valid associated domains
    bool hasNoValidDomainAssociated() const;

    // Owning domains
    void listAssociatedDomains(string& strResult, bool bVertical = true) const;
    uint32_t getBelongingDomainCount() const;

    // Elements with no domains
    void listRogueElements(string& strResult) const;

    // Footprint as string
    string getFootprintAsString() const;

    // Parameter access
    virtual bool setValue(CPathNavigator& pathNavigator, const string& strValue, CParameterAccessContext& parameterContext) const;
    virtual bool getValue(CPathNavigator& pathNavigator, string& strValue, CParameterAccessContext& parameterContext) const;
    // Used for simulation only
    virtual void setDefaultValues(CParameterAccessContext& parameterAccessContext) const;

    // Element properties
    virtual void showProperties(string& strResult) const;

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
    void getBelongingDomains(list<const CConfigurableDomain*>& configurableDomainList) const;
    void listDomains(const list<const CConfigurableDomain*>& configurableDomainList, string& strResult, bool bVertical) const;

    // Belonging subsystem
    virtual const CSubsystem* getBelongingSubsystem() const;

    // Check parent is still of current type (by structure knowledge)
    bool isOfConfigurableElementType(const CElement* pParent) const;

    // Offset in main blackboard
    uint32_t _uiOffset;

    // Associated configurable domains
    list<const CConfigurableDomain*> _configurableDomainList;
};

