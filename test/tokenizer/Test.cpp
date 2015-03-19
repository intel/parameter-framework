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

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main()
#include <catch.hpp>

#include <string>
#include <vector>

using std::string;
using std::vector;

SCENARIO("Tokenizer tests") {
    GIVEN("A default tokenizer") {

        GIVEN("A trivial string") {
            Tokenizer tokenizer("a bcd ef");

            THEN("next() api should work") {
                CHECK(tokenizer.next() == "a");
                CHECK(tokenizer.next() == "bcd");
                CHECK(tokenizer.next() == "ef");
                CHECK(tokenizer.next() == "");
            }
            THEN("split() api should work") {
                vector<string> expected;
                expected.push_back("a");
                expected.push_back("bcd");
                expected.push_back("ef");

                CHECK(tokenizer.split() == expected);
            }
        }

        GIVEN("An empty string") {
            Tokenizer tokenizer("");

            THEN("next() api should work") {
                CHECK(tokenizer.next() == "");
            }
            THEN("split() api should work") {
                vector<string> expected;

                CHECK(tokenizer.split().empty());
            }
        }

        GIVEN("A slash-separated string and tokenizer") {
            Tokenizer tokenizer("/a/bcd/ef g/h/", "/");

            THEN("next() api should work") {
                CHECK(tokenizer.next() == "a");
                CHECK(tokenizer.next() == "bcd");
                CHECK(tokenizer.next() == "ef g");
                CHECK(tokenizer.next() == "h");
                CHECK(tokenizer.next() == "");
            }
            THEN("split() api should work") {
                vector<string> expected;
                expected.push_back("a");
                expected.push_back("bcd");
                expected.push_back("ef g");
                expected.push_back("h");

                CHECK(tokenizer.split() == expected);
            }
        }

        GIVEN("Multiple separators in a row") {
            Tokenizer tokenizer("  a \n\t bc  ");

            THEN("next() api should work") {
                CHECK(tokenizer.next() == "a");
                CHECK(tokenizer.next() == "bc");
                CHECK(tokenizer.next() == "");
            }
            THEN("split() api should work") {
                vector<string> expected;
                expected.push_back("a");
                expected.push_back("bc");

                CHECK(tokenizer.split() == expected);
            }
        }
    }

}
