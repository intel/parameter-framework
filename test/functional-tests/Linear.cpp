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
#include "BinaryCopy.hpp"

#include <catch.hpp>

#include <string>

using std::string;

namespace parameterFramework
{

const auto validLinearInstances = Config{
    &Config::instances,
    // Size is fixed at 32 and as such is optional */
    R"(<IntegerParameter Name="trivial" Signed="true"> <LinearAdaptation SlopeNumerator="200" SlopeDenominator="10"/> </IntegerParameter>
    <IntegerParameter Name="nominal" Size="32" Signed="true" Min="-1440" Max="300"> <LinearAdaptation SlopeNumerator="10" SlopeDenominator="100"/> </IntegerParameter>)"};
const auto &invalidLinearParameters = Tests<string>{
    {"invalid Size(64)", "<IntegerParameter Name='nominal' Size='64' Signed='true' Min='-144' "
                         "Max='30'> <LinearAdaptation SlopeNumerator='200' SlopeDenominator='10'/> "
                         "</IntegerParameter>"},
    {"minimum > maximum", "<IntegerParameter Name='nominal' Size='32' Signed='true' Min='30' "
                          "Max='-144'> <LinearAdaptation SlopeNumerator='1' "
                          "SlopeDenominator='10'/> </IntegerParameter>"},
    {"SlopeDenominator=0",
     "<IntegerParameter Name='nominal' Size='32' Signed='true' Min='-144' Max='30'> "
     "<LinearAdaptation SlopeNumerator='1' SlopeDenominator='0'/> "
     "</IntegerParameter>"}};

struct LinearsPF : public ParameterFramework
{
    LinearsPF() : ParameterFramework{std::move(validLinearInstances)} {}
};

SCENARIO_METHOD(LazyPF, "Invalid Linear points XML structure", "[Linear Type]")
{
    for (auto &vec : invalidLinearParameters) {
        GIVEN ("intentional error: " + vec.title) {
            create(Config{&Config::instances, vec.payload});
            THEN ("Start should fail") {
                CHECK_THROWS_AS(mPf->start(), Exception);
            }
        }
    }
}

SCENARIO_METHOD(LinearsPF, "Linear points", "[Linear Type]")
{
    GIVEN ("A valid XML structure file") {
        THEN ("Start should succeed") {
            CHECK_NOTHROW(start());
            REQUIRE_NOTHROW(setTuningMode(true));
            string path = "/test/test/nominal";
            AND_THEN ("Set/Get a Loagaritmic point parameter in real value space") {

                for (auto &vec : Tests<string>{
                         {"(too high)", "301"},
                         {"(too low)", "-1441"},
                         {"(not a number)", "foobar"},
                     }) {
                    GIVEN ("Invalid value " + vec.title) {
                        CHECK_THROWS_AS(setParameter(path, vec.payload), Exception);
                    }
                }
                for (auto &vec : Tests<string>{
                         {"(upper limit)", "300"},
                         {"(lower limit)", "-1440"},
                         {"(inside range)", "0"},
                     }) {
                    GIVEN ("A valid value " + vec.title) {
                        CHECK_NOTHROW(setParameter(path, vec.payload));
                        string getValueBack = "-11";
                        REQUIRE_NOTHROW(getParameter(path, getValueBack));
                        CHECK(getValueBack == vec.payload);
                    }
                }
            }

            AND_THEN ("Set/Get double type parameter handle") {
                ElementHandle handle{*this, path};
                /** @FIXME: 'set' operations on a ParameterHandle are silently
                 * ignored in tuning mode. Does it make sense ? */
                REQUIRE_NOTHROW(setTuningMode(false));
                REQUIRE_NOTHROW(setRawValueSpace(true));

                /* nominal is defined as a Num=10,Denum=100 (division by 10). So limits should be 10
                 * above Mina dn Max Integer value in order to get exception. */
                for (auto &vec : Tests<double>{
                         {"(upper limit)", 3000.0f},
                         {"(lower limit)", -14400.0f},
                         {"(inside range)", 0.0f},
                     }) {
                    GIVEN ("A valid value " + vec.title) {
                        CHECK_NOTHROW(handle.setAsDouble(vec.payload));
                        double getValueBack = 1.0f;
                        REQUIRE_NOTHROW(handle.getAsDouble(getValueBack));
                        CHECK(getValueBack == vec.payload);
                    }
                }
                for (auto &vec : Tests<double>{
                         {"(too high)", 3010.0f}, {"(too low)", -14410.00f},
                     }) {
                    GIVEN ("An invalid value " + vec.title) {
                        CHECK_THROWS_AS(handle.setAsDouble(vec.payload), Exception);
                    }
                }
            }
        }
    }
}
} // namespace parameterFramework
