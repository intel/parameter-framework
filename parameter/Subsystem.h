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

#include "ConfigurableElement.h"
#include "Mapper.h"
#include "MappingContext.h"
#include <stack>
#include <vector>

class CInstanceDefinition;
class CComponentLibrary;
class CSubsystemObject;
class CSubsystemObjectCreator;
class CInstanceConfigurableElement;

class CSubsystem : public CConfigurableElement, private IMapper
{
    // Subsystem objects iterator
    typedef list<CSubsystemObject*>::const_iterator SubsystemObjectListIterator;
public:
    CSubsystem(const string& strName);
    virtual ~CSubsystem();

    // From IXmlSink
    virtual bool fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext);

    // Susbsystem Endianness
    bool isBigEndian() const;

    // Susbsystem sanity
    virtual bool isAlive() const;

    // Resynchronization after subsystem restart needed
    virtual bool needResync(bool bClear);

    // XML configuration settings parsing
    virtual bool serializeXmlSettings(CXmlElement& xmlConfigurationSettingsElementContent, CConfigurationAccessContext& configurationAccessContext) const;

    // from CElement
    virtual string getKind() const;

    /**
     * Fetch mapping data of an element.
     *
     * The mapping is represented as a string of all the mapping data (key:value) defined in the
     * context of the element.
     * This method gathers the mapping data found in each Element of the configurableElementPath
     * list to format the resulting string.
     *
     * @param[in] configurableElementPath List of all the ConfigurableElements found
     * that have a mapping. Elements are added at the end of the list, so the root Element will be
     * the last one.
     *
     * @return Formatted string of the mapping data
     */
    virtual string getMapping(list<const CConfigurableElement*>& configurableElementPath) const;

protected:
    // Parameter access
    virtual bool accessValue(CPathNavigator& pathNavigator, string& strValue, bool bSet, CParameterAccessContext& parameterAccessContext) const;
    virtual void logValue(string& strValue, CErrorContext& errorContext) const;
    // Used for simulation and virtual subsystems
    virtual void setDefaultValues(CParameterAccessContext& parameterAccessContext) const;

    /// Functionality intendedn for derived Subsystems
    // Subsystem context mapping keys publication
    void addContextMappingKey(const string& strMappingKey);
    // Subsystem object creator publication (strong reference)
    void addSubsystemObjectFactory(CSubsystemObjectCreator* pSubsystemObjectCreator);
private:
    CSubsystem(const CSubsystem&);
    CSubsystem& operator=(const CSubsystem&);

    // Belonging subsystem
    virtual const CSubsystem* getBelongingSubsystem() const;

    // Mapping execution
    bool mapSubsystemElements(string& strError);

    /**
     * Handle a configurable element mapping.
     *
     * Add context mappings to the context and instantiate a subsystem object if needed.
     *
     * @param[in:out] pInstanceConfigurableElement The configurable element
     * @param[out] bKeepDiving Keep diving for mapping keys
                   Is set to true if a subsystem object (tree leave) has been instantiated.
                   Undetermined on error
     * @param[out] strError String filled with an human readable error on error,
                   left unmodified otherwise
     *
     * @return true on success, false on failure
     */
    virtual bool mapBegin(CInstanceConfigurableElement* pInstanceConfigurableElement, bool& bKeepDiving, string& strError);
    virtual void mapEnd();

    // Mapping access
    /**
     * Generic mapping error handling
     *
     * Format an human readable error string from a key and a message in case of mapping error
     *
     * @param[in] strKey The key on which the error refers
     * @param[in] strMessage The error message
     * @param[in] pInstanceConfigurableElement The element on wich the error refers
     *
     * returns The formated error string
     */
    string getMappingError(const string& strKey,
                           const string& strMessage,
                           const CInstanceConfigurableElement* pInstanceConfigurableElement) const;

    /**
     * Format the mapping data of the ConfigurableElements that have been gathered through recursive
     * calls to the getMapping() method.
     * These elements shall be evaluated from the root level to the leaves level, so the list must
     * be parsed in reverse order.
     *
     * @param[in] configurableElementPath List of ConfigurableElements containing mapping data
     *
     * @return String containing the formatted mapping
     */
    string formatMappingDataList(
            const list<const CConfigurableElement*>& configurableElementPath) const;

    /**
     * Find the SubystemObject which contains a specific CInstanceConfigurableElement.
     *
     * @param[in] pInstanceConfigurableElement The CInstanceConfigurableElement that is related to
     * the wanted SubsystemObject. Each SubsystemObject of the Subystem internal list is checked in
     * order to find a match.
     *
     * @return A pointer to the SubsystemObject related to pInstanceConfigurableElement
     */
    const CSubsystemObject* findSubsystemObjectFromConfigurableElement(
            const CInstanceConfigurableElement* pInstanceConfigurableElement) const;

    /**
     * Find the mapping data defined for the CInstanceConfigurableElement given in parameter, that
     * corresponds to Subsystem level mapping (Subsystem level mapping keys are defined in
     * CSubsystemObjectCreator classes).
     * The CInstanceConfigurableElement might as well contain local mapping data.
     *
     * @param[in] pInstanceConfigurableElement The element which mapping data will be parsed for
     * a match
     * @param[out] strMappingKey Mapping key defined at the Subsystem level
     * @param[out] strMappingValue Mapping value contained in pInstanceConfigurableElement
     */
    void findSubsystemLevelMappingKeyValue(
            const CInstanceConfigurableElement* pInstanceConfigurableElement,
            string& strMappingKey,
            string& strMappingValue) const;

    /**
     * Formats the mapping of a SubsystemObject
     *
     * @param[in] pInstanceConfigurableElement Element corresponding to a SubsystemObject
     *
     * @return String containing the formatted mapping
     */
    string getFormattedSubsystemMappingData(
            const CInstanceConfigurableElement* pInstanceConfigurableElement) const;
    /**
     * Generic context handling
     *
     * Feed context with mapping data of the current element
     *
     * @param[in] pInstanceConfigurableElement The element containing mapping data
     * @param[out] context The context mapping to update with the current element mapping values
     * @param[out] strError The formated error string
     *
     * @return true on success
     */
    bool handleMappingContext(const CInstanceConfigurableElement* pInstanceConfigurableElement,
                              CMappingContext& context,
                              string& strError) const;

    /**
     * Looks if a subsystem object needs to be instantiated for the given configurable
     * element, then instantiate it if needed.
     *
     * @param[in:out] pInstanceConfigurableElement The configurable element to check
     *            for instanciation
     * @param[in] context The mapping values container
     * @param[out] bHasCreatedSubsystemObject If a subsystem object has been instantiated.
                   Undetermined on error
     * @param[out] strError String filled with an human readable error on error,
                   left unmodified otherwise
     *
     * @return true on success, false on failure
     */
    bool handleSubsystemObjectCreation(CInstanceConfigurableElement* pInstanceConfigurableElement,
                                       CMappingContext& context, bool& bHasCreatedSubsystemObject,
                                       string& strError);

    // Subsystem context mapping keys
    vector<string> _contextMappingKeyArray;

    // Subsystem object creator map
    vector<CSubsystemObjectCreator*> _subsystemObjectCreatorArray;

    // Subsystem sync objects (house keeping)
    list<CSubsystemObject*> _subsystemObjectList;

    // Mapping Context stack
    stack<CMappingContext> _contextStack;

    // Subelements
    CComponentLibrary* _pComponentLibrary;
    CInstanceDefinition* _pInstanceDefinition;

    // Endianness
    bool _bBigEndian;
};
