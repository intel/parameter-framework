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

#include "Tokenizer.h"

#define CATCH_CONFIG_MAIN // This tells Catch to provide a main()
#include <catch.hpp>

#include <string>
#include <vector>

using std::string;
using std::vector;

using Expected = vector<string>;

SCENARIO("Tokenizer tests")
{
    GIVEN ("A default tokenizer") {

        GIVEN ("A trivial string") {
            Tokenizer tokenizer("a bcd ef");
            Expected expected{"a", "bcd", "ef"};

            THEN ("split() api should work") {
                CHECK(tokenizer.split() == expected);
            }
        }

        GIVEN ("An empty string") {
            Tokenizer tokenizer("");
            Expected expected{};

            THEN ("split() should be empty") {
                CHECK(tokenizer.split() == expected);
            }
        }

        GIVEN ("Multiple separators in a row") {
            Tokenizer tokenizer("  a \n\t bc  ");
            Expected expected{"a", "bc"};

            THEN ("split() api should work") {
                CHECK(tokenizer.split() == expected);
            }
        }
    }

    GIVEN ("A slash-separated string and tokenizer") {
        Tokenizer tokenizer("/a/bcd/ef g/h/", "/");
        Expected expected{"a", "bcd", "ef g", "h"};

        THEN ("split() api should work") {
            CHECK(tokenizer.split() == expected);
        }
    }

    GIVEN ("A tokenizer that doesn't merge consecutive separators") {

        GIVEN ("An empty string") {
            Tokenizer tokenizer("", Tokenizer::defaultDelimiters, false);
            Expected expected{};

            THEN ("split() should be empty") {
                CHECK(tokenizer.split() == expected);
            }
        }

        GIVEN ("A string consisting only of a delimiter") {
            Tokenizer tokenizer(",", ",", false);
            Expected expected{"", ""};

            THEN ("split() should return two empty tokens") {
                CHECK(tokenizer.split() == expected);
            }
        }

        GIVEN ("Multiple separators in a row") {
            Tokenizer tokenizer(" a  b \nc d ", Tokenizer::defaultDelimiters, false);
            Expected expected{"", "a", "", "b", "", "c", "d", ""};

            THEN ("split() api should work") {
                CHECK(tokenizer.split() == expected);
            }
        }
    }
}
