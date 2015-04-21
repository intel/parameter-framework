/*
 * Copyright (c) 2011-2015, Intel Corporation
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
#include "XmlDomainSerializingContext.h"
#include "XmlDomainImportContext.h"
#include "RuleParser.h"
#include <criterion/Criterion.h>

#include <assert.h>

#define base CRule

using std::string;

CSelectionCriterionRule::CSelectionCriterionRule() :
    _pSelectionCriterion(NULL), mMatchesWhenVerb(""), mMatchState{0}
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
    _pSelectionCriterion = ruleParser.getCriteria().getSelectionCriterion(ruleParser.getType());

    // Check existence
    if (!_pSelectionCriterion) {

        strError = "Couldn't find selection criterion " + ruleParser.getType();

        return false;
    }

    // Verb
    if (!ruleParser.next(mMatchesWhenVerb, strError)) {

        return false;
    }
    // Value
    string strValue;

    if (!ruleParser.next(strValue, strError)) {

        return false;
    }

    // Matches when
    if (!_pSelectionCriterion->isMatchMethodAvailable(mMatchesWhenVerb)) {

        strError = "Matche type: " + mMatchesWhenVerb + " incompatible with " +
                   (_pSelectionCriterion->isInclusive() ? "inclusive" : "exclusive") +
                   " criterion: " + _pSelectionCriterion->getCriterionName();

        return false;
    }

    // Value
    int numericalValue;
    if (!_pSelectionCriterion->getNumericalValue(strValue, numericalValue)) {

        strError = "Value error: \"" + strValue + "\" is not part of criterion \"" +
                   _pSelectionCriterion->getCriterionName() + "\"";

        return false;
    }
    mMatchState = {numericalValue};

    return true;
}

// Dump
void CSelectionCriterionRule::dump(string& strResult) const
{
    // Criterion
    strResult += _pSelectionCriterion->getCriterionName();
    strResult += " ";
    // Verb
    strResult += mMatchesWhenVerb;
    strResult += " ";
    // Value
    string strValue;
    assert(!mMatchState.empty());
    _pSelectionCriterion->getLiteralValue(*mMatchState.begin(), strValue);
    strResult += strValue;
}

// Rule check
bool CSelectionCriterionRule::matches() const
{
    assert(_pSelectionCriterion);

    return _pSelectionCriterion->match(mMatchesWhenVerb, mMatchState);
}

// From IXmlSink
bool CSelectionCriterionRule::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    // Retrieve actual context
    CXmlDomainImportContext& xmlDomainImportContext = static_cast<CXmlDomainImportContext&>(serializingContext);

    // Get selection criterion
    string strSelectionCriterion = xmlElement.getAttributeString("SelectionCriterion");

    _pSelectionCriterion =
        xmlDomainImportContext.getCriteria().getSelectionCriterion(strSelectionCriterion);

    // Check existence
    if (!_pSelectionCriterion) {

        xmlDomainImportContext.setError("Couldn't find selection criterion " + strSelectionCriterion + " in " + getKind() + " " + xmlElement.getPath());

        return false;
    }

    // Get MatchesWhen
    mMatchesWhenVerb = xmlElement.getAttributeString("MatchesWhen");
    string strError;

    if (!_pSelectionCriterion->isMatchMethodAvailable(mMatchesWhenVerb)) {

        xmlDomainImportContext.setError("Wrong MatchesWhen attribute " + mMatchesWhenVerb + " in " +
                                        getKind() + " " + xmlElement.getPath() + ": " +
                                        _pSelectionCriterion->getCriterionName());

        return false;
    }

    // Get Value
    string strValue = xmlElement.getAttributeString("Value");

    int numericalValue;
    if (!_pSelectionCriterion->getNumericalValue(strValue, numericalValue)) {

        xmlDomainImportContext.setError("Wrong Value attribute value " + strValue + " in " + getKind() + " " + xmlElement.getPath());

        return false;
    }
    mMatchState = {numericalValue};

    // Done
    return true;
}

// From IXmlSource
void CSelectionCriterionRule::toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const
{
    (void)serializingContext;

    assert(_pSelectionCriterion);

    // Set selection criterion
    xmlElement.setAttributeString("SelectionCriterion", _pSelectionCriterion->getCriterionName());

    // Set MatchesWhen
    xmlElement.setAttributeString("MatchesWhen", mMatchesWhenVerb);

    // Set Value
    string strValue;

    assert(!mMatchState.empty());
    _pSelectionCriterion->getLiteralValue(*mMatchState.begin(), strValue);

    xmlElement.setAttributeString("Value", strValue);
}
