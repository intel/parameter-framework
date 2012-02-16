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
#include "RuleParser.h"
#include "CompoundRule.h"
#include "SelectionCriterionRule.h"
#include <assert.h>

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
    string::size_type iDelimiter;

    assert(_uiCurrentPos <= _strApplicationRule.length());

    // Consume spaces
    if ((iDelimiter = _strApplicationRule.find_first_not_of(" ", _uiCurrentPos)) != string::npos) {

        // New pos
        _uiCurrentPos = iDelimiter;
    }

    // Parse
    if ((_uiCurrentPos != _strApplicationRule.length()) && ((iDelimiter = _strApplicationRule.find_first_of(_acDelimiters[_eStatus], _uiCurrentPos)) != string::npos)) {

        switch(_strApplicationRule[iDelimiter]) {

        case '{':
            _eStatus = EBeginCompoundRule;
            // Extract type
            _strRuleType = _strApplicationRule.substr(_uiCurrentPos, iDelimiter - _uiCurrentPos);
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
            _strRuleType = _strApplicationRule.substr(_uiCurrentPos, iDelimiter - _uiCurrentPos);
            break;
        case ',':
            _eStatus = EContinue;
            break;
        }
        // New pos
        _uiCurrentPos = iDelimiter + 1;
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
    string::size_type iDelimiter;

    // Consume spaces
    if ((iDelimiter = _strApplicationRule.find_first_not_of(" ", _uiCurrentPos)) != string::npos) {

        // New pos
        _uiCurrentPos = iDelimiter;
    }

    if ((iDelimiter = _strApplicationRule.find_first_of("{} ,", _uiCurrentPos)) == string::npos) {

        strError = "Syntax error";

        return false;
    }

    strNext = _strApplicationRule.substr(_uiCurrentPos, iDelimiter - _uiCurrentPos);

    // New pos
    _uiCurrentPos = iDelimiter;

    return true;
}
