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

#include "Rule.h"

class CCompoundRule : public CRule
{
public:
    CCompoundRule();

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
    // Returns true if children dynamic creation is to be dealt with
    virtual bool childrenAreDynamic() const;

    // Type
    bool _bTypeAll;

    // Types
    static const char* _apcTypes[];
};
