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

#include "Element.h"
#include "SelectionCriterionType.h"
#include "SelectionCriterionInterface.h"

#include <list>

using namespace std;

class CSelectionCriterion : public CElement, public ISelectionCriterionInterface
{
public:
    CSelectionCriterion(const string& strName, const CSelectionCriterionType* pType);

    /// From ISelectionCriterionInterface
    // State
    virtual void setCriterionState(int iState);
    virtual int getCriterionState() const;
    // Name
    virtual string getCriterionName() const;
    // Type
    virtual const ISelectionCriterionTypeInterface* getCriterionType() const;
    // Modified status
    bool hasBeenModified() const;
    void resetModifiedStatus();

    /// Match methods
    bool is(int iState) const;
    bool isNot(int iState) const;
    bool includes(int iState) const;
    bool excludes(int iState) const;

    /// User request
    string getFormattedDescription(bool bWithTypeInfo, bool bHumanReadable) const;

    /// From CElement
    virtual string getKind() const;
private:
    // Current state
    int _iState;
    // Type
    const CSelectionCriterionType* _pType;
    // Counter to know how many modifications have been applied to this criterion
    uint8_t _uiNbModifications;
};

