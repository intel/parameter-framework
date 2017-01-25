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

const auto validFixedPointInstances =
    Config{&Config::instances,
           // Default for integers is unsigned/32bits
           R"(<FixedPointParameter Name="nominal" Size="16" Integral="2" Fractional="7"/>)"};
const auto &invalidFixedPointParameters =
    Tests<string>{{"U8 Dec + Fractional > 7",
                   "<FixedPointParameter Name='q2.6' Size='8' Integral='2' Fractional='6'/>"},
                  {"U16 Dec + Fractional > 15",
                   "<FixedPointParameter Name='q9.7' Size='16' Integral='9' Fractional='7'/>"},
                  {"U32 Fractional > 31",
                   "<FixedPointParameter Name='q0.32' Size='32' Integral='0' Fractional='32'/>"}};

struct FixedPointPF : public ParameterFramework
{
    FixedPointPF() : ParameterFramework{std::move(validFixedPointInstances)} {}
};

SCENARIO_METHOD(LazyPF, "Invalid FixedPoint types XML structure", "[FixedPoint types]")
{
    for (auto &vec : invalidFixedPointParameters) {
        GIVEN ("intentional error: " + vec.title) {
            create(Config{&Config::instances, vec.payload});
            THEN ("Start should fail") {
                CHECK_THROWS_AS(mPf->start(), Exception);
            }
        }
    }
}

SCENARIO_METHOD(FixedPointPF, "FixedPoint types", "[FixedPoint types]")
{
    GIVEN ("A valid XML structure file") {
        THEN ("Start should succeed") {
            CHECK_NOTHROW(start());
            REQUIRE_NOTHROW(setTuningMode(true));
            string path = "/test/test/nominal";

            AND_THEN ("Set/Get a Fixed Point type parameter in real value space") {
                for (auto &vec : Tests<string>{
                         {"(too high)", "4.0000000"},
                         {"(too low)", "-4.0078125"},
                         {"(not a number)", "foobar"},
                     }) {
                    GIVEN ("Invalid value " + vec.title) {
                        CHECK_THROWS_AS(setParameter(path, vec.payload), Exception);
                    }
                }
                for (auto &vec : Tests<string>{
                         {"(upper limit)", "3.9921875"},
                         {"(lower limit)", "-4.0000000"},
                         {"(inside range)", "0.0000000"},
                     }) {
                    GIVEN ("A valid value " + vec.title) {
                        CHECK_NOTHROW(setParameter(path, vec.payload));
                        string getValueBack;
                        REQUIRE_NOTHROW(getParameter(path, getValueBack));
                        CHECK(getValueBack == vec.payload);
                    }
                }
            }
            AND_THEN ("Set/Get a Fixed Point parameter in raw value space") {
                REQUIRE_NOTHROW(setRawValueSpace(true));

                for (auto &vec : Tests<string>{
                         {"(upper limit) as decimal", "32767"},
                         {"(lower limit) as decimal", "-32768"},
                         {"(inside range) as decimal", "0"},
                     }) {
                    GIVEN ("A valid value " + vec.title) {
                        CHECK_NOTHROW(setParameter(path, vec.payload));
                        string getValueBack;
                        REQUIRE_NOTHROW(getParameter(path, getValueBack));
                        CHECK(getValueBack == vec.payload);
                    }
                }
                for (auto &vec : Tests<string>{
                         {"(too high)", "32768"}, {"(too low)", "-32769"},
                     }) {
                    GIVEN ("Invalid value interger" + vec.title) {
                        CHECK_THROWS_AS(setParameter(path, vec.payload), Exception);
                    }
                }
                for (auto &vec : Tests<string>{
                         {"(too high)", "0x10000"}, {"(too low)", "0xfffe0000"},
                     }) {
                    GIVEN ("Invalid value hexa" + vec.title) {
                        CHECK_THROWS_AS(setParameter(path, vec.payload), Exception);
                    }
                }
            }
        }
    }
}
}
