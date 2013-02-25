/* 
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
 * CREATED: 2011-06-01
 * UPDATED: 2011-07-27
 */
#pragma once

#include "ParameterBlackboard.h"
#include "BinaryStream.h"
#include "SyncerSet.h"

using namespace std;

class CConfigurableElement;
class CXmlElement;
class CConfigurationAccessContext;

class CAreaConfiguration
{
public:
    CAreaConfiguration(const CConfigurableElement* pConfigurableElement, const CSyncerSet* pSyncerSet);

    // Save data from current
    void save(const CParameterBlackboard* pMainBlackboard);

    // Apply data to current
    bool restore(CParameterBlackboard* pMainBlackboard, bool bSync, list<string>* plstrError) const;

    // Ensure validity
    void validate(const CParameterBlackboard* pMainBlackboard);

    // Return validity
    bool isValid() const;

    // Ensure validity against given valid area configuration
    void validateAgainst(const CAreaConfiguration* pValidAreaConfiguration);

    // Compound handling
    const CConfigurableElement* getConfigurableElement() const;

    // Configuration merging
    virtual void copyToOuter(CAreaConfiguration* pToAreaConfiguration) const;

    // Configuration splitting
    virtual void copyFromOuter(const CAreaConfiguration* pFromAreaConfiguration);

    // XML configuration settings parsing/composing
    bool serializeXmlSettings(CXmlElement& xmlConfigurableElementSettingsElementContent, CConfigurationAccessContext& configurationAccessContext);

    // Serialization
    void serialize(CBinaryStream& binaryStream);

    // Data size
    uint32_t getSize() const;

    // Fetch the Configuration Blackboard
    CParameterBlackboard& getBlackboard();
    const CParameterBlackboard& getBlackboard() const;

protected:
    CAreaConfiguration(const CConfigurableElement* pConfigurableElement, const CSyncerSet* pSyncerSet, uint32_t uiSize);

private:
    // Blackboard copies
    virtual void copyTo(CParameterBlackboard* pToBlackboard, uint32_t uiOffset) const;
    virtual void copyFrom(const CParameterBlackboard* pFromBlackboard, uint32_t uiOffset);

    // Store validity
    void setValid(bool bValid);

protected:
    // Associated configurable element
    const CConfigurableElement* _pConfigurableElement;

    // Configurable element settings
    CParameterBlackboard _blackboard;

private:
    // Syncer set (required for immediate synchronization)
    const CSyncerSet* _pSyncerSet;

    // Area configuration validity (invalid area configurations can't be restored)
    bool _bValid;
};

