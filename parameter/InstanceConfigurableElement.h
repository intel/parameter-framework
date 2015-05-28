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

#include "ConfigurableElementWithMapping.h"
#include "TypeElement.h"

#include <list>
#include <string>

class IMapper;
class CParameterBlackboard;
class CParameterAccessContext;

class CInstanceConfigurableElement : public CConfigurableElementWithMapping
{
public:
    enum Type {
        EBitParameter,
        EBitParameterBlock,
        EParameter,
        EStringParameter,
        EParameterBlock,
        EComponent
    };

    CInstanceConfigurableElement(const std::string& strName, const CTypeElement* pTypeElement);

    // Instantiated type
    const CTypeElement* getTypeElement() const;

    virtual bool getMappingData(const std::string& strKey, const std::string*& pStrValue) const;

    /**
     * Returns the mapping data associated to the type element of the current
     * InstanceConfigurableElement, as a formatted std::string
     *
     * @return A std::string containing the formatted mapping
     */
    std::string getFormattedMapping() const;

    // From CElement
    virtual std::string getKind() const;

    // Syncer to/from HW
    void setSyncer(ISyncer* pSyncer);
    void unsetSyncer();

    // Type
    virtual Type getType() const = 0;

    // Mapping execution
    bool map(IMapper& mapper, std::string& strError);

    // Element properties
    virtual void showProperties(std::string& strResult) const;

    // Scalar or Array?
    bool isScalar() const;

    // Array Length
    uint32_t getArrayLength() const;

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
     */
    virtual void getListOfElementsWithMapping(std::list<const CConfigurableElement*>&
                                               configurableElementPath) const;

    virtual void toXml(CXmlElement &xmlElement, CXmlSerializingContext &serializingContext) const;

protected:
    // Syncer
    virtual ISyncer* getSyncer() const;
    // Syncer set (descendant)
    virtual void fillSyncerSetFromDescendant(CSyncerSet& syncerSet) const;

    /**
     * Performs the sync if the AutoSync is enabled.
     * If AutoSync is disabled, any call to sync will returns true, even if synchronization has not
     * been done. It will happen when the AutoSync will be switched back on.
     *
     * @param[in:out] parameterAccessContext Parameter access context object
     *
     * @return true if the synchronization succeded or if the AutoSync is off, false otherwise.
     */
    bool sync(CParameterAccessContext& parameterAccessContext) const;

    // Check parameter access path well formed for leaf elements
    static bool checkPathExhausted(CPathNavigator& pathNavigator, CErrorContext& errorContext);
private:
    // Type Element
    const CTypeElement* _pTypeElement;

    // Sync to HW
    ISyncer* _pSyncer;
};

