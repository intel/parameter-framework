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

const auto validLogarithmicInstances = Config{
    &Config::instances,
    // Size is fixed at 32 and as such is optional */
    R"(<IntegerParameter Name="trivial" Signed="true"> <LogarithmicAdaptation SlopeNumerator="200" LogarithmBase="10"/> </IntegerParameter>
    <IntegerParameter Name="nominal" Size="32" Signed="true" Min="-144" Max="30"> <LogarithmicAdaptation SlopeNumerator="1" LogarithmBase="10"/> </IntegerParameter>)"};
const auto &invalidLogarithmicParameters = Tests<string>{
    {"invalid Size(64)", "<IntegerParameter Name='nominal' Size='64' Signed='true' Min='-144' "
                         "Max='30'> <LogarithmicAdaptation SlopeNumerator='1' LogarithmBase='10'/> "
                         "</IntegerParameter>"},
    {"minimum > maximum", "<IntegerParameter Name='nominal' Size='32' Signed='true' Min='30' "
                          "Max='-144'> <LogarithmicAdaptation SlopeNumerator='1' "
                          "LogarithmBase='10'/> </IntegerParameter>"},
    {"logBase =1", "<IntegerParameter Name='nominal' Size='32' Signed='true' Min='-144' Max='30'> "
                   "<LogarithmicAdaptation SlopeNumerator='1' LogarithmBase='1'/> "
                   "</IntegerParameter>"},
    {"logBase negative", "<IntegerParameter Name='nominal' Size='32' Signed='true' Min='-144' "
                         "Max='30'> <LogarithmicAdaptation SlopeNumerator='1' "
                         "LogarithmBase='-10'/> </IntegerParameter>"}};

struct LogarithmicsPF : public ParameterFramework
{
    LogarithmicsPF() : ParameterFramework{std::move(validLogarithmicInstances)} {}
};

SCENARIO_METHOD(LazyPF, "Invalid Logarithmic points XML structure", "[Logarithmic Type]")
{
    for (auto &vec : invalidLogarithmicParameters) {
        GIVEN ("intentional error: " + vec.title) {
            create(Config{&Config::instances, vec.payload});
            THEN ("Start should fail") {
                CHECK_THROWS_AS(mPf->start(), Exception);
            }
        }
    }
}

SCENARIO_METHOD(LogarithmicsPF, "Logarithmic points", "[Logarithmic Type]")
{
    GIVEN ("A valid XML structure file") {
        THEN ("Start should succeed") {
            CHECK_NOTHROW(start());
            REQUIRE_NOTHROW(setTuningMode(true));
            string path = "/test/test/nominal";
            AND_THEN ("Set/Get a Loagaritmic point parameter in real value space") {

                for (auto &vec : Tests<string>{
                         {"(too high)", "31"}, {"(too low)", "-145"}, {"(not a number)", "foobar"},
                     }) {
                    GIVEN ("Invalid value " + vec.title) {
                        CHECK_THROWS_AS(setParameter(path, vec.payload), Exception);
                    }
                }
                for (auto &vec : Tests<string>{
                         {"(upper limit)", "30"},
                         {"(lower limit)", "-144"},
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
        }
    }
}
} // namespace parameterFramework
