/*
 * Copyright (c) 2011-2014, Intel Corporation
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
#pragma once

#include <string>
#include <stdint.h>

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

    CRuleParser(const std::string& strApplicationRule, const CSelectionCriteriaDefinition* pSelectionCriteriaDefinition);
    ~CRuleParser();

    // Parse
    bool parse(CCompoundRule* pParentRule, std::string& strError);

    // Iterate
    bool iterate(std::string& strError);

    // Next word
    bool next(std::string& strNext, std::string& strError);

    // Rule type
    const std::string& getType() const;

    // Criteria defintion
    const CSelectionCriteriaDefinition* getSelectionCriteriaDefinition() const;

    // Root rule
    CCompoundRule* grabRootRule();
private:

    CRuleParser(const CRuleParser&);
    CRuleParser& operator=(const CRuleParser&);

    // Rule definition
    std::string _strApplicationRule;
    // Criteria defintion
    const CSelectionCriteriaDefinition* _pSelectionCriteriaDefinition;
    /** String iterator */
    std::string::size_type _uiCurrentPos;
    // Deepness
    uint32_t _uiCurrentDeepness;
    // Current Type
    std::string _strRuleType;
    // Status
    Status _eStatus;
    // Root rule
    CCompoundRule* _pRootRule;
    // Matches
    static const char* _acDelimiters[ENbStatuses];
};

