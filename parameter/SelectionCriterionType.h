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

#include "Element.h"
#include <map>
#include <string>
#include "SelectionCriterionTypeInterface.h"

class CSelectionCriterionType : public CElement, public ISelectionCriterionTypeInterface
{
    typedef std::map<std::string, int>::const_iterator NumToLitMapConstIt;

public:
    CSelectionCriterionType(bool bIsInclusive);

    // From ISelectionCriterionTypeInterface
    virtual bool addValuePair(int iValue, const std::string& strValue);
    /**
     * Retrieve the numerical value from the std::string representation of the criterion type.
     *
     * @param[in] strValue: criterion type value represented as a stream. If the criterion is
     *                      inclusive, it supports more than one criterion type value delimited
     *                      by the "|" symbol.
     * @param[out] iValue: criterion type value represented as an integer.
     *
     * @return true if integer value retrieved from the std::string one, false otherwise.
     */
    virtual bool getNumericalValue(const std::string& strValue, int& iValue) const;
    virtual bool getLiteralValue(int iValue, std::string& strValue) const;
    virtual bool isTypeInclusive() const;

    // Value list
    std::string listPossibleValues() const;

    // Formatted state
    virtual std::string getFormattedState(int iValue) const;

    /**
      * Export to XML
      *
      * @param[in] xmlElement The XML element to export to
      * @param[in] serializingContext The serializing context
      *
      */
    virtual void toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const;

    // From CElement
    virtual std::string getKind() const;
private:
    /**
     * Retrieve the numerical value from the std::string representation of the criterion type.
     *
     * @param[in] strValue: criterion type value represented as a stream. If the criterion is
     *                      inclusive, it expects only one criterion type value.
     * @param[out] iValue: criterion type value represented as an integer.
     *
     * @return true if integer value retrieved from the std::string one, false otherwise.
     */
    bool getAtomicNumericalValue(const std::string& strValue, int& iValue) const;
    bool _bInclusive;
    std::map<std::string, int> _numToLitMap;

    static const std::string _strDelimiter; /**< Inclusive criterion type delimiter. */
};

