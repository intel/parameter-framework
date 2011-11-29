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
#include "SelectionCriterionRule.h"
#include "SelectionCriterion.h"
#include "XmlDomainSerializingContext.h"
#include "SelectionCriteriaDefinition.h"
#include "SelectionCriterionTypeInterface.h"
#include "RuleParser.h"
#include <assert.h>

#define base CRule

const CSelectionCriterionRule::SMatchingRuleDescription CSelectionCriterionRule::_astMatchesWhen[CSelectionCriterionRule::ENbMatchesWhen] = {
    { "Is", true },
    { "IsNot", true },
    { "Includes", false },
    { "Excludes", false }
};

CSelectionCriterionRule::CSelectionCriterionRule() : _pSelectionCriterion(NULL), _eMatchesWhen(CSelectionCriterionRule::EIs), _iMatchValue(0)
{
}

// Class kind
string CSelectionCriterionRule::getKind() const
{
    return "SelectionCriterionRule";
}

// Content dumping
void CSelectionCriterionRule::logValue(string& strValue, CErrorContext& errorContext) const
{
    (void)errorContext;

    // Dump rule
    dump(strValue);
}

// Parse
bool CSelectionCriterionRule::parse(CRuleParser& ruleParser, string& strError)
{
    // Criterion
    _pSelectionCriterion = ruleParser.getSelectionCriteriaDefinition()->getSelectionCriterion(ruleParser.getType());

    // Check existence
    if (!_pSelectionCriterion) {

        strError = "Couldn't find selection criterion " + ruleParser.getType();

        return false;
    }

    // Verb
    string strMatchesWhen;

    if (!ruleParser.next(strMatchesWhen, strError)) {

        return false;
    }
    // Value
    string strValue;

    if (!ruleParser.next(strValue, strError)) {

        return false;
    }

    // Matches when
    if (!setMatchesWhen(strMatchesWhen, strError)) {

        strError = "Verb error: " + strError;

        return false;
    }

    // Value
    if (!_pSelectionCriterion->getCriterionType()->getNumericalValue(strValue, _iMatchValue)) {

        strError = "Value error: " + strError;

        return false;
    }

    return true;
}

// Dump
void CSelectionCriterionRule::dump(string& strResult) const
{
    // Criterion
    strResult += _pSelectionCriterion->getName();
    strResult += " ";
    // Verb
    strResult += _astMatchesWhen[_eMatchesWhen].pcMatchesWhen;
    strResult += " ";
    // Value
    string strValue;
    _pSelectionCriterion->getCriterionType()->getLiteralValue(_iMatchValue, strValue);
    strResult += strValue;
}

// Rule check
bool CSelectionCriterionRule::matches() const
{
    assert(_pSelectionCriterion);

    switch(_eMatchesWhen) {
    case EIs:
        return _pSelectionCriterion->is(_iMatchValue);
    case EIsNot:
        return _pSelectionCriterion->isNot(_iMatchValue);
    case EIncludes:
        return _pSelectionCriterion->includes(_iMatchValue);
    case EExcludes:
        return _pSelectionCriterion->excludes(_iMatchValue);
    default:
        assert(0);
        return false;
    }
}

// From IXmlSink
bool CSelectionCriterionRule::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    // Retrieve actual context
    CXmlDomainSerializingContext& xmlDomainSerializingContext = static_cast<CXmlDomainSerializingContext&>(serializingContext);

    // Get selection criterion
    string strSelectionCriterion = xmlElement.getAttributeString("SelectionCriterion");

    _pSelectionCriterion = xmlDomainSerializingContext.getSelectionCriteriaDefinition()->getSelectionCriterion(strSelectionCriterion);

    // Check existence
    if (!_pSelectionCriterion) {

        xmlDomainSerializingContext.setError("Couldn't find selection criterion " + strSelectionCriterion + " in " + getKind() + " " + xmlElement.getPath());

        return false;
    }

    // Get MatchesWhen
    string strMatchesWhen = xmlElement.getAttributeString("MatchesWhen");
    string strError;

    if (!setMatchesWhen(strMatchesWhen, strError)) {

        xmlDomainSerializingContext.setError("Wrong MatchesWhen attribute " + strMatchesWhen + " in " + getKind() + " " + xmlElement.getPath() + ": " + strError);

        return false;
    }

    // Get Value
    string strValue = xmlElement.getAttributeString("Value");

    if (!_pSelectionCriterion->getCriterionType()->getNumericalValue(strValue, _iMatchValue)) {

        xmlDomainSerializingContext.setError("Wrong Value attribute value " + strValue + " in " + getKind() + " " + xmlElement.getPath());

        return false;
    }

    // Done
    return true;
}

// From IXmlSource
void CSelectionCriterionRule::toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const
{
    (void)serializingContext;

    assert(_pSelectionCriterion);

    // Set selection criterion
    xmlElement.setAttributeString("SelectionCriterion", _pSelectionCriterion->getName());

    // Set MatchesWhen
    xmlElement.setAttributeString("MatchesWhen", _astMatchesWhen[_eMatchesWhen].pcMatchesWhen);

    // Set Value
    string strValue;

     _pSelectionCriterion->getCriterionType()->getLiteralValue(_iMatchValue, strValue);

    xmlElement.setAttributeString("Value", strValue);
}

// XML MatchesWhen attribute parsing
bool CSelectionCriterionRule::setMatchesWhen(const string& strMatchesWhen, string& strError)
{
    uint32_t uiMatchesWhen;

    for (uiMatchesWhen = 0; uiMatchesWhen < ENbMatchesWhen; uiMatchesWhen++) {

        const SMatchingRuleDescription* pstMatchingRuleDescription = &_astMatchesWhen[uiMatchesWhen];

        if (strMatchesWhen == pstMatchingRuleDescription->pcMatchesWhen) {

            // Found it!

            // Get Type
            const ISelectionCriterionTypeInterface* pSelectionCriterionType = _pSelectionCriterion->getCriterionType();

            // Check compatibility if relevant
            if (!pSelectionCriterionType->isTypeInclusive() && !pstMatchingRuleDescription->bExclusiveTypeCompatible) {

                strError = "Value incompatible with exclusive kind of type";

                return false;
            }

            // Store
            _eMatchesWhen = (MatchesWhen)uiMatchesWhen;

            return true;
        }
    }

    strError = "Value not found";

    return false;
}
