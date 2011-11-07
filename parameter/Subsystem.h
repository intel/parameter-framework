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

#include "ConfigurableElement.h"
#include "Mapper.h"
#include "MappingContext.h"
#include <stack>
#include <vector>

class CInstanceDefinition;
class CComponentLibrary;
class CSubsystemObject;
class CSubsystemObjectCreator;

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

    // XML configuration settings parsing
    virtual bool serializeXmlSettings(CXmlElement& xmlConfigurationSettingsElementContent, CConfigurationAccessContext& configurationAccessContext) const;

    // from CElement
    virtual string getKind() const;
protected:
    // Parameter access
    virtual bool setValue(CPathNavigator& pathNavigator, const string& strValue, CParameterAccessContext& parameterContext) const;
    virtual bool getValue(CPathNavigator& pathNavigator, string& strValue, CParameterAccessContext& parameterContext) const;
    virtual void logValue(string& strValue, CErrorContext& errorContext) const;
    // Used for simulation only
    virtual void setDefaultValues(CParameterAccessContext& parameterAccessContext) const;

    /// Functionality intendedn for derived Subsystems
    // Subsystem context mapping keys publication
    void addContextMappingKey(const string& strMappingKey);
    // Subsystem object creator publication (strong reference)
    void addSubsystemObjectFactory(CSubsystemObjectCreator* pSubsystemObjectCreator);
private:
    // Belonging subsystem
    virtual const CSubsystem* getBelongingSubsystem() const;

    // Mapping execution
    bool mapSubsystemElements(string& strError);

    // Generic subsystem mapping error handling
    void getMappingError(string& strError, const string& strKey, const string& strMessage, const CInstanceConfigurableElement* pInstanceConfigurableElement);

    // From IMapper
    virtual bool mapBegin(CInstanceConfigurableElement* pInstanceConfigurableElement, bool& bKeepDiving, string& strError);
    virtual void mapEnd();

    // Mapping generic context handling
    bool handleMappingContext(const CInstanceConfigurableElement* pInstanceConfigurableElement, CMappingContext& context, string& strError);
    // Subsystem object creation handling
    bool handleSubsystemObjectCreation(CInstanceConfigurableElement* pInstanceConfigurableElement, CMappingContext& context, string& strError);

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
