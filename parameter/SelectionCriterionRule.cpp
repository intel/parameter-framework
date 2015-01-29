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
#include "SelectionCriterionRule.h"
#include "SelectionCriterion.h"
#include "XmlDomainSerializingContext.h"
#include "XmlDomainImportContext.h"
#include "SelectionCriteriaDefinition.h"
#include "SelectionCriterionTypeInterface.h"
#include "RuleParser.h"
#include <assert.h>

#define base CRule

using std::string;

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

        strError = "Value error: \"" + strValue + "\" is not part of criterion \"" +
                   _pSelectionCriterion->getCriterionName() + "\"";

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
    CXmlDomainImportContext& xmlDomainImportContext = static_cast<CXmlDomainImportContext&>(serializingContext);

    // Get selection criterion
    string strSelectionCriterion = xmlElement.getAttributeString("SelectionCriterion");

    _pSelectionCriterion = xmlDomainImportContext.getSelectionCriteriaDefinition()->getSelectionCriterion(strSelectionCriterion);

    // Check existence
    if (!_pSelectionCriterion) {

        xmlDomainImportContext.setError("Couldn't find selection criterion " + strSelectionCriterion + " in " + getKind() + " " + xmlElement.getPath());

        return false;
    }

    // Get MatchesWhen
    string strMatchesWhen = xmlElement.getAttributeString("MatchesWhen");
    string strError;

    if (!setMatchesWhen(strMatchesWhen, strError)) {

        xmlDomainImportContext.setError("Wrong MatchesWhen attribute " + strMatchesWhen + " in " + getKind() + " " + xmlElement.getPath() + ": " + strError);

        return false;
    }

    // Get Value
    string strValue = xmlElement.getAttributeString("Value");

    if (!_pSelectionCriterion->getCriterionType()->getNumericalValue(strValue, _iMatchValue)) {

        xmlDomainImportContext.setError("Wrong Value attribute value " + strValue + " in " + getKind() + " " + xmlElement.getPath());

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
