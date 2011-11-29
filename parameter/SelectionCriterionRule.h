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
#pragma once

#include "Rule.h"

class CSelectionCriterion;

class CSelectionCriterionRule : public CRule
{
    // Matching rules
    enum MatchesWhen {
        EIs,
        EIsNot,
        EIncludes,
        EExcludes,

        ENbMatchesWhen
    };
    // Matching rule description
    struct SMatchingRuleDescription
    {
        const char* pcMatchesWhen;
        bool bExclusiveTypeCompatible;
    };

public:
    CSelectionCriterionRule();

    // Parse
    virtual bool parse(CRuleParser& ruleParser, string& strError);

    // Dump
    virtual void dump(string& strResult) const;

    // Rule check
    virtual bool matches() const;

    // From IXmlSink
    virtual bool fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext);

    // From IXmlSource
    virtual void toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const;

    // Class kind
    virtual string getKind() const;
protected:
    // Content dumping
    virtual void logValue(string& strValue, CErrorContext& errorContext) const;
private:
    // XML MatchesWhen attribute parsing
    bool setMatchesWhen(const string& strMatchesWhen, string& strError);

    // Selection criterion
    const CSelectionCriterion* _pSelectionCriterion;

    // MatchesWhen
    MatchesWhen _eMatchesWhen;

    // Value
    int32_t _iMatchValue;

    // Used for XML MatchesWhen attribute parsing
    static const SMatchingRuleDescription _astMatchesWhen[ENbMatchesWhen];
};

