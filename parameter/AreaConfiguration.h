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
    bool restore(CParameterBlackboard* pMainBlackboard, bool bSync, string& strError) const;

    // Ensure validity
    void validate(const CParameterBlackboard* pMainBlackboard);

    // Return validity
    bool isValid() const;

    // Ensure validity against given valid area configuration
    void validateAgainst(const CAreaConfiguration* pValidAreaConfiguration);

    // Compound handling
    const CConfigurableElement* getConfigurableElement() const;

    // Configuration merging
    void copyFromInner(const CAreaConfiguration* pFromAreaConfiguration);

    // Configuration splitting
    void copyToInner(CAreaConfiguration* pToAreaConfiguration) const;

    // XML configuration settings parsing/composing
    bool serializeXmlSettings(CXmlElement& xmlConfigurableElementSettingsElementContent, CConfigurationAccessContext& configurationAccessContext);

    // Serialization
    void serialize(CBinaryStream& binaryStream);

    // Data size
    uint32_t getSize() const;
private:
    // Store validity
    void setValid(bool bValid);

    // Associated configurable element
    const CConfigurableElement* _pConfigurableElement;

    // Syncer set (required for immediate synchronization)
    const CSyncerSet* _pSyncerSet;

    // Configurable element settings
    CParameterBlackboard _blackboard;

    // Area configuration validity (invalid area configurations can't be restored)
    bool _bValid;
};

