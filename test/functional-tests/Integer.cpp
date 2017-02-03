/*
 * Copyright (c) 2016, Intel Corporation
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

const auto validIntegerInstances = Config{&Config::instances,
                                          // Default for integers is unsigned/32bits
                                          R"(<IntegerParameter Name="Empty"/>
    <IntegerParameter Name="trivial" Size="8" Signed="true"/>
    <IntegerParameter Name="nominal" Size="8" Signed="true" Min="-50" Max="12"/>
    <IntegerParameter Name="negMinMaxS8" Size="8" Signed="true" Min="-120" Max="-110"/>
    <IntegerParameter Name="posMinMaxS8" Size="8" Signed="true" Min="90" Max="100"/>
    <IntegerParameter Name="defaultMinS8" Size="8" Signed="true" Min="-128"/>
    <IntegerParameter Name="defaultMaxS8" Size="8" Signed="true" Max="127"/>
    <IntegerParameter Name="defaultMinU8" Size="8" Signed="false" Min="0"/>
    <IntegerParameter Name="defaultMaxU8" Size="8" Signed="false" Max="255"/>
    <IntegerParameter Name="defaultMinS16" Size="16" Signed="true" Min="-32768"/>
    <IntegerParameter Name="defaultMaxS16" Size="16" Signed="true" Max="32767"/>
    <IntegerParameter Name="defaultMinU16" Size="16" Signed="false" Min="0"/>
    <IntegerParameter Name="defaultMaxU16" Size="16" Signed="false" Max="65535"/>
    <IntegerParameter Name="defaultMinS32" Size="32" Signed="true" Min="-2147483648"/>
    <IntegerParameter Name="defaultMaxS32" Size="32" Signed="true" Max="2147483647"/>
    <IntegerParameter Name="defaultMinU32" Size="32" Signed="false" Min="0"/>
    <IntegerParameter Name="defaultMaxU32" Size="32" Signed="false" Max="4294967295"/>)"};
const auto &invalidIntegerParameters = Tests<string>{
    {"invalid Size(64)", "<IntegerParameter Name='error' Size='64'/>"},
    {"minimum > maximum", "<IntegerParameter Name='error' Min='1' Max='0'/>"},
    {"S8 minimum > MaxRange", "<IntegerParameter Name='error' Size='8' Signed='true' Min='128'/>"},
    {"S8 minimum < MinRange", "<IntegerParameter Name='error' Size='8' Signed='true' Min='-129'/>"},
    {"S8 maximum > MaxRange", "<IntegerParameter Name='error' Size='8' Signed='true' Max='128'/>"},
    {"S8 maximum < MinRange", "<IntegerParameter Name='error' Size='8' Signed='true' Max='-129'/>"},
    {"U8 minimum > MaxRange", "<IntegerParameter Name='error' Size='8' Signed='false' Min='256'/>"},
    {"U8 maximum > MaxRange", "<IntegerParameter Name='error' Size='8' Signed='false' Max='256'/>"},
    {"S16 minimum > MaxRange",
     "<IntegerParameter Name='error' Size='16' Signed='true' Min='32768'/>"},
    {"S16 minimum < MinRange",
     "<IntegerParameter Name='error' Size='16' Signed='true' Min='-32769'/>"},
    {"S16 maximum > MaxRange",
     "<IntegerParameter Name='error' Size='16' Signed='true' Max='32768'/>"},
    {"S16 maximum < MinRange",
     "<IntegerParameter Name='error' Size='16' Signed='true' Max='-32769'/>"},
    {"U16 minimum > MaxRange",
     "<IntegerParameter Name='error' Size='16' Signed='false' Min='65536'/>"},
    {"U16 maximum > MaxRange",
     "<IntegerParameter Name='error' Size='16' Signed='false' Max='65536'/>"}};

struct IntegerPF : public ParameterFramework
{
    IntegerPF() : ParameterFramework{std::move(validIntegerInstances)} {}
};

SCENARIO_METHOD(LazyPF, "Invalid Integer types XML structure", "[Integer types]")
{
    for (auto &vec : invalidIntegerParameters) {
        GIVEN ("intentional error: " + vec.title) {
            create(Config{&Config::instances, vec.payload});
            THEN ("Start should fail") {
                CHECK_THROWS_AS(mPf->start(), Exception);
            }
        }
    }
}

SCENARIO_METHOD(IntegerPF, "Integer types", "[Integer types]")
{
    GIVEN ("A valid XML structure file") {
        THEN ("Start should succeed") {
            CHECK_NOTHROW(start());
            REQUIRE_NOTHROW(setTuningMode(true));
            string path = "/test/test/nominal";

            AND_THEN ("Set/Get a integer type parameter in real value space") {

                for (auto &vec : Tests<string>{
                         {"(too high)", "13"}, {"(too low)", "-51"}, {"(not a number)", "foobar"},
                     }) {
                    GIVEN ("Invalid value " + vec.title) {
                        CHECK_THROWS_AS(setParameter(path, vec.payload), Exception);
                    }
                }
                for (auto &vec : Tests<string>{
                         {"(upper limit)", "12"}, {"(lower limit)", "-50"}, {"(inside range)", "0"},
                     }) {
                    GIVEN ("A valid value " + vec.title) {
                        CHECK_NOTHROW(setParameter(path, vec.payload));
                        string getValueBack;
                        REQUIRE_NOTHROW(getParameter(path, getValueBack));
                        CHECK(getValueBack == vec.payload);
                    }
                }
            }

            AND_THEN ("Set/Get a integer type parameter in raw value space") {
                REQUIRE_NOTHROW(setRawValueSpace(true));
                REQUIRE_NOTHROW(setHexOutputFormat(true));

                for (auto &vec : Tests<string>{
                         {"(too high)", "0x0D"}, {"(too low)", "0xCD"},
                     }) {
                    GIVEN ("Invalid value " + vec.title) {
                        CHECK_THROWS_AS(setParameter(path, vec.payload), Exception);
                    }
                }
                for (auto &vec : Tests<string>{
                         {"(upper limit)", "0x0C"},
                         {"(lower limit)", "0xCE"},
                         {"(inside range)", "0x00"},
                     }) {
                    GIVEN ("A valid value " + vec.title) {
                        CHECK_NOTHROW(setParameter(path, vec.payload));
                        string getValueBack;
                        REQUIRE_NOTHROW(getParameter(path, getValueBack));
                        CHECK(getValueBack == vec.payload);
                    }
                }
            }

            AND_THEN ("Set/Get integer type parameter handle") {
                ElementHandle handle{*this, path};
                /** @FIXME: 'set' operations on a ParameterHandle are silently
                 * ignored in tuning mode. Does it make sense ? */
                REQUIRE_NOTHROW(setTuningMode(false));

                for (auto &vec : Tests<int32_t>{
                         {"(upper limit)", 12}, {"(lower limit)", -50}, {"(inside range)", 0},
                     }) {
                    GIVEN ("A valid value " + vec.title) {
                        CHECK_NOTHROW(handle.setAsSignedInteger(vec.payload));
                        int32_t getValueBack;
                        REQUIRE_NOTHROW(handle.getAsSignedInteger(getValueBack));
                        CHECK(getValueBack == vec.payload);
                    }
                }
                for (auto &vec : Tests<int32_t>{
                         {"(too high)", 13}, {"(too low)", -51},
                     }) {
                    GIVEN ("An invalid value " + vec.title) {
                        CHECK_THROWS_AS(handle.setAsSignedInteger(vec.payload), Exception);
                    }
                }
            }

            AND_THEN ("Set/Get double type parameter handle") {
                ElementHandle handle{*this, path};
                /** @FIXME: 'set' operations on a ParameterHandle are silently
                 * ignored in tuning mode. Does it make sense ? */
                REQUIRE_NOTHROW(setTuningMode(false));

                for (auto &vec : Tests<double>{
                         {"(upper limit)", 12.0f},
                         {"(lower limit)", -50.0f},
                         {"(inside range)", 0.0f},
                     }) {
                    GIVEN ("A valid value (rejected not supported)" + vec.title) {
                        CHECK_THROWS_AS(handle.setAsDouble(vec.payload), Exception);
                    }
                }
                for (auto &vec : Tests<double>{
                         {"(too high)", 12.01f}, {"(too low)", -50.01f},
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
