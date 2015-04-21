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

#include "client/CriterionInterface.h"
#include "XmlSource.h"
#include <log/Logger.h>

#include <map>
#include <string>
#include <functional>
#include <stdexcept>

namespace core
{
namespace criterion
{

/** Criterion object used to apply rules based on system state */
class Criterion : public IXmlSource, public CriterionInterface
{
public:
    /** Indicates an error at the Criterion creation */
    using InvalidCriterionError = std::runtime_error;

    /** @param[in] name the criterion name
     *  @param[in] values available values the criterion can take
     *  @param[in] logger the main application logger
     *
     *  @throw InvalidCriterionError if there is less than 2 values provided.
     */
    Criterion(const std::string& name, const Values& values, core::log::Logger& logger);

    // @{
    /** @see CriterionInterface */
    virtual bool setState(const State& state, std::string& error) override;

    virtual State getState() const override final;

    virtual std::string getCriterionName() const override final;

    virtual bool isInclusive() const override;

    bool getLiteralValue(int numericalValue, std::string& literalValue) const override final;

    virtual bool getNumericalValue(const std::string& literalValue,
                                   int& numericalValue) const override final;

    virtual std::string getFormattedState() const override;
    // @}

    bool hasBeenModified() const;
    void resetModifiedStatus();

    /** Request criterion state match with a desired method
     *
     * @param[in] method, the desired match method
     * @param[in] state, the state to match
     * @return true if the current state match the state given in parameter with the desired method
     *
     * @throw std::out_of_range if the desired match method does not exist
     */
    bool match(const std::string& method, const State& state) const;

    /** Check if a match method is available for this criterion
     *
     * @param[in] method, the desired match method
     * @return true if the method is available, false otherwise
     */
    bool isMatchMethodAvailable(const std::string& method) const;

    std::string getFormattedDescription(bool bWithTypeInfo, bool bHumanReadable) const;

    /** List different values a criterion can have
     *
     * @return formatted string containing criterion possible values
     */
    std::string listPossibleValues() const;

    /** Export to XML
     *
     * @param[in] xmlElement The XML element to export to
     * @param[in] serializingContext The serializing context
     */
    virtual void toXml(CXmlElement& xmlElement, CXmlSerializingContext& context) const override;

protected:
    /** Criterion Match callback type
     *
     * Those method should take an integer in parameter which represents the state to match
     * and returns a boolean which indicates if the current state match the state given in
     * parameter.
     */
    typedef std::function<bool (const State&)> MatchMethod;

    /** Match method container, MatchMethod are indexed by their name */
    typedef std::map<std::string, MatchMethod> MatchMethods;

    /** Initializer constructor
     * This Constructor initialize class members and should be called by derived class
     * in order to add functionalities
     *
     * @param[in] name the criterion name
     * @param[in] logger the main application logger
     * @param[in] derivedValuePairs initial value pairs of derived classes
     * @param[in] derivedMatchMethods match methods of derived classes
     */
    Criterion(const std::string& name,
              core::log::Logger& logger,
              const Values& derivedValuePairs,
              const MatchMethods& derivedMatchMethods);

    /** Contains pair association between literal and numerical value */
    const Values mValues;

    /** Available criterion match methods */
    const MatchMethods mMatchMethods;

    /** Current state */
    State mState;

    /** Register a modification of the criterion
     * Client need to be informed when the criterion has been modified
     * and how many times.
     * This method helps to count and log modifications.
     */
    void stateModificationsEvent();

private:
    /** Counter to know how many modifications have been applied to this criterion */
    uint32_t _uiNbModifications;

    /** Application logger */
    core::log::Logger& _logger;

    /** Criterion name */
    const std::string mName;
};

} /** criterion namespace */
} /** core namespace */
