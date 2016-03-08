/*
 * Copyright (c) 2015, Intel Corporation
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

#include "Config.hpp"
#include "ParameterFramework.hpp"

#include <catch.hpp>

#include <string>

#ifndef SCENARIO_METHOD
/** SCENARIO_METHOD is not available in catch on ubuntu 12.04 */
#define SCENARIO_METHOD(className, ...) TEST_CASE_METHOD(className, "Scenario: " __VA_ARGS__)
#endif

namespace parameterFramework
{

/** Value to test with a title.
 *
 * When testing code it is often useful to have an list of possible
 * values and run the test for each of them.
 * This class represents one element of this list. For the complete list
 * see Tests.
 *
 * Catch has no build-in support for such need
 * (in fact it has but it is still experimental, look for "generators")
 * but it can be emulated with a loop over Tests.
 *
 * Each Test MUST specify a unique title, Ie all titles of a Tests MUST
 * be different. This is dued to the way that catch detects that a SECTION
 * has already been run. For more explanation see Tests.
 */
template <class Value>
struct Test
{
    std::string title;
    Value payload;
};

/** Use a vector to represent a collection of test input.
 *
 * This type is designed to be used to parametrize tests.
 * Use it as follow:
 *     for (auto &test : Tests<std::string>{
 *     //                      ^~~~~~~~~~~ Test parameter type
 *             {"an invalid tag", "<invalid tag\"/> "},
 *           //^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Test parameters
 *             {"an unknown tag", "<unknown_tag/>"},
 *           // ^~~~~~~~~~~~~~~~ Unique title across the tests
 *             {"an unclosed tag", "<unclosed>"} }) {
 *           //                    ^~~~~~~~~~~ Value to test
 *         SECTION("Testing: " + test.title) {
 *         //                     ^~~~~~~~~~ Section title MUST unique
 *            test.payload //< value to test
 *            REQUIRE(getTag() != test.payload); // Example
 *            ...
 *         }
 *     }
 *
 *
 * Beware that if Value is not copyable, only movable this will
 * fail to compile as initializer_list does not support move semantic
 * (lets hope it will be fix in C++17).
 *
 * If a new test vector needs to support move, define:
 *     template <class Value>
 *     using MovableTests = Test<value>[];
 * This could be the default but VS2013 does not support it.
 * VS requires that an array size be defined. Thus define
 *     template <class Value, size_t size>
 *     using MovableTests = Test<value>[size];
 * will fix the VS compilation. Nevertheless this means that
 * all move only test vector will need to specify their size
 * which is redondant.
 * This is why it is not the default.
 * Hopefully it will be when VS will support deducing the size.
 */
template <class Value>
using Tests = std::vector<Test<Value>>;

/** Defer Parameter Framework creation.
 * A custom configuration can be provided.
 */
class LazyPF
{
public:
    using PF = ParameterFramework;

    void create(Config &&configFile) { mPf.reset(new PF{std::move(configFile)}); }
    std::unique_ptr<PF> mPf;
};

/** PF that will log a warning at start. */
struct WarningPF : public ParameterFramework
{
    WarningPF() : ParameterFramework{{&Config::domains, "<InvalidDomain/>"}}
    {
        setFailureOnFailedSettingsLoad(false);
    }
};
} // namespace parameterFramework
