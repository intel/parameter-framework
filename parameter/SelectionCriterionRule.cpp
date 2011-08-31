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
#include <assert.h>

#define base CRule

const char* CSelectionCriterionRule::_apcMatchesWhen[CSelectionCriterionRule::ENbMatchesWhen] = {
    "Is",
    "Contains"
};

CSelectionCriterionRule::CSelectionCriterionRule() : _pSelectionCriterion(NULL), _eMatchesWhen(CSelectionCriterionRule::EIs), _iMatchValue(0)
{
}

// Class kind
string CSelectionCriterionRule::getKind() const
{
    return "SelectionCriterionRule";
}

// Rule check
bool CSelectionCriterionRule::matches() const
{
    assert(_pSelectionCriterion);

    switch(_eMatchesWhen) {
    case EIs:
        return _pSelectionCriterion->equals(_iMatchValue);
    case EContains:
        return _pSelectionCriterion->contains(_iMatchValue);
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

    if (!setMatchesWhen(strMatchesWhen)) {

        xmlDomainSerializingContext.setError("Wrong MatchesWhen attribute " + strMatchesWhen + " in " + getKind() + " " + xmlElement.getPath());

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
    xmlElement.setAttributeString("MatchesWhen", _apcMatchesWhen[_eMatchesWhen]);

    // Set Value
    string strValue;

     _pSelectionCriterion->getCriterionType()->getLiteralValue(_iMatchValue, strValue);

    xmlElement.setAttributeString("Value", strValue);
}

// XML MatchesWhen attribute parsing
bool CSelectionCriterionRule::setMatchesWhen(const string& strMatchesWhen)
{
    uint32_t uiMatchesWhen;

    for (uiMatchesWhen = 0; uiMatchesWhen < ENbMatchesWhen; uiMatchesWhen++) {

        if (strMatchesWhen == _apcMatchesWhen[uiMatchesWhen]) {

            // Found it!
            _eMatchesWhen = (MatchesWhen)uiMatchesWhen;

            return true;
        }
    }
    return false;
}
