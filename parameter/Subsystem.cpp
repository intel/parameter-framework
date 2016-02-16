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
#include "Subsystem.h"
#include "ComponentLibrary.h"
#include "InstanceDefinition.h"
#include "XmlParameterSerializingContext.h"
#include "ParameterAccessContext.h"
#include "ConfigurationAccessContext.h"
#include "SubsystemObjectCreator.h"
#include "MappingData.h"
#include <assert.h>
#include <sstream>

#define base CConfigurableElement

using std::string;
using std::list;

CSubsystem::CSubsystem(const string &strName, core::log::Logger &logger)
    : base(strName), _pComponentLibrary(new CComponentLibrary),
      _pInstanceDefinition(new CInstanceDefinition), _logger(logger)
{
    // Note: A subsystem contains instance components
    // InstanceDefintion and ComponentLibrary objects are then not chosen to be children
    // They'll be delt with locally
}

CSubsystem::~CSubsystem()
{
    // FIXME use unique_ptr, would make this method empty

    for (auto *subsystemObject : _subsystemObjectList) {

        delete subsystemObject;
    }

    // Remove susbsystem creators
    for (auto *subsystemObjectCreator : _subsystemObjectCreatorArray) {

        delete subsystemObjectCreator;
    }

    // Order matters!
    delete _pInstanceDefinition;
    delete _pComponentLibrary;

    delete _pMappingData;
}

string CSubsystem::getKind() const
{
    return "Subsystem";
}

// Susbsystem sanity
bool CSubsystem::isAlive() const
{
    return true;
}

// Resynchronization after subsystem restart needed
bool CSubsystem::needResync(bool /*bClear*/)
{
    return false;
}

bool CSubsystem::structureFromXml(const CXmlElement &xmlElement,
                                  CXmlSerializingContext &serializingContext)
{
    // Subsystem class does not rely on generic fromXml algorithm of Element class.
    // So, setting here the description if found as XML attribute.
    string description;
    xmlElement.getAttribute(gDescriptionPropertyName, description);
    setDescription(description);

    // Context
    CXmlParameterSerializingContext &parameterBuildContext =
        static_cast<CXmlParameterSerializingContext &>(serializingContext);

    // Install temporary component library for further component creation
    parameterBuildContext.setComponentLibrary(_pComponentLibrary);

    CXmlElement childElement;

    // Manage mapping attribute
    string rawMapping;
    xmlElement.getAttribute("Mapping", rawMapping);
    if (!rawMapping.empty()) {

        std::string error;
        _pMappingData = new CMappingData;
        if (!_pMappingData->init(rawMapping, error)) {

            serializingContext.setError("Invalid Mapping data from XML element '" +
                                        xmlElement.getPath() + "': " + error);
            return false;
        }
    }

    // XML populate ComponentLibrary
    xmlElement.getChildElement("ComponentLibrary", childElement);

    if (!_pComponentLibrary->fromXml(childElement, serializingContext)) {

        return false;
    }

    // XML populate InstanceDefintion
    xmlElement.getChildElement("InstanceDefintion", childElement);
    if (!_pInstanceDefinition->fromXml(childElement, serializingContext)) {

        return false;
    }

    // Create components
    _pInstanceDefinition->createInstances(this);

    // Execute mapping to create subsystem mapping entities
    string strError;
    if (!mapSubsystemElements(strError)) {

        serializingContext.setError(strError);

        return false;
    }

    return true;
}

bool CSubsystem::mapSubsystemElements(string &strError)
{
    // Default mapping context
    CMappingContext context(_contextMappingKeyArray.size());
    // Add Subsystem-level mapping data, which will be propagated to all children
    handleMappingContext(this, context, strError);

    _contextStack.push(context);

    // Map all instantiated subelements in subsystem
    size_t nbChildren = getNbChildren();

    for (size_t child = 0; child < nbChildren; child++) {

        CInstanceConfigurableElement *pInstanceConfigurableChildElement =
            static_cast<CInstanceConfigurableElement *>(getChild(child));

        if (!pInstanceConfigurableChildElement->map(*this, strError)) {

            return false;
        }
    }
    return true;
}

// Formats the mapping of the ConfigurableElements
string CSubsystem::formatMappingDataList(
    const list<const CConfigurableElement *> &configurableElementPath) const
{
    // The list is parsed in reverse order because it has been filled from the leaf to the trunk
    // of the tree. When formatting the mapping, we want to start from the subsystem level
    std::list<string> mappings;
    list<const CConfigurableElement *>::const_reverse_iterator it;
    for (it = configurableElementPath.rbegin(); it != configurableElementPath.rend(); ++it) {

        auto maybeMapping = (*it)->getFormattedMapping();
        if (not maybeMapping.empty()) {
            mappings.push_back(maybeMapping);
        }
    }

    return utility::asString(mappings, ", ");
}

// Find the CSubystemObject containing a specific CInstanceConfigurableElement
const CSubsystemObject *CSubsystem::findSubsystemObjectFromConfigurableElement(
    const CInstanceConfigurableElement *pInstanceConfigurableElement) const
{

    list<CSubsystemObject *>::const_iterator it;
    for (it = _subsystemObjectList.begin(); it != _subsystemObjectList.end(); ++it) {

        // Check if one of the SubsystemObjects is associated with a ConfigurableElement
        // corresponding to the expected one
        const CSubsystemObject *pSubsystemObject = *it;

        if (pSubsystemObject->getConfigurableElement() == pInstanceConfigurableElement) {

            return pSubsystemObject;
        }
    }

    return nullptr;
}

void CSubsystem::findSubsystemLevelMappingKeyValue(
    const CInstanceConfigurableElement *pInstanceConfigurableElement, string &strMappingKey,
    string &strMappingValue) const
{
    // Find creator to get key name
    std::vector<CSubsystemObjectCreator *>::const_iterator it;
    for (it = _subsystemObjectCreatorArray.begin(); it != _subsystemObjectCreatorArray.end();
         ++it) {

        const CSubsystemObjectCreator *pSubsystemObjectCreator = *it;

        strMappingKey = pSubsystemObjectCreator->getMappingKey();

        // Check if the ObjectCreator MappingKey corresponds to the element mapping data
        const string *pStrValue;
        if (pInstanceConfigurableElement->getMappingData(strMappingKey, pStrValue)) {

            strMappingValue = *pStrValue;
            return;
        }
    }
    assert(0);
}

// Formats the mapping data as a comma separated list of key value pairs
string CSubsystem::getFormattedSubsystemMappingData(
    const CInstanceConfigurableElement *pInstanceConfigurableElement) const
{
    // Find the SubsystemObject related to pInstanceConfigurableElement
    const CSubsystemObject *pSubsystemObject =
        findSubsystemObjectFromConfigurableElement(pInstanceConfigurableElement);

    // Exit if node does not correspond to a SubsystemObject
    if (pSubsystemObject == NULL) {

        return "";
    }

    // Find SubsystemCreator mapping key
    string strMappingKey;
    string strMappingValue; // mapping value where amends are not replaced by their value
    findSubsystemLevelMappingKeyValue(pInstanceConfigurableElement, strMappingKey, strMappingValue);

    // Find SubSystemObject mapping value (with amends replaced by their value)
    return strMappingKey + ":" + pSubsystemObject->getFormattedMappingValue();
}

string CSubsystem::getMapping(list<const CConfigurableElement *> &configurableElementPath) const
{
    if (configurableElementPath.empty()) {

        return "";
    }

    // Get the first element, which is the element containing the amended mapping
    const CInstanceConfigurableElement *pInstanceConfigurableElement =
        static_cast<const CInstanceConfigurableElement *>(configurableElementPath.front());

    // Format context mapping data
    string strValue = formatMappingDataList(configurableElementPath);

    // Print the mapping of the first node, which corresponds to a SubsystemObject
    auto subsystemObjectAmendedMapping =
        getFormattedSubsystemMappingData(pInstanceConfigurableElement);
    if (not subsystemObjectAmendedMapping.empty()) {
        strValue += ", " + subsystemObjectAmendedMapping;
    }

    return strValue;
}

// Used for simulation and virtual subsystems
void CSubsystem::setDefaultValues(CParameterAccessContext &parameterAccessContext) const
{
    base::setDefaultValues(parameterAccessContext);
}

// Belonging subsystem
const CSubsystem *CSubsystem::getBelongingSubsystem() const
{
    return this;
}

// Subsystem context mapping keys publication
void CSubsystem::addContextMappingKey(const string &strMappingKey)
{
    _contextMappingKeyArray.push_back(strMappingKey);
}

// Subsystem object creator publication (strong reference)
void CSubsystem::addSubsystemObjectFactory(CSubsystemObjectCreator *pSubsystemObjectCreator)
{
    _subsystemObjectCreatorArray.push_back(pSubsystemObjectCreator);
}

// Generic error handling from derived subsystem classes
string CSubsystem::getMappingError(const string &strKey, const string &strMessage,
                                   const CConfigurableElement *pConfigurableElement) const
{
    return getName() + " " + getKind() + " " + "mapping:\n" + strKey + " " + "error: \"" +
           strMessage + "\" " + "for element " + pConfigurableElement->getPath();
}

bool CSubsystem::getMappingData(const std::string &strKey, const std::string *&pStrValue) const
{
    if (_pMappingData) {

        return _pMappingData->getValue(strKey, pStrValue);
    }
    return false;
}

// Returns the formatted mapping
std::string CSubsystem::getFormattedMapping() const
{
    if (!_pMappingData) {
        return "";
    }
    return _pMappingData->asString();
}

// Mapping generic context handling
bool CSubsystem::handleMappingContext(const CConfigurableElement *pConfigurableElement,
                                      CMappingContext &context, string &strError) const
{
    // Feed context with found mapping data
    for (size_t item = 0; item < _contextMappingKeyArray.size(); item++) {

        const string &strKey = _contextMappingKeyArray[item];
        const string *pStrValue;

        if (pConfigurableElement->getMappingData(strKey, pStrValue)) {
            // Assign item to context
            if (!context.setItem(item, &strKey, pStrValue)) {

                strError = getMappingError(strKey, "Already set", pConfigurableElement);

                return false;
            }
        }
    }
    return true;
}

// Subsystem object creation handling
bool CSubsystem::handleSubsystemObjectCreation(
    CInstanceConfigurableElement *pInstanceConfigurableElement, CMappingContext &context,
    bool &bHasCreatedSubsystemObject, string &strError)
{
    bHasCreatedSubsystemObject = false;

    for (const auto *pSubsystemObjectCreator : _subsystemObjectCreatorArray) {

        // Mapping key
        string strKey = pSubsystemObjectCreator->getMappingKey();
        // Object id
        const string *pStrValue;

        if (pInstanceConfigurableElement->getMappingData(strKey, pStrValue)) {

            // First check context consistency
            // (required ancestors must have been set prior to object creation)
            uint32_t uiAncestorMask = pSubsystemObjectCreator->getAncestorMask();

            for (size_t uiAncestorKey = 0; uiAncestorKey < _contextMappingKeyArray.size();
                 uiAncestorKey++) {

                if (!((1 << uiAncestorKey) & uiAncestorMask)) {
                    // Ancestor not required
                    continue;
                }
                // Check ancestor was provided
                if (!context.iSet(uiAncestorKey)) {

                    strError =
                        getMappingError(strKey, _contextMappingKeyArray[uiAncestorKey] + " not set",
                                        pInstanceConfigurableElement);

                    return false;
                }
            }

            // Then check configurable element size is correct
            if (pInstanceConfigurableElement->getFootPrint() >
                pSubsystemObjectCreator->getMaxConfigurableElementSize()) {

                string strSizeError =
                    "Size should not exceed " +
                    std::to_string(pSubsystemObjectCreator->getMaxConfigurableElementSize());

                strError = getMappingError(strKey, strSizeError, pInstanceConfigurableElement);

                return false;
            }

            // Do create object and keep its track
            _subsystemObjectList.push_back(pSubsystemObjectCreator->objectCreate(
                *pStrValue, pInstanceConfigurableElement, context, _logger));

            // Indicate subsytem creation to caller
            bHasCreatedSubsystemObject = true;

            // The subsystem Object has been instantiated, no need to continue looking for an
            // instantiation mapping
            break;
        }
    }

    return true;
}

// From IMapper
// Handle a configurable element mapping
bool CSubsystem::mapBegin(CInstanceConfigurableElement *pInstanceConfigurableElement,
                          bool &bKeepDiving, string &strError)
{
    // Get current context
    CMappingContext context = _contextStack.top();

    // Add mapping in context
    if (!handleMappingContext(pInstanceConfigurableElement, context, strError)) {

        return false;
    }

    // Push context
    _contextStack.push(context);

    // Assume diving by default
    bKeepDiving = true;

    // Deal with ambiguous usage of parameter blocks
    bool bShouldCreateSubsystemObject = true;

    switch (pInstanceConfigurableElement->getType()) {

    case CInstanceConfigurableElement::EComponent:
    case CInstanceConfigurableElement::EParameterBlock:
        // Subsystem object creation is optional in parameter blocks
        bShouldCreateSubsystemObject = false;
    // No break
    case CInstanceConfigurableElement::EBitParameterBlock:
    case CInstanceConfigurableElement::EParameter:
    case CInstanceConfigurableElement::EStringParameter:

        bool bHasCreatedSubsystemObject;

        if (!handleSubsystemObjectCreation(pInstanceConfigurableElement, context,
                                           bHasCreatedSubsystemObject, strError)) {

            return false;
        }
        // Check for creation error
        if (bShouldCreateSubsystemObject && !bHasCreatedSubsystemObject) {

            strError = getMappingError("Not found", "Subsystem object mapping key is missing",
                                       pInstanceConfigurableElement);
            return false;
        }
        // Not created and no error, keep diving
        bKeepDiving = !bHasCreatedSubsystemObject;

        return true;

    default:
        assert(0);
        return false;
    }
}

void CSubsystem::mapEnd()
{
    // Unstack context
    _contextStack.pop();
}
