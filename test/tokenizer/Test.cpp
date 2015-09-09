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
    GIVEN("A trivial string") {
        string input = "a bcd ef";
        const vector<string> expected = {"a", "bcd", "ef"};

        GIVEN("A skip-empty tokenizer") {
            Tokenizer skipTokenizer(input);

            THEN("next() api should work") {
                for (auto &token : expected) {
                    CHECK(skipTokenizer.next() == token);
                }
                CHECK(skipTokenizer.done());
                CHECK(skipTokenizer.next() == "");
            }
            THEN("split() api should work") {
                CHECK(skipTokenizer.split() == expected);
            }

            THEN("split() after next() should return the remaining tokens") {
                auto head = expected.front();
                auto tail = vector<string>(expected.begin() + 1, expected.end());
                CHECK(skipTokenizer.next() == expected[0]);
                CHECK(skipTokenizer.split() == tail);
            }
        }

        GIVEN("A no-skip-empty tokenizer") {
            Tokenizer noSkipTokenizer(input, Tokenizer::defaultDelimiters, false);

            THEN("next() api should work") {
                for (auto &token : expected) {
                    CHECK(noSkipTokenizer.next() == token);
                }
                CHECK(noSkipTokenizer.done());
                CHECK(noSkipTokenizer.next() == "");
            }
            THEN("split() api should work") {
                CHECK(noSkipTokenizer.split() == expected);
            }
        }
    }

    GIVEN("A zero-delimited string") {
        Tokenizer tokenizer(string("a\0bcd\0ef", 8), string("\0", 1));
        const vector<string> expected = {"a", "bcd", "ef"};

        THEN("next() api should work") {
            for (auto &token : expected) {
                CHECK(tokenizer.next() == token);
            }
            CHECK(tokenizer.done());
        }
        THEN("split() api should work") {
            CHECK(tokenizer.split() == expected);
        }
    }

    GIVEN("An empty string") {
        Tokenizer skipTokenizer("");
        Tokenizer noSkipTokenizer("", "ignored", false);

        THEN("The skip-empty tokenizer should find no token") {
            CHECK(skipTokenizer.done());
        }
        THEN("The no-skip-empty tokenizer should find a token") {
            CHECK(not noSkipTokenizer.done());
            CHECK(noSkipTokenizer.next() == "");
        }
    }

    GIVEN("A slash-separated string and tokenizer") {
        Tokenizer tokenizer("/a/bcd/ef g/h/", "/");
        const vector<string> expected = {"a", "bcd", "ef g", "h"};

        THEN("next() api should work") {
            for (auto &token : expected) {
                CHECK(tokenizer.next() == token);
            }
            CHECK(tokenizer.done());
        }
        THEN("split() api should work") {
            CHECK(tokenizer.split() == expected);
        }
    }

    GIVEN("Multiple separators in a row - skip empties") {
        string input = "  a \n\t bc  ";
        const vector<string> expected = {"a", "bc"};
        Tokenizer tokenizer(input);

        THEN("next() api should work") {
            for (auto &token : expected) {
                CHECK(tokenizer.next() == token);
            }
            CHECK(tokenizer.done());
        }
        THEN("split() api should work") {
            CHECK(tokenizer.split() == expected);
        }
    }

    GIVEN("Multiple separators in a row - don't skip empties") {
        string input = "  a \n\t bc  ";
        vector<string> expected = {"", "", "a", "", "", "", "bc", "", ""};
        Tokenizer tokenizer(input, Tokenizer::defaultDelimiters, false);

        THEN("next() api should work") {
            for (auto &token : expected) {
                CHECK(tokenizer.next() == token);
            }
            CHECK(tokenizer.done());
        }
        THEN("split() api should work") {
            vector<string> expected = {"", "", "a", "", "", "", "bc", "", ""};

            CHECK(tokenizer.split() == expected);
        }
    }
}
