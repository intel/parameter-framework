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

const auto validBitParameterInstances = Config{
    &Config::instances,
    // Default for integers is unsigned/32bits
    R"(<BitParameterBlock Name="nominal" Size="16"><BitParameter Pos="0" Size="1" Name="bool"/><BitParameter Pos="1" Size="2" Name="twobits"/><BitParameter Pos="3" Size="3" Max="6" Name="treebits"/></BitParameterBlock>)"};

const auto &invalidBitParameterParameters =
    Tests<string>{{"Too much bits", "<BitParameterBlock Name='toomuchbits' Size='8'><BitParameter "
                                    "Pos='1' Size='2' Name='twobits'/><BitParameter Pos='3' "
                                    "Size='7' Name='toobits'/></BitParameterBlock>"},
                  {"Max too high", "<BitParameterBlock Name='maxtoohigh' Size='8'><BitParameter "
                                   "Pos='1' Size='2' Name='twobits'/><BitParameter Pos='3' "
                                   "Size='2' Max='5' Name='toohigh'/></BitParameterBlock>"}};

struct BitParameterPF : public ParameterFramework
{
    BitParameterPF() : ParameterFramework{std::move(validBitParameterInstances)} {}
};

SCENARIO_METHOD(LazyPF, "Invalid BitParameter types XML structure", "[BitParameter types]")
{
    for (auto &vec : invalidBitParameterParameters) {
        GIVEN ("intentional error: " + vec.title) {
            create(Config{&Config::instances, vec.payload});
            THEN ("Start should fail") {
                CHECK_THROWS_AS(mPf->start(), Exception);
            }
        }
    }
}

SCENARIO_METHOD(BitParameterPF, "BitParameter types", "[BitParameter types]")
{
    GIVEN ("A valid XML structure file") {
        THEN ("Start should succeed") {
            CHECK_NOTHROW(start());
            REQUIRE_NOTHROW(setTuningMode(true));
            string path = "/test/test/nominal/treebits";

            AND_THEN ("Set/Get a BitParameter type parameter in real value space") {

                for (auto &vec : Tests<string>{
                         {"(too high)", "7"}, {"(too low)", "-1"},
                     }) {
                    GIVEN ("Invalid value " + vec.title) {
                        CHECK_THROWS_AS(setParameter(path, vec.payload), Exception);
                    }
                }
                for (auto &vec : Tests<string>{
                         {"(upper limit)", "6"}, {"(lower limit)", "0"},
                     }) {
                    GIVEN ("A valid value " + vec.title) {
                        CHECK_NOTHROW(setParameter(path, vec.payload));
                        string getValueBack;
                        REQUIRE_NOTHROW(getParameter(path, getValueBack));
                        CHECK(getValueBack == vec.payload);
                    }
                }
            }

            AND_THEN ("Set/Get a BitParameter type parameter in real value space") {
                ElementHandle handle{*this, path};
                REQUIRE_NOTHROW(setRawValueSpace(true));
                REQUIRE_NOTHROW(setHexOutputFormat(true));

                for (auto &vec : Tests<string>{
                         {"(too high)", "0x7"},
                     }) {
                    GIVEN ("Invalid value " + vec.title) {
                        CHECK_THROWS_AS(setParameter(path, vec.payload), Exception);
                    }
                }
                for (auto &vec : Tests<string>{
                         {"(upper limit)", "0x6"}, {"(lower limit)", "0x0"},
                     }) {
                    GIVEN ("A valid value " + vec.title) {
                        CHECK_NOTHROW(setParameter(path, vec.payload));
                        string getValueBack;
                        REQUIRE_NOTHROW(getParameter(path, getValueBack));
                        CHECK(getValueBack == vec.payload);
                    }
                }
            }

            AND_THEN ("Set/Get a BitParameter type parameter in boolean") {
                ElementHandle handle{*this, path};
                /** @FIXME: 'set' operations on a ParameterHandle are silently
                 * ignored in tuning mode. Does it make sense ? */
                REQUIRE_NOTHROW(setTuningMode(false));

                for (auto &vec : Tests<bool>{
                         {"(upper limit)", true}, {"(lower limit)", false},
                     }) {
                    GIVEN ("Invalid value " + vec.title) {
                        CHECK_THROWS_AS(handle.setAsBoolean(vec.payload), Exception);
                    }
                }
            }

            AND_THEN ("Set/Get a BitParameter type parameter in boolean") {
                path = "/test/test/nominal/bool";
                ElementHandle handle{*this, path};
                /** @FIXME: 'set' operations on a ParameterHandle are silently
                 * ignored in tuning mode. Does it make sense ? */
                REQUIRE_NOTHROW(setTuningMode(false));

                for (auto &vec : Tests<bool>{
                         {"(upper limit)", true}, {"(lower limit)", false},
                     }) {
                    GIVEN ("A valid value " + vec.title) {
                        CHECK_NOTHROW(handle.setAsBoolean(vec.payload));
                        bool getValueBack;
                        REQUIRE_NOTHROW(handle.getAsBoolean(getValueBack));
                        CHECK(getValueBack == vec.payload);
                    }
                }
            }
        }
    }
}
}
