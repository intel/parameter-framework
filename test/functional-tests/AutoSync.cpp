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
#include <SubsystemObject.h>
#include <IntrospectionEntryPoint.h>
#include "Test.hpp"
#include <catch.hpp>
#include <string>

using std::string;

namespace parameterFramework
{

struct BoolPF : public ParameterFramework
{
    BoolPF() : ParameterFramework{createConfig()} {}

    /** Set the boolean parameter value within the "Conf" configuration,
     * which is always applicable. */
    void setParameterValue(bool value)
    {
        std::string valueStr = value ? "1" : "0";
        setConfigurationParameter("Domain", "Conf", "/test/test/param", valueStr);
    }

private:
    static Config createConfig()
    {
        Config config;
        config.instances = R"(<BooleanParameter Name="param" Mapping="Object"/>)";
        config.plugins = {{"", {"introspection-subsystem"}}};
        config.subsystemType = "INTROSPECTION";

        config.domains = R"(<ConfigurableDomain Name="Domain">
                                <Configurations>
                                    <Configuration Name="Conf">
                                        <CompoundRule Type="All"/>
                                    </Configuration>
                                </Configurations>

                                <ConfigurableElements>
                                    <ConfigurableElement Path="/test/test/param"/>
                                </ConfigurableElements>

                                <Settings>
                                    <Configuration Name="Conf">
                                        <ConfigurableElement Path="/test/test/param">
                                            <BooleanParameter Name="param">0</BooleanParameter>
                                        </ConfigurableElement>
                                    </Configuration>
                                </Settings>
                            </ConfigurableDomain>)";

        return config;
    }
};

SCENARIO_METHOD(BoolPF, "Auto sync")
{
    GIVEN ("A Pfw that starts") {
        REQUIRE_NOTHROW(start());

        THEN ("Parameter value is false according to the settings") {
            REQUIRE_FALSE(introspectionSubsystem::getParameterValue());

            AND_THEN ("Tuning is off") {
                REQUIRE_FALSE(isTuningModeOn());

                WHEN ("Turning autosync on") {
                    REQUIRE_NOTHROW(setAutoSync(true));

                    AND_WHEN ("A parameter is set") {
                        REQUIRE_NOTHROW(setParameterValue(true));

                        THEN ("Sync is done") {
                            CHECK(introspectionSubsystem::getParameterValue());
                        }
                    }
                }
                WHEN ("Turning autosync off") {
                    REQUIRE_NOTHROW(setAutoSync(false));

                    AND_WHEN ("A parameter is set") {
                        REQUIRE_NOTHROW(setParameterValue(true));

                        THEN ("Sync should not have occurred yet") {
                            REQUIRE_FALSE(introspectionSubsystem::getParameterValue());

                            WHEN ("Turning autosync on") {
                                REQUIRE_NOTHROW(setAutoSync(true));

                                THEN ("Sync is done") {
                                    CHECK(introspectionSubsystem::getParameterValue());
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
}
