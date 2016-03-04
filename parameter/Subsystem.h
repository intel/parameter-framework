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

#include "ConfigurableElement.h"
#include "Mapper.h"
#include "MappingContext.h"
#include <log/Logger.h>

#include <list>
#include <stack>
#include <string>
#include <vector>

class CInstanceDefinition;
class CComponentLibrary;
class CSubsystemObject;
class CSubsystemObjectCreator;
class CInstanceConfigurableElement;
class CMappingData;

class PARAMETER_EXPORT CSubsystem : public CConfigurableElement, private IMapper
{
    // Subsystem objects iterator
    typedef std::list<CSubsystemObject *>::const_iterator SubsystemObjectListIterator;

public:
    /**
     * Class Constructor
     *
     * @param[in] strName subsystem name
     * @param[in] logger the main logger of the application
     */
    CSubsystem(const std::string &strName, core::log::Logger &logger);
    ~CSubsystem() override;

    bool structureFromXml(const CXmlElement &xmlElement,
                          CXmlSerializingContext &serializingContext) override;

    // Susbsystem sanity
    virtual bool isAlive() const;

    // Resynchronization after subsystem restart needed
    virtual bool needResync(bool bClear);

    // from CElement
    std::string getKind() const override;

    bool getMappingData(const std::string &strKey, const std::string *&pStrValue) const override;
    std::string getFormattedMapping() const override;

    /**
     * Fetch mapping data of an element.
     *
     * The mapping is represented as a std::string of all the mapping data (key:value) defined in
     * the
     * context of the element.
     * This method gathers the mapping data found in each Element of the configurableElementPath
     * list to format the resulting std::string.
     *
     * @param[in] configurableElementPath List of all the ConfigurableElements found
     * that have a mapping. Elements are added at the end of the list, so the root Element will be
     * the last one.
     *
     * @return Formatted std::string of the mapping data
     */
    virtual std::string getMapping(
        std::list<const CConfigurableElement *> &configurableElementPath) const;

protected:
    // Used for simulation and virtual subsystems
    void setDefaultValues(CParameterAccessContext &parameterAccessContext) const override;

    /// Functionality intendedn for derived Subsystems
    // Subsystem context mapping keys publication
    void addContextMappingKey(const std::string &strMappingKey);
    // Subsystem object creator publication (strong reference)
    void addSubsystemObjectFactory(CSubsystemObjectCreator *pSubsystemObjectCreator);

private:
    CSubsystem(const CSubsystem &);
    CSubsystem &operator=(const CSubsystem &);

    // Belonging subsystem
    const CSubsystem *getBelongingSubsystem() const override;

    // Mapping execution
    bool mapSubsystemElements(std::string &strError);

    /**
     * Handle a configurable element mapping.
     *
     * Add context mappings to the context and instantiate a subsystem object if needed.
     *
     * @param[in,out] pInstanceConfigurableElement The configurable element
     * @param[out] bKeepDiving Keep diving for mapping keys
                   Is set to true if a subsystem object (tree leave) has been instantiated.
                   Undetermined on error
     * @param[out] strError String filled with an human readable error on error,
                   left unmodified otherwise
     *
     * @return true on success, false on failure
     */
    bool mapBegin(CInstanceConfigurableElement *pInstanceConfigurableElement, bool &bKeepDiving,
                  std::string &strError) override;
    void mapEnd() override;

    // Mapping access
    /**
     * Generic mapping error handling
     *
     * Format an human readable error std::string from a key and a message in case of mapping error
     *
     * @param[in] strKey The key on which the error refers
     * @param[in] strMessage The error message
     * @param[in] pConfigurableElement The element on which the error refers
     *
     * returns The formated error std::string
     */
    std::string getMappingError(const std::string &strKey, const std::string &strMessage,
                                const CConfigurableElement *pConfigurableElement) const;

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
    std::string formatMappingDataList(
        const std::list<const CConfigurableElement *> &configurableElementPath) const;

    /**
     * Find the SubystemObject which contains a specific CInstanceConfigurableElement.
     *
     * @param[in] pInstanceConfigurableElement The CInstanceConfigurableElement that is related to
     * the wanted SubsystemObject. Each SubsystemObject of the Subystem internal list is checked in
     * order to find a match.
     *
     * @return A pointer to the SubsystemObject related to pInstanceConfigurableElement
     */
    const CSubsystemObject *findSubsystemObjectFromConfigurableElement(
        const CInstanceConfigurableElement *pInstanceConfigurableElement) const;

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
        const CInstanceConfigurableElement *pInstanceConfigurableElement,
        std::string &strMappingKey, std::string &strMappingValue) const;

    /**
     * Formats the mapping of a SubsystemObject
     *
     * @param[in] pInstanceConfigurableElement Element corresponding to a SubsystemObject
     *
     * @return String containing the formatted mapping
     */
    std::string getFormattedSubsystemMappingData(
        const CInstanceConfigurableElement *pInstanceConfigurableElement) const;
    /**
     * Generic context handling
     *
     * Feed context with mapping data of the current element
     *
     * @param[in] pConfigurableElement The element containing mapping data
     * @param[out] context The context mapping to update with the current element mapping values
     * @param[out] strError The formated error std::string
     *
     * @return true on success
     */
    bool handleMappingContext(const CConfigurableElement *pConfigurableElement,
                              CMappingContext &context, std::string &strError) const;

    /**
     * Looks if a subsystem object needs to be instantiated for the given configurable
     * element, then instantiate it if needed.
     *
     * @param[in,out] pInstanceConfigurableElement The configurable element to check
     *            for instanciation
     * @param[in] context The mapping values container
     * @param[out] bHasCreatedSubsystemObject If a subsystem object has been instantiated.
                   Undetermined on error
     * @param[out] strError String filled with an human readable error on error,
                   left unmodified otherwise
     *
     * @return true on success, false on failure
     */
    bool handleSubsystemObjectCreation(CInstanceConfigurableElement *pInstanceConfigurableElement,
                                       CMappingContext &context, bool &bHasCreatedSubsystemObject,
                                       std::string &strError);

    // Subsystem context mapping keys
    std::vector<std::string> _contextMappingKeyArray;

    // Subsystem object creator map
    std::vector<CSubsystemObjectCreator *> _subsystemObjectCreatorArray;

    // Subsystem sync objects (house keeping)
    std::list<CSubsystemObject *> _subsystemObjectList;

    // Mapping Context stack
    std::stack<CMappingContext> _contextStack;

    // Subelements
    CComponentLibrary *_pComponentLibrary;
    CInstanceDefinition *_pInstanceDefinition;

    //! Contains the mapping info at Subsystem level
    CMappingData *_pMappingData{nullptr};

    /** Logger which has to be provided to subsystem objects */
    core::log::Logger &_logger;
};
