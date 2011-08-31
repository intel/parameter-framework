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

#include "BinarySerializableElement.h"
#include <list>

class CConfigurableElement;
class CAreaConfiguration;
class CParameterBlackboard;
class CConfigurationAccessContext;
class CCompoundRule;

class CDomainConfiguration : public CBinarySerializableElement
{
    enum ChildElementType {
        ECompoundRule
    };
    typedef list<CAreaConfiguration*>::const_iterator AreaConfigurationListIterator;
public:
    CDomainConfiguration(const string& strName);
    virtual ~CDomainConfiguration();

    // Configurable Elements association
    void addConfigurableElement(const CConfigurableElement* pConfigurableElement);
    void removeConfigurableElement(const CConfigurableElement* pConfigurableElement);

    // Save data from current
    void save(const CParameterBlackboard* pMainBlackboard);
    // Apply data to current
    void restore(CParameterBlackboard* pMainBlackboard) const;
    // Ensure validity for configurable element area configuration
    void validate(const CConfigurableElement* pConfigurableElement, const CParameterBlackboard* pMainBlackboard);
    // Ensure validity of all area configurations
    void validate(const CParameterBlackboard* pMainBlackboard);
    // Return configuration validity for given configurable element
    bool isValid(const CConfigurableElement* pConfigurableElement) const;
    // Ensure validity of configurable element's area configuration by copying in from a valid one
    void validateAgainst(const CDomainConfiguration* pValidDomainConfiguration, const CConfigurableElement* pConfigurableElement);
    // Ensure validity of all configurable element's area configuration by copying in from a valid ones
    void validateAgainst(const CDomainConfiguration* pValidDomainConfiguration);
    // Applicability checking
    bool isApplicable() const;
    // Merge existing configurations to given configurable element ones
    void merge(CConfigurableElement* pToConfigurableElement, CConfigurableElement* pFromConfigurableElement);
    // Domain splitting
    void split(CConfigurableElement* pFromConfigurableElement);

    // XML configuration settings parsing/composing
    bool serializeXmlSettings(const CConfigurableElement* pConfigurableElement, CXmlElement& xmlConfigurationSettingsElementContent, CConfigurationAccessContext& configurationAccessContext);

    // Presence of application condition
    bool hasRule() const;

    // Serialization
    virtual void binarySerialize(CBinaryStream& binaryStream);

    // Data size
    virtual uint32_t getDataSize() const;

    // Class kind
    virtual string getKind() const;
private:
    // Returns true if children dynamic creation is to be dealt with (here, will allow child deletion upon clean)
    virtual bool childrenAreDynamic() const;
    // AreaConfiguration retrieval from configurable element
    CAreaConfiguration* getAreaConfiguration(const CConfigurableElement* pConfigurableElement) const;

    // Rule
    const CCompoundRule* getRule() const;

    // AreaConfigurations
    list<CAreaConfiguration*> _areaConfigurationList;
};
