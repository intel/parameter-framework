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

#include <string>
#include <list>
#include <set>

namespace core
{
namespace criterion
{
/** Type which represent a criterion state part */
using Value = std::string;

/** Criterion value collection
 * When the criterion will be created, it will be checked that no internal values
 * are duplicated (i.e that we have a set of value). Nevertheless, we decided to
 * use a list to store values in order to let the user choose the value order.
 * This is important as the first value will be chosen as default one.
 * @see createExclusiveCriterion
 */
using Values = std::list<Value>;

/** Criterion state representation */
using State = std::set<Value>;

/** Client criterion interface used for interacting with the system state
 * Allows client to set or retrieve a Criterion state.
 */
class CriterionInterface
{
public:
    /** Set a new state to the criterion
     * The state should only be composed of registered values.
     * If the requested state is already set, the function will succeed but no modification
     * will be registered.
     *
     * @param[in] state the state to set
     * @param[out] error the string describing the error if an error occurred
     *                   undefined otherwise
     * @return true if in case of success, false otherwise
     */
    virtual bool setState(const State& state, std::string& error) = 0;

    /** Retrieve the current criterion state */
    virtual State getState() const = 0;

    /** Retrieve Criterion name */
    virtual std::string getName() const = 0;

protected:
    virtual ~CriterionInterface() {}
};

} /** criterion namespace */
} /** core namespace */
