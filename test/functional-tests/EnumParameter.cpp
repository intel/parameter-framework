/*
 * Copyright (c) 2017, Intel Corporation
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

#include <catch.hpp>

#include <string>

using std::string;

namespace parameterFramework
{

const auto validEnumParameterInstances = Config{
    &Config::instances,
    // Default for integers is unsigned/32bits
    R"(<EnumParameter Name="nominal" Size="8"><ValuePair Literal="on" Numerical="3"/><ValuePair Literal="off" Numerical="0"/></EnumParameter>)"};

const auto &invalidEnumParameterParameters = Tests<string>{
    {"No Size", "<EnumParameter Name='nosize'><ValuePair Literal='on' Numerical='3'/><ValuePair "
                "Literal='off' Numerical='0'/></EnumParameter>"}};

struct EnumParameterPF : public ParameterFramework
{
    EnumParameterPF() : ParameterFramework{std::move(validEnumParameterInstances)} {}
};

SCENARIO_METHOD(LazyPF, "Invalid EnumParameter types XML structure", "[EnumParameter types]")
{
    for (auto &vec : invalidEnumParameterParameters) {
        GIVEN ("intentional error: " + vec.title) {
            create(Config{&Config::instances, vec.payload});
            THEN ("Start should fail") {
                CHECK_THROWS_AS(mPf->start(), Exception);
            }
        }
    }
}

SCENARIO_METHOD(EnumParameterPF, "EnumParameter types", "[EnumParameter types]")
{
    GIVEN ("A valid XML structure file") {
        THEN ("Start should succeed") {
            CHECK_NOTHROW(start());
            REQUIRE_NOTHROW(setTuningMode(true));
            string path = "/test/test/nominal";

            AND_THEN ("Set/Get a EnumParameter type parameter in real value space") {

                for (auto &vec : Tests<string>{
                         {"(not a number)", "foobar"},
                     }) {
                    GIVEN ("Invalid value " + vec.title) {
                        CHECK_THROWS_AS(setParameter(path, vec.payload), Exception);
                    }
                }
                for (auto &vec : Tests<string>{
                         {"(upper limit)", "on"}, {"(lower limit)", "off"},
                     }) {
                    GIVEN ("A valid value " + vec.title) {
                        CHECK_NOTHROW(setParameter(path, vec.payload));
                        string getValueBack;
                        REQUIRE_NOTHROW(getParameter(path, getValueBack));
                        CHECK(getValueBack == vec.payload);
                    }
                }
            }

            AND_THEN ("Set/Get a EnumParameter type parameter in raw value space") {
                ElementHandle handle{*this, path};
                REQUIRE_NOTHROW(setRawValueSpace(true));

                for (auto &vec : Tests<string>{{"(too high)", "7"}, {"(too low)", "-1"}}) {
                    GIVEN ("Invalid value " + vec.title) {
                        CHECK_THROWS_AS(setParameter(path, vec.payload), Exception);
                    }
                }
                for (auto &vec : Tests<string>{
                         {"(upper limit)", "3"}, {"(lower limit)", "0"},
                     }) {
                    GIVEN ("A valid value " + vec.title) {
                        CHECK_NOTHROW(setParameter(path, vec.payload));
                        string getValueBack;
                        REQUIRE_NOTHROW(getParameter(path, getValueBack));
                        CHECK(getValueBack == vec.payload);
                    }
                }

                REQUIRE_NOTHROW(setHexOutputFormat(true));

                for (auto &vec : Tests<string>{{"(too high)", "0x7"}, {"(too low)", "0xFF"}}) {
                    GIVEN ("Invalid value " + vec.title) {
                        CHECK_THROWS_AS(setParameter(path, vec.payload), Exception);
                    }
                }
                for (auto &vec : Tests<string>{
                         {"(upper limit)", "0x03"}, {"(lower limit)", "0x00"},
                     }) {
                    GIVEN ("A valid value " + vec.title) {
                        CHECK_NOTHROW(setParameter(path, vec.payload));
                        string getValueBack;
                        REQUIRE_NOTHROW(getParameter(path, getValueBack));
                        CHECK(getValueBack == vec.payload);
                    }
                }
            }

            AND_THEN ("Set/Get a EnumParameter type parameter in Integer value space") {
                ElementHandle handle{*this, path};
                /** @FIXME: 'set' operations on a ParameterHandle are silently
                 * ignored in tuning mode. Does it make sense ? */
                REQUIRE_NOTHROW(setTuningMode(false));

                for (auto &vec : Tests<int32_t>{
                         {"(upper limit)", 3}, {"(lower limit)", 0},
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
        }
    }
}
}
