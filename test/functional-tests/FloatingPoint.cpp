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

#include "Config.hpp"
#include "ParameterFramework.hpp"
#include "ElementHandle.hpp"
#include "Test.hpp"
#include "BinaryCopy.hpp"

#include <catch.hpp>

#include <string>

using std::string;

namespace parameterFramework
{

const auto validInstances = Config{&Config::instances,
                                   // Size is fixed at 32 and as such is optional */
                                   R"(<FloatingPointParameter Name="Empty"/>
    <FloatingPointParameter Name="trivial" Size="32"/>
    <FloatingPointParameter Name="nominal" Size="32" Min="-50.4" Max="12.2"/>
    <FloatingPointParameter Name="defaultMin" Size="32" Max="12.2"/>
    <FloatingPointParameter Name="defaultMax" Size="32" Min="-50.4"/>)"};
const auto &invalidParameters =
    Tests<string>{{"invalid Size(64)", "<FloatingPointParameter Name='error' Size='64'/>"},
                  {"invalid Size(16)", "<FloatingPointParameter Name='error' Size='16'/>"},
                  {"minimum > maximum", "<FloatingPointParameter Name='error' Min='1' Max='0'/>"}};

struct FloatsPF : public ParameterFramework
{
    FloatsPF() : ParameterFramework{std::move(validInstances)} {}
};

SCENARIO_METHOD(LazyPF, "Invalid floating points XML structure", "[floating point]")
{
    for (auto &vec : invalidParameters) {
        GIVEN ("intentional error: " + vec.title) {
            create(Config{&Config::instances, vec.payload});
            THEN ("Start should fail") {
                CHECK_THROWS_AS(mPf->start(), Exception);
            }
        }
    }
}

SCENARIO_METHOD(FloatsPF, "Floating points", "[floating points]")
{
    GIVEN ("A valid XML structure file") {
        THEN ("Start should succeed") {
            CHECK_NOTHROW(start());
            REQUIRE_NOTHROW(setTuningMode(true));
            string path = "/test/test/nominal";

            AND_THEN ("Set/Get a floating point parameter in real value space") {

                for (auto &vec : Tests<string>{
                         {"(too high)", "12.3"},
                         {"(too low)", "-50.5"},
                         {"(not a number)", "foobar"},
                     }) {
                    GIVEN ("Invalid value " + vec.title) {
                        CHECK_THROWS_AS(setParameter(path, vec.payload), Exception);
                    }
                }
                for (auto &vec : Tests<string>{
                         {"(upper limit)", "12.2"},
                         {"(lower limit)", "-50.4"},
                         {"(inside range)", "0"},
                     }) {
                    GIVEN ("A valid value " + vec.title) {
                        CHECK_NOTHROW(setParameter(path, vec.payload));
                        string getValueBack;
                        REQUIRE_NOTHROW(getParameter(path, getValueBack));
                        CHECK(getValueBack == vec.payload);
                    }
                }
            }

            AND_THEN ("Set/Get a floating point parameter in raw value space") {
                const float tooHigh = 12.3f;
                const float tooLow = -50.5f;
                const float nan = std::numeric_limits<float>::quiet_NaN();
                const float inf = std::numeric_limits<float>::infinity();
                REQUIRE_NOTHROW(setRawValueSpace(true));
                for (auto &vec : Tests<string>{
                         {"(too high, as decimal)",
                          std::to_string(::utility::binaryCopy<uint32_t>(tooHigh))},
                         {"(too low, as decimal)",
                          std::to_string(::utility::binaryCopy<uint32_t>(tooLow))},
                         {"(meaningless)", "foobar"},
                         {"(infinity)", std::to_string(::utility::binaryCopy<uint32_t>(inf))},
                         {"(NaN)", std::to_string(::utility::binaryCopy<uint32_t>(nan))},
                     }) {
                    GIVEN ("Invalid value " + vec.title) {
                        CHECK_THROWS_AS(setParameter(path, vec.payload), Exception);
                    }
                }
                const float upper = 12.2f;
                const float lower = -50.4f;
                const float zero = 0.0f;
                for (auto &vec : Tests<string>{
                         {"(upper limit, as decimal)",
                          std::to_string(::utility::binaryCopy<uint32_t>(upper))},
                         {"(lower limit, as decimal)",
                          std::to_string(::utility::binaryCopy<uint32_t>(lower))},
                         {"(inside range, as decimal)",
                          std::to_string(::utility::binaryCopy<uint32_t>(zero))},
                     }) {
                    GIVEN ("A valid value " + vec.title) {
                        CHECK_NOTHROW(setParameter(path, vec.payload));
                        string getValueBack;
                        REQUIRE_NOTHROW(getParameter(path, getValueBack));
                        CHECK(getValueBack == vec.payload);
                    }
                }
            }

            AND_THEN ("Set/Get floating point parameter handle") {
                ElementHandle handle{*this, path};
                /** @FIXME: 'set' operations on a ParameterHandle are silently
                 * ignored in tuning mode. Does it make sense ? */
                REQUIRE_NOTHROW(setTuningMode(false));

                /* warning: even though the API below takes a double as
                 * argument, we need to define the test vector as floats in
                 * order to prevent rounding issues */
                for (auto &vec : Tests<float>{
                         {"(upper limit)", 12.2f},
                         {"(lower limit)", -50.4f},
                         {"(inside range)", 0.0f},
                     }) {
                    GIVEN ("A valid value " + vec.title) {
                        CHECK_NOTHROW(handle.setAsDouble(vec.payload));
                        double getValueBack;
                        REQUIRE_NOTHROW(handle.getAsDouble(getValueBack));
                        CHECK(getValueBack == vec.payload);
                    }
                }
                for (auto &vec : Tests<float>{
                         {"(too high)", 12.3f}, {"(too low)", -50.5f},
                     }) {
                    GIVEN ("An invalid value " + vec.title) {
                        CHECK_THROWS_AS(handle.setAsDouble(vec.payload), Exception);
                    }
                }
            }
        }
    }
}
}
