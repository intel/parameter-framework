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
#include "Subsystem.h"
#include "ComponentLibrary.h"
#include "InstanceDefinition.h"
#include "XmlParameterSerializingContext.h"
#include "ParameterAccessContext.h"
#include "ConfigurationAccessContext.h"
#include "SubsystemObjectCreator.h"
#include <assert.h>

#define base CConfigurableElement

CSubsystem::CSubsystem(const string& strName) : base(strName), _pComponentLibrary(new CComponentLibrary), _pInstanceDefinition(new CInstanceDefinition), _bBigEndian(false)
{
    // Note: A subsystem contains instance components
    // InstanceDefintion and ComponentLibrary objects are then not chosen to be children
    // They'll be delt with locally
}

CSubsystem::~CSubsystem()
{
    // Remove subsystem objects
    SubsystemObjectListIterator subsystemObjectIt;

    for (subsystemObjectIt = _subsystemObjectList.begin(); subsystemObjectIt != _subsystemObjectList.end(); ++subsystemObjectIt) {

        delete *subsystemObjectIt;
    }

    // Remove susbsystem creators
    uint32_t uiIndex;

    for (uiIndex = 0; uiIndex < _subsystemObjectCreatorArray.size(); uiIndex++) {

        delete _subsystemObjectCreatorArray[uiIndex];
    }

    // Order matters!
    delete _pInstanceDefinition;
    delete _pComponentLibrary;
}

string CSubsystem::getKind() const
{
    return "Subsystem";
}

// Susbsystem Endianness
bool CSubsystem::isBigEndian() const
{
    return _bBigEndian;
}

// From IXmlSink
bool CSubsystem::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    // Context
    CXmlParameterSerializingContext& parameterBuildContext = static_cast<CXmlParameterSerializingContext&>(serializingContext);

    // Install temporary component library for further component creation
    parameterBuildContext.setComponentLibrary(_pComponentLibrary);

    CXmlElement childElement;

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

    // Endianness
    _bBigEndian = xmlElement.getAttributeBoolean("Endianness", "Big");

    return true;
}

// XML configuration settings parsing
bool CSubsystem::serializeXmlSettings(CXmlElement& xmlConfigurationSettingsElementContent, CConfigurationAccessContext& configurationAccessContext) const
{
    // Fix Endianness
    configurationAccessContext.setBigEndianSubsystem(_bBigEndian);

    return base::serializeXmlSettings(xmlConfigurationSettingsElementContent, configurationAccessContext);
}


bool CSubsystem::mapSubsystemElements(string& strError)
{
    // Default mapping context
    _contextStack.push(CMappingContext(_contextMappingKeyArray.size()));

    // Map all instantiated subelements in subsystem
    uint32_t uiNbChildren = getNbChildren();
    uint32_t uiChild;

    for (uiChild = 0; uiChild < uiNbChildren; uiChild++) {

        CInstanceConfigurableElement* pInstanceConfigurableChildElement = static_cast<CInstanceConfigurableElement*>(getChild(uiChild));

        if (!pInstanceConfigurableChildElement->map(*this, strError)) {

            return false;
        }
    }
    return true;
}

// Parameter access
bool CSubsystem::setValue(CPathNavigator& pathNavigator, const string& strValue, CErrorContext& errorContext) const
{
    CParameterAccessContext& parameterContext = static_cast<CParameterAccessContext&>(errorContext);

    // Deal with Endianness
    parameterContext.setBigEndianSubsystem(_bBigEndian);

    return base::setValue(pathNavigator, strValue, errorContext);
}

bool CSubsystem::getValue(CPathNavigator& pathNavigator, string& strValue, CErrorContext& errorContext) const
{
    CParameterAccessContext& parameterContext = static_cast<CParameterAccessContext&>(errorContext);

    // Deal with Endianness
    parameterContext.setBigEndianSubsystem(_bBigEndian);

    return base::getValue(pathNavigator, strValue, errorContext);
}

void CSubsystem::logValue(string& strValue, CErrorContext& errorContext) const
{
    CParameterAccessContext& parameterContext = static_cast<CParameterAccessContext&>(errorContext);

    // Deal with Endianness
    parameterContext.setBigEndianSubsystem(_bBigEndian);

    return base::logValue(strValue, errorContext);
}

// Used for simulation only
void CSubsystem::setDefaultValues(CParameterAccessContext& parameterAccessContext) const
{
    // Deal with Endianness
    parameterAccessContext.setBigEndianSubsystem(_bBigEndian);

    base::setDefaultValues(parameterAccessContext);
}

// Belonging subsystem
const CSubsystem* CSubsystem::getBelongingSubsystem() const
{
    return this;
}

// Subsystem context mapping keys publication
void CSubsystem::addContextMappingKey(const string& strMappingKey)
{
    _contextMappingKeyArray.push_back(strMappingKey);
}

// Subsystem object creator publication (strong reference)
void CSubsystem::addSubsystemObjectFactory(CSubsystemObjectCreator* pSubsystemObjectCreator)
{
    _subsystemObjectCreatorArray.push_back(pSubsystemObjectCreator);
}

// Mapping generic context handling
bool CSubsystem::handleMappingContext(const CInstanceConfigurableElement* pInstanceConfigurableElement, CMappingContext& context, string& strError)
{
    // Feed context with found mapping data
    uint32_t uiItem;

    for (uiItem = 0; uiItem < _contextMappingKeyArray.size(); uiItem++) {

        string strKey = _contextMappingKeyArray[uiItem];
        const string* pStrValue;

        if (pInstanceConfigurableElement->getMappingData(strKey, pStrValue)) {
            // Assign item to context
            if (!context.setItem(uiItem, pStrValue)) {

                getMappingError(strError, strKey, "Already set", pInstanceConfigurableElement);

                return false;
            }
        }
    }
    return true;
}

// Creation handling
bool CSubsystem::handleSubsystemObjectCreation(CInstanceConfigurableElement* pInstanceConfigurableElement, CMappingContext& context, string& strError)
{
    uint32_t uiItem;

    for (uiItem = 0; uiItem < _subsystemObjectCreatorArray.size(); uiItem++) {

        const CSubsystemObjectCreator* pSubsystemObjectCreator = _subsystemObjectCreatorArray[uiItem];

        // Mapping key
        string strKey = pSubsystemObjectCreator->getMappingKey();
        // Object id
        const string* pStrValue;

        if (pInstanceConfigurableElement->getMappingData(strKey, pStrValue)) {

            // First check context consistensy (required ancestors must have been set prior to object creation)
            uint32_t uiAncestorKey;
            uint32_t uiAncestorMask = pSubsystemObjectCreator->getAncestorMask();

            for (uiAncestorKey = 0; uiAncestorKey < _contextMappingKeyArray.size(); uiAncestorKey++) {

                if (!((1 << uiAncestorKey) & uiAncestorMask)) {
                    // Ancestor not required
                    continue;
                }
                // Check ancestor was provided
                if (!context.iSet(uiAncestorKey)) {

                    getMappingError(strError, strKey, _contextMappingKeyArray[uiAncestorKey] + " not set", pInstanceConfigurableElement);

                    return false;
                }
            }

            // Then check configurable element size is correct
            if (pInstanceConfigurableElement->getFootPrint() > pSubsystemObjectCreator->getMaxConfigurableElementSize()) {

                string strSizeError = "Size should not exceed " + pInstanceConfigurableElement->getFootprintAsString();

                getMappingError(strError, strKey, strSizeError, pInstanceConfigurableElement);

                return false;
            }

            // Do create object and keep its track
            _subsystemObjectList.push_back(pSubsystemObjectCreator->objectCreate(*pStrValue, pInstanceConfigurableElement, context));

            // Done
            return true;
        }
    }
    getMappingError(strError, "Mapping key", "Not found", pInstanceConfigurableElement);

    return false;
}

// Generic error handling from derived subsystem classes
void CSubsystem::getMappingError(string& strError, const string& strKey, const string& strMessage, const CInstanceConfigurableElement* pInstanceConfigurableElement)
{
    strError = getName() + " " + getKind() + " mapping:\n" + strKey + " error : \"" + strMessage + "\" for element " + pInstanceConfigurableElement->getPath();
}

// From IMapper
bool CSubsystem::mapBegin(CInstanceConfigurableElement* pInstanceConfigurableElement, string& strError)
{
    // Get current context
    CMappingContext context = _contextStack.top();

    switch(pInstanceConfigurableElement->getType()) {

    case CInstanceConfigurableElement::EComponent:

        if (!handleMappingContext(pInstanceConfigurableElement, context, strError)) {

            return false;
        }
        break;
    case CInstanceConfigurableElement::EParameterBlock:
    case CInstanceConfigurableElement::EBitParameterBlock:
    case CInstanceConfigurableElement::EParameter:
    {
        if (!handleSubsystemObjectCreation(pInstanceConfigurableElement, context, strError)) {

            return false;
        }
        break;
    }
    default:
        assert(0);
        return false;
    }

    // Push context
    _contextStack.push(context);

    return true;
}

void CSubsystem::mapEnd()
{
    // Unstack context
    _contextStack.pop();
}
