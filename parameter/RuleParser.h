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

#include <string>
#include <stdint.h>

using namespace std;

class CCompoundRule;
class CSelectionCriteriaDefinition;

class CRuleParser
{
public:
    enum Status {
        EInit,
        EBeginCompoundRule,
        EEndCompoundRule,
        ECriterionRule,
        EContinue,
        EDone,

        ENbStatuses
    };

    CRuleParser(const string& strApplicationRule, const CSelectionCriteriaDefinition* pSelectionCriteriaDefinition);
    ~CRuleParser();

    // Parse
    bool parse(CCompoundRule* pParentRule, string& strError);

    // Iterate
    bool iterate(string& strError);

    // Next word
    bool next(string& strNext, string& strError);

    // Rule type
    const string& getType() const;

    // Criteria defintion
    const CSelectionCriteriaDefinition* getSelectionCriteriaDefinition() const;

    // Root rule
    CCompoundRule* grabRootRule();
private:

    CRuleParser(const CRuleParser&);
    CRuleParser& operator=(const CRuleParser&);

    // Rule definition
    string _strApplicationRule;
    // Criteria defintion
    const CSelectionCriteriaDefinition* _pSelectionCriteriaDefinition;
    // Iterator
    uint32_t _uiCurrentPos;
    // Deepness
    uint32_t _uiCurrentDeepness;
    // Current Type
    string _strRuleType;
    // Status
    Status _eStatus;
    // Root rule
    CCompoundRule* _pRootRule;
    // Matches
    static const char* _acDelimiters[ENbStatuses];
};

