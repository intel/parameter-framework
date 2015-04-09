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
#include "RuleParser.h"
#include "CompoundRule.h"
#include "SelectionCriterionRule.h"
#include <assert.h>

using std::string;

// Matches
const char* CRuleParser::_acDelimiters[CRuleParser::ENbStatuses] = {
    "{",    // EInit
    "{} ",  // EBeginCompoundRule
    ",}",   // EEndCompoundRule
    ",}",   // ECriterionRule
    "{ ",   // EContinue
    ""      // EDone
};

CRuleParser::CRuleParser(const string& strApplicationRule, const CSelectionCriteriaDefinition* pSelectionCriteriaDefinition) :
    _strApplicationRule(strApplicationRule),
    _pSelectionCriteriaDefinition(pSelectionCriteriaDefinition),
    _uiCurrentPos(0),
    _uiCurrentDeepness(0),
    _eStatus(CRuleParser::EInit),
    _pRootRule(NULL)
{
}

CRuleParser::~CRuleParser()
{
    delete _pRootRule;
}

// Parse
bool CRuleParser::parse(CCompoundRule* pParentRule, string& strError)
{
    while (true) {
        // Iterate till next relevant delimiter
        if (!iterate(strError)) {

            return false;
        }
        switch(_eStatus) {
        case EBeginCompoundRule: {

            // Create new compound rule
            CCompoundRule* pCompoundRule = new CCompoundRule;

            // Parse
            if (!pCompoundRule->parse(*this, strError)) {

                delete pCompoundRule;

                return false;
            }
            // Parent rule creation context?
            if (pParentRule) {

                // Chain
                pParentRule->addChild(pCompoundRule);
            } else {
                // Root rule
                delete _pRootRule;
                _pRootRule = pCompoundRule;
            }
            // Parse
            if (!parse(pCompoundRule, strError)) {

                return false;
            }
            // Go on
            break;
        }
        case EEndCompoundRule:
            return true;
        case EContinue:
            // Seek for new rule
            break;
        case ECriterionRule: {
            // Create new criterion rule
            CSelectionCriterionRule* pCriterionRule = new CSelectionCriterionRule;

            // Parse
            if (!pCriterionRule->parse(*this, strError)) {

                delete pCriterionRule;

                return false;
            }

            // Chain
            pParentRule->addChild(pCriterionRule);

            // Go on
            break;
        }
        case EDone: {
            // If the current state is EDone, check that at least one rule has been found.
            if (_pRootRule) {

                // At least one rule found
                return true;
            } else {

                strError = "Syntax error, no rule found";

                return false;
            }

        }
        default:
            assert(0);
            return false;
        }
    }

    return true;
}

// Iterate
bool CRuleParser::iterate(string& strError)
{
    string::size_type delimiter;

    assert(_uiCurrentPos <= _strApplicationRule.length());

    // Consume spaces
    if ((delimiter = _strApplicationRule.find_first_not_of(" ", _uiCurrentPos)) != string::npos) {

        // New pos
        _uiCurrentPos = delimiter;
    }

    // Parse
    if ((_uiCurrentPos != _strApplicationRule.length()) && ((delimiter = _strApplicationRule.find_first_of(_acDelimiters[_eStatus], _uiCurrentPos)) != string::npos)) {

        switch(_strApplicationRule[delimiter]) {

        case '{':
            _eStatus = EBeginCompoundRule;
            // Extract type
            _strRuleType = _strApplicationRule.substr(_uiCurrentPos, delimiter - _uiCurrentPos);
            _uiCurrentDeepness++;
            break;
        case '}':
            _eStatus = EEndCompoundRule;

            if (!_uiCurrentDeepness--) {

                strError = "Missing opening brace";

                return false;
            }
            break;
        case ' ':
            _eStatus = ECriterionRule;
            // Extract type
            _strRuleType = _strApplicationRule.substr(_uiCurrentPos, delimiter - _uiCurrentPos);
            break;
        case ',':
            _eStatus = EContinue;
            break;
        }
        // New pos
        _uiCurrentPos = delimiter + 1;
    } else {

        if (_uiCurrentDeepness) {

            strError = "Missing closing brace";

            return false;
        }

        // Remaining characters
        if (_uiCurrentPos != _strApplicationRule.length()) {

            strError = "Syntax error";

            return false;
        }
        // Done
        _eStatus = EDone;
    }
    return true;
}

// Rule type
const string& CRuleParser::getType() const
{
    return _strRuleType;
}

// Criteria defintion
const CSelectionCriteriaDefinition* CRuleParser::getSelectionCriteriaDefinition() const
{
    return _pSelectionCriteriaDefinition;
}

// Root rule
CCompoundRule* CRuleParser::grabRootRule()
{
    CCompoundRule* pRootRule = _pRootRule;

    assert(pRootRule);

    _pRootRule = NULL;

    return pRootRule;
}

// Next word
bool CRuleParser::next(string& strNext, string& strError)
{
    string::size_type delimiter;

    // Consume spaces
    if ((delimiter = _strApplicationRule.find_first_not_of(" ", _uiCurrentPos)) != string::npos) {

        // New pos
        _uiCurrentPos = delimiter;
    }

    if ((delimiter = _strApplicationRule.find_first_of("{} ,", _uiCurrentPos)) == string::npos) {

        strError = "Syntax error";

        return false;
    }

    strNext = _strApplicationRule.substr(_uiCurrentPos, delimiter - _uiCurrentPos);

    // New pos
    _uiCurrentPos = delimiter;

    return true;
}
