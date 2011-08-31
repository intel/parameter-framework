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
#include "TypeElement.h"

class IMapper;
class CParameterBlackboard;
class CParameterAccessContext;

class CInstanceConfigurableElement : public CConfigurableElement
{
public:
    enum Type {
        EBitParameter,
        EBitParameterBlock,
        EParameter,
        EParameterBlock,
        EComponent
    };

    CInstanceConfigurableElement(const string& strName, const CTypeElement* pTypeElement);

    // Instantiated type
    const CTypeElement* getTypeElement() const;

    // Mapping info
    bool getMappingData(const string& strKey, const string*& pStrValue) const;

    // From CElement
    virtual string getKind() const;

    // Syncer to/from HW
    void setSyncer(ISyncer* pSyncer);
    void unsetSyncer();

    // Type
    virtual Type getType() const = 0;

    // Mapping execution
    bool map(IMapper& mapper, string& strError);
protected:
    // Syncer
    virtual ISyncer* getSyncer() const;
    // Syncer set (descendant)
    virtual void fillSyncerSetFromDescendant(CSyncerSet& syncerSet) const;
    // Sync
    bool sync(CParameterAccessContext& parameterAccessContext) const;
    // Check parameter access path well formed for leaf elements
    static bool checkPathExhausted(CPathNavigator& pathNavigator, CErrorContext& errorContext);
private:
    // Type Element
    const CTypeElement* _pTypeElement;

    // Sync to HW
    ISyncer* _pSyncer;
};

