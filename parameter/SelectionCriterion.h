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
#pragma once

#include "XmlSource.h"
#include "SelectionCriterionInterface.h"
#include <log/Logger.h>

#include <map>
#include <string>

/** Criterion object used to apply rules based on system state */
class CSelectionCriterion : public IXmlSource, public ISelectionCriterionInterface
{
public:
    CSelectionCriterion(const std::string& name, core::log::Logger& logger);

    /// From ISelectionCriterionInterface
    // State
    virtual void setCriterionState(int iState);
    virtual int getCriterionState() const;
    // Name
    virtual std::string getCriterionName() const;
    // Modified status
    bool hasBeenModified() const;
    void resetModifiedStatus();

    /// Match methods
    bool is(int iState) const;
    bool isNot(int iState) const;
    bool includes(int iState) const;
    bool excludes(int iState) const;

    /// User request
    std::string getFormattedDescription(bool bWithTypeInfo, bool bHumanReadable) const;

    //@{
    /** @see ISelectionCriterionInterface */
    virtual bool isInclusive() const override;

    virtual bool addValuePair(int numericalValue,
                              const std::string& literalValue,
                              std::string& error) override;

    bool getLiteralValue(int numericalValue, std::string& literalValue) const override final;

    virtual bool getNumericalValue(const std::string& literalValue,
                                   int& numericalValue) const override;

    virtual std::string getFormattedState() const override;
    //@}

    /** List different values a criterion can have
     *
     * @return formatted string containing criterion possible values
     */
    std::string listPossibleValues() const;

    /**
      * Export to XML
      *
      * @param[in] xmlElement The XML element to export to
      * @param[in] serializingContext The serializing context
      *
      */
    virtual void toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const;

protected:

    /** Set a "default formatted state" when no criterion state is set
     *
     * @param formattedState, the formatted state string to check
     * @result the reference of the string passed in parameter filled with the default value
     * if it was empty
     *
     * This method returns a reference on his referenced parameter in order to have the easy
     * notation.
     *    return checkFormattedStateEmptyness(myFormattedStateToReturn);
     */
    std::string& checkFormattedStateEmptyness(std::string& formattedState) const;

    /** Contains pair association between literal and numerical value */
    std::map<std::string, int> mValuePairs;

    /** Current state
     *
     * FIXME: Use bit set object instead
     */
    int32_t mState;

private:
    /** Counter to know how many modifications have been applied to this criterion */
    uint32_t _uiNbModifications;

    /** Application logger */
    core::log::Logger& _logger;

    /** Criterion name */
    const std::string mName;
};

