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
#include "CompoundRule.h"
#include "RuleParser.h"

#define base CRule

using std::string;

// Types
const char* CCompoundRule::_apcTypes[2] = {
    "Any",
    "All"
};

CCompoundRule::CCompoundRule() : _bTypeAll(false)
{
}

// Class kind
string CCompoundRule::getKind() const
{
    return "CompoundRule";
}

// Returns true if children dynamic creation is to be dealt with
bool CCompoundRule::childrenAreDynamic() const
{
    return true;
}

// Content dumping
void CCompoundRule::logValue(string& strValue, CErrorContext& errorContext) const
{
    (void)errorContext;

    // Type
    strValue = _apcTypes[_bTypeAll];
}

// Parse
bool CCompoundRule::parse(CRuleParser& ruleParser, string& strError)
{
    // Get rule type
    uint32_t uiType;

    for (uiType = 0;  uiType < 2; uiType++) {

        if (ruleParser.getType() == _apcTypes[uiType]) {

            // Set type
            _bTypeAll = uiType != 0;

            return true;
        }
    }

    // Failed
    strError = "Unknown compound rule type: ";
    strError += ruleParser.getType();

    return false;
}

// Dump
void CCompoundRule::dump(string& strResult) const
{
    strResult += _apcTypes[_bTypeAll];
    strResult += "{";

    // Children
    size_t uiChild;
    size_t uiNbChildren = getNbChildren();
    bool bFirst = true;

    for (uiChild = 0; uiChild < uiNbChildren; uiChild++) {

        if (!bFirst) {

            strResult += ", ";
        }

        // Dump inner rule
        const CRule* pRule = static_cast<const CRule*>(getChild(uiChild));

        pRule->dump(strResult);

        bFirst = false;
    }

    strResult += "}";
}

// Rule check
bool CCompoundRule::matches() const
{
    size_t uiChild;
    size_t uiNbChildren = getNbChildren();

    for (uiChild = 0; uiChild < uiNbChildren; uiChild++) {

        const CRule* pRule = static_cast<const CRule*>(getChild(uiChild));

        if (pRule->matches() ^ _bTypeAll) {

            return !_bTypeAll;
        }
    }
    return _bTypeAll;
}

// From IXmlSink
bool CCompoundRule::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    // Get type
    _bTypeAll = xmlElement.getAttributeBoolean("Type", _apcTypes[true]);

    // Base
    return base::fromXml(xmlElement, serializingContext);
}

// From IXmlSource
void CCompoundRule::toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const
{
    // Set type
    xmlElement.setAttributeString("Type", _apcTypes[_bTypeAll]);

    // Base
    base::toXml(xmlElement, serializingContext);
}
