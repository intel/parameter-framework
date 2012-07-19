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

#include <stdint.h>

#include "BaseParameter.h"

class CBitParameter : public CBaseParameter
{
public:
    CBitParameter(const string& strName, const CTypeElement* pTypeElement);

    // Instantiation, allocation
    virtual uint32_t getFootPrint() const;

    // Type
    virtual Type getType() const;

    /// Value access
    // Boolean access
    virtual bool accessAsBoolean(bool& bValue, bool bSet, CParameterAccessContext& parameterAccessContext) const;

    // Integer Access
    virtual bool accessAsInteger(uint32_t& uiValue, bool bSet, CParameterAccessContext& parameterAccessContext) const;

    // AreaConfiguration creation
    virtual CAreaConfiguration* createAreaConfiguration(const CSyncerSet* pSyncerSet) const;

    // Size
    uint32_t getBelongingBlockSize() const;

    // Access from area configuration
    uint32_t merge(uint32_t uiOriginData, uint32_t uiNewData) const;
private:

    // String Access
    virtual bool doSetValue(const string& strValue, uint32_t uiOffset, CParameterAccessContext& parameterAccessContext) const;
    virtual void doGetValue(string& strValue, uint32_t uiOffset, CParameterAccessContext& parameterAccessContext) const;

    // Generic Access
    template <typename type>
    bool doSet(type value, uint32_t uiOffset, CParameterAccessContext& parameterAccessContext) const;
    template <typename type>
    void doGet(type& value, uint32_t uiOffset, CParameterAccessContext& parameterAccessContext) const;

};
