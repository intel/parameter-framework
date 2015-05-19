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

#include "ParameterFramework.h"
#include "FullIo.hpp"

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main()
#include <catch.hpp>

#include <string>
#include <memory>
#include <vector>

#include <cstring>
#include <cerrno>
#include <climits>
extern "C"
{
#include <unistd.h>
}

struct Test
{
    /** @return true if str is empty. */
    bool empty(const char *str)
    {
        REQUIRE(str != NULL);
        return *str == '\0';
    }

    void REQUIRE_FAILURE(bool success)
    {
        THEN("It should be an error") {
            INFO("Previous pfw log: \n" + logLines);
            CAPTURE(pfwGetLastError(pfw));
            CHECK(not success);
            CHECK(not empty(pfwGetLastError(pfw)));
        }
    }

    void REQUIRE_SUCCESS(bool success)
    {
        THEN("It should be a success") {
            INFO("Previous pfw log: \n" + logLines);
            CAPTURE(pfwGetLastError(pfw));
            CHECK(success);
            CHECK(empty(pfwGetLastError(pfw)));
        }
    }

    /** Class to create a temporary file */
    class TmpFile
    {
    public:
        TmpFile(const std::string &content) {
            char tmpName[] = "./tmpPfwUnitTestXXXXXX";
            mFd = mkstemp(tmpName);
            CAPTURE(errno);
            REQUIRE(mFd != -1);
            mPath = tmpName;
            REQUIRE(utility::fullWrite(mFd, content.c_str(), content.length()));
        }
        ~TmpFile() {
            CHECK(close(mFd) != -1);
            unlink(mPath.c_str());
        }
        operator const char *() const { return mPath.c_str(); }
        const std::string &path() const { return mPath; }
    private:
        std::string mPath;
        int mFd;
    };

    /** Log in logLines. */
    static void logCb(void *voidLogLines, PfwLogLevel level, const char *logLine)
    {
        std::string &logLines = *reinterpret_cast<std::string *>(voidLogLines);
        switch(level) {
        case pfwLogWarning:
            logLines += "Warning: ";
            break;
        case pfwLogInfo:
            logLines += "Info: ";
        }
        logLines += logLine;
        logLines += '\n';
    }

    /** Log buffer, will only be display in case of failure */
    std::string logLines;

    /** Pfw handler used in the tests. */
    PfwHandler *pfw;

};

TEST_CASE_METHOD(Test, "Parameter-framework c api use") {
    // Create criteria
    const char *letterList[] = {"a", "b", "c", NULL};
    const char *numberList[] = {"1", "2", "3", NULL};
    const PfwCriterion criteria[] = {
        {"inclusiveCrit", true, letterList},
        {"exclusiveCrit", false, numberList},
    };
    size_t criterionNb = sizeof(criteria)/sizeof(criteria[0]);
    PfwLogger logger = {&logLines, logCb};

    // Create valid pfw config file
    const char *intParameterPath = "/test/system/integer";
    const char *stringParameterPath = "/test/system/string";
    TmpFile system("<?xml version='1.0' encoding='UTF-8'?>\
        <Subsystem Name='system' Type='Virtual' Endianness='Little'>\
            <ComponentLibrary/>\
            <InstanceDefinition>\
                <IntegerParameter Name='integer' Size='32' Signed='true' Max='100'/>\
                <StringParameter Name='string' MaxLength='9'/>\
            </InstanceDefinition>\
        </Subsystem>");
    TmpFile libraries("<?xml version='1.0' encoding='UTF-8'?>\
        <SystemClass Name='test'>\
            <SubsystemInclude Path='" + system.path() + "'/>\
        </SystemClass>");
    TmpFile config("<?xml version='1.0' encoding='UTF-8'?>\
        <ParameterFrameworkConfiguration\
            SystemClassName='test' TuningAllowed='false'>\
            <SubsystemPlugins/>\
            <StructureDescriptionFileLocation Path='" + libraries.path() + "'/>\
        </ParameterFrameworkConfiguration>");

    GIVEN("A created parameter framework") {
        pfw = pfwCreate();
        REQUIRE(pfw != NULL);

        THEN("Error message should be empty") {
            CHECK(empty(pfwGetLastError(pfw)));
        }

        WHEN("The pfw is started without an handler") {
            CHECK(not pfwStart(NULL, config, criteria, criterionNb, &logger));
        }
        WHEN("The pfw is started without a config path") {
            REQUIRE_FAILURE(pfwStart(pfw, NULL, criteria, criterionNb, &logger));
        }
        WHEN("The pfw is started without an existent file") {
            REQUIRE_FAILURE(pfwStart(pfw, "/doNotExist", criteria, criterionNb, &logger));
        }

        WHEN("The pfw is started without a criteria list") {
            REQUIRE_FAILURE(pfwStart(pfw, config, NULL, criterionNb, &logger));
        }
        WHEN("The pfw is started with duplicated criterion value") {
            const PfwCriterion duplicatedCriteria[] = {
                {"duplicated name", true, letterList},
                {"duplicated name", false, numberList},
            };
            REQUIRE_FAILURE(pfwStart(pfw, config, duplicatedCriteria, 2, &logger));
        }
        WHEN("The pfw is started with duplicated criterion value state") {
            const char * values[] = {"a", "a", NULL};
            const PfwCriterion duplicatedCriteria[] = {{"name", true, values}};

            WHEN("Using test logger") {
                REQUIRE_FAILURE(pfwStart(pfw, config, duplicatedCriteria, 1, &logger));
            }
            WHEN("Using default logger") {
                // Test coverage of default logger warning
                REQUIRE_FAILURE(pfwStart(pfw, config, duplicatedCriteria, 1, NULL));
            }
        }
        WHEN("The pfw is started with NULL name criterion") {
            const PfwCriterion duplicatedCriteria[] = {{NULL, true, letterList}};
            REQUIRE_FAILURE(pfwStart(pfw, config, duplicatedCriteria, 1, &logger));
        }
        WHEN("The pfw is started with NULL criterion state list") {
            const PfwCriterion duplicatedCriteria[] = {{"name", true, NULL}};
            REQUIRE_FAILURE(pfwStart(pfw, config, duplicatedCriteria, 1, &logger));
        }
        GIVEN("A criteria with lots of values")
        {
            // Build a criterion with as many value as there is bits in int.
            std::vector<char> names(sizeof(int) * CHAR_BIT + 1, 'a');
            names.back() = '\0';
            std::vector<const char *> values(names.size());
            for(size_t i = 0; i < values.size(); ++i) {
                values[i] = &names[i];
            }
            values.back() = NULL;
            /* The pfw c api requires criterion values to be a NULL terminated
             * array of string. Each string is a pointer to a NULL terminated
             * array of char. The pfw requires each string to be different
             * from all others, ie strcmp(values[i], values[j]) != 0 for any
             * i j.
             *
             * In order to generate easily an array of different strings,
             * instantiate one string (names) big enough
             * (@see PfwCriterion::values).
             * Then instantiate an array of pointer (values),
             * each pointing to a different position in the previously
             * created string.
             *
             * Representation of the names and values vectors.
             *
             * n = number of bit in an int
             *            <--- n+1 elements --->
             * names    = |a|a|a|a|...|a|a|a|\0|
             *             ^ ^             ^
             * values[0] = ´ |             |
             * values[1] = --´             |
             * ...                         |
             * values[n - 1] =  -----------´
             * values[n] = NULL
             *
             */
            const PfwCriterion duplicatedCriteria[] = {{"name", true, &values[0]}};

            WHEN("The pfw is started with a too long criterion state list") {
                REQUIRE_FAILURE(pfwStart(pfw, config, duplicatedCriteria, 1, &logger));
            }
            WHEN("The pfw is started with max length criterion state list") {
                values[values.size() - 2] = NULL; // Hide last value
                REQUIRE_SUCCESS(pfwStart(pfw, config, duplicatedCriteria, 1, &logger));
            }
        }

        WHEN("The pfw is started with zero criteria") {
            REQUIRE_SUCCESS(pfwStart(pfw, config, criteria, 0, &logger));
        }

        WHEN("The pfw is started twice a pfw") {
            REQUIRE_SUCCESS(pfwStart(pfw, config, criteria, criterionNb, &logger));
            REQUIRE_FAILURE(pfwStart(pfw, config, criteria, criterionNb, &logger));
        }

        WHEN("The pfw is started without a logger callback") {
            PfwLogger noLog = { NULL, NULL };
            REQUIRE_SUCCESS(pfwStart(pfw, config, criteria, criterionNb, &noLog));
        }
        WHEN("The pfw is started with default logger") {
            REQUIRE_SUCCESS(pfwStart(pfw, config, criteria, criterionNb, NULL));
        }

        WHEN("Get criterion of a stopped pfw") {
            int value;
            REQUIRE_FAILURE(pfwGetCriterion(pfw, criteria[0].name, &value));
        }
        WHEN("Set criterion of a stopped pfw") {
            REQUIRE_FAILURE(pfwSetCriterion(pfw, criteria[0].name, 1));
        }
        WHEN("Commit criteria of a stopped pfw") {
            REQUIRE_FAILURE(pfwApplyConfigurations(pfw));
        }

        WHEN("Bind parameter with a stopped pfw") {
            REQUIRE(pfwBindParameter(pfw, intParameterPath) == NULL);
        }

        WHEN("The pfw is started correctly")
        {
            REQUIRE_SUCCESS(pfwStart(pfw, config, criteria, criterionNb, &logger));
            int value;

            WHEN("Get criterion without an handle") {
                REQUIRE(not pfwGetCriterion(NULL, criteria[0].name, &value));
            }
            WHEN("Get criterion without a name") {
                REQUIRE_FAILURE(pfwGetCriterion(pfw, NULL, &value));
            }
            WHEN("Get criterion without an output value") {
                REQUIRE_FAILURE(pfwGetCriterion(pfw, criteria[0].name, NULL));
            }
            WHEN("Get not existing criterion") {
                REQUIRE_FAILURE(pfwGetCriterion(pfw, "Do not exist", &value));
            }
            THEN("All criterion should value 0") {
                for(size_t i = 0; i < criterionNb; ++i) {
                    const char *criterionName = criteria[i].name;
                    CAPTURE(criterionName);
                    REQUIRE_SUCCESS(pfwGetCriterion(pfw, criterionName, &value));
                    REQUIRE(value == 0);
                }
            }

            WHEN("Set criterion without an handle") {
                REQUIRE(not pfwSetCriterion(NULL, criteria[0].name, 1));
            }
            WHEN("Set criterion without a name") {
                REQUIRE_FAILURE(pfwSetCriterion(pfw, NULL, 2));
            }
            WHEN("Set not existing criterion") {
                REQUIRE_FAILURE(pfwSetCriterion(pfw, "Do not exist", 3));
            }
            WHEN("Set criterion value") {
                for(size_t i = 0; i < criterionNb; ++i) {
                    const char *criterionName = criteria[i].name;
                    CAPTURE(criterionName);
                    REQUIRE_SUCCESS(pfwSetCriterion(pfw, criterionName, 3));
                }
                THEN("Get criterion value should return what was set") {
                    for(size_t i = 0; i < criterionNb; ++i) {
                        const char *criterionName = criteria[i].name;
                        CAPTURE(criterionName);
                        REQUIRE_SUCCESS(pfwGetCriterion(pfw, criterionName, &value));
                        REQUIRE(value == 3);
                    }
                }
            }
            WHEN("Commit criteria without a pfw") {
                REQUIRE(not pfwApplyConfigurations(NULL));
            }
            WHEN("Commit criteria of a started pfw") {
                REQUIRE_SUCCESS(pfwApplyConfigurations(pfw));
            }

            WHEN("Bind parameter without a pfw") {
                REQUIRE(pfwBindParameter(NULL, intParameterPath) == NULL);
            }
            WHEN("Bind parameter without a path") {
                REQUIRE_FAILURE(pfwBindParameter(pfw, NULL) != NULL);
            }
            WHEN("Bind a non existing parameter") {
                REQUIRE_FAILURE(pfwBindParameter(pfw, "do/not/exist") != NULL);
            }

            WHEN("Set an int parameter without a parameter handle") {
                REQUIRE(not pfwSetIntParameter(NULL, value));
            }
            WHEN("Get an int parameter without a parameter handle") {
                REQUIRE(not pfwGetIntParameter(NULL, &value));
            }

            GIVEN("An integer parameter handle") {
                PfwParameterHandler *param = pfwBindParameter(pfw, intParameterPath);
                REQUIRE_SUCCESS(param != NULL);

                WHEN("Get an int parameter without an output value") {
                    REQUIRE_FAILURE(pfwGetIntParameter(param, NULL));
                }

                WHEN("Set parameter out of range") {
                    REQUIRE_FAILURE(pfwSetIntParameter(param, 101));
                }

                WHEN("Set parameter") {
                    REQUIRE_SUCCESS(pfwSetIntParameter(param, 11));
                    THEN("Get parameter should return what was set") {
                        REQUIRE_SUCCESS(pfwGetIntParameter(param, &value));
                        REQUIRE(value == 11);
                    }
                }

                pfwUnbindParameter(param);
            }

            GIVEN("An string parameter handle") {
                PfwParameterHandler *param = pfwBindParameter(pfw, stringParameterPath);
                REQUIRE_SUCCESS(param != NULL);

                WHEN("Get an int parameter without an output value") {
                    REQUIRE_FAILURE(pfwGetStringParameter(param, NULL));
                }

                WHEN("Set parameter out of range") {
                    REQUIRE_FAILURE(pfwSetStringParameter(param, "ko_1234567"));
                }

                WHEN("Set parameter") {
                    const char *value;
                    REQUIRE_SUCCESS(pfwSetStringParameter(param, "ok"));
                    THEN("Get parameter should return what was set") {
                        REQUIRE_SUCCESS(pfwGetStringParameter(param, &value));
                        REQUIRE(value == std::string("ok"));
                        pfwFree((void *)value);
                    }
                }

                pfwUnbindParameter(param);
            }
        }

        pfwDestroy(pfw);
    }
}

SCENARIO("Get last error without a pfw") {
    THEN("Should return NULL") {
        CHECK(pfwGetLastError(NULL) == NULL);
    }
}
