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
#   define SCENARIO_METHOD(className, ...) \
        TEST_CASE_METHOD(className, "Scenario: " __VA_ARGS__)
#endif

namespace parameterFramework
{

template <class Value>
struct Test
{
    std::string title;
    Value payload;
};

/** Using C style array instead of a C++ collection to workaround
 *  initializer list not supporting move semantic.
 */
template <class Value>
using Tests = Test<Value>[];

/** Defer Parameter Framework creation.
 * A custom configuration can be provided.
 */
class LazyPF
{
public:
    using PF = ParameterFramework;

    void create(Config &&configFile)
    {
        mPf.reset(new PF{ std::move(configFile) });
    }
    std::unique_ptr<PF> mPf;
};

/** PF that will log a warning at start. */
struct WarningPF : public ParameterFramework
{
    WarningPF() :
        ParameterFramework{ { &Config::domains, "<InvalidDomain/>" } }
    {
        setFailureOnFailedSettingsLoad(false);
    }
};

}
