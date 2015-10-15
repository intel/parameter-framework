/*
 * Copyright (c) 2011-2014, Intel Corporation
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

#include "Utility.h"

#include <catch.hpp>
#include <functional>
#include <map>

using std::list;
using std::string;

namespace utility
{

SCENARIO("join<int>") {
    struct Test
    {
        list<int> input;
        std::function<int (int, int)> binaryOpt;
        int empty;
        int result;
        int resultNoEmpty;
    };
    const list<Test> tests =
    {
        {{},           nullptr,                          21, 21,  0},
        {{5},          nullptr,                          -1, 5,   5},
        {{5, 2},       [](int, int){return 73;},         -1, 73,  73},
        {{2, 3, 7},    [](int l, int r){return l * r ;}, -1, 42,  42},
        {{1, 10, 100}, [](int l, int r){return l + r ;}, -1, 111, 111}
    };
    for (auto &test : tests) {
        CAPTURE(Catch::toString(test.input));
        const auto &first = begin(test.input);
        const auto &last = end(test.input);
        REQUIRE(join(first, last, test.binaryOpt, test.empty) == test.result);
        REQUIRE(join<int>(first, last, test.binaryOpt) == test.resultNoEmpty);
    }
}


SCENARIO("asString(list)") {
    struct Test
    {
        string title;
        list<string> input;
        string separator;
        string result;
        string resultNoSep;
    };
    const list<Test> tests =
    {
        {"Empty list",      {},                  "aa", "",        ""},
        {"One element",     {"a"},               "<>", "a",       "a"},
        {"Three elem list", {"1", "2", "3"},     "**", "1**2**3", "1\n2\n3"},
        {"No separator",    {"12", "ab", "+-"},  "",   "12ab+-",  "12\nab\n+-"},
        {"empty elem list", {"a", "b", "", "d"}, "|",  "a|b||d",  "a\nb\n\nd"},
    };
    for (auto &test : tests) {
        CAPTURE(Catch::toString(test.input));
        WHEN("Separator, " + test.title) {
            CAPTURE(test.separator);
            REQUIRE(asString(test.input, test.separator) == test.result);
        }
        THEN("No separator, " + test.title) {
            REQUIRE(asString(test.input) == test.resultNoSep);
        }
    }
}

SCENARIO("asString(map)") {
    using std::map;

    using Map = map<string, string>;
    struct Test
    {
        Map input;
        string itemSep;
        string keyValueSep;
        string result;
        string resultNoKeyValueSep;
        string resultNoSep;
    };
    const list<Test> tests =
    {
        {{}, "itemSep", "keyValueSep", "", "", ""},
        { Map{{"a", "b"},
              {"c", "d"},
              {"e", "f"}}, // input
          " - ", "\n", // item & keyValue sep
          "a - b\nc - d\ne - f", //result
          "a:b\nc:d\ne:f", //resultNoKeyValueSep
          "a:b, c:d, e:f" //resultNoSep
        }
    };
    for (const auto &test : tests) {
        CAPTURE(Catch::toString(test.input));
        CAPTURE(test.keyValueSep);
        CAPTURE(test.itemSep);
        REQUIRE(asString(test.input, test.keyValueSep, test.itemSep) == test.result);
        REQUIRE(asString(test.input, test.keyValueSep) == test.resultNoKeyValueSep);
        REQUIRE(asString(test.input) == test.resultNoSep);
    }

}

SCENARIO("appendTitle") {
    struct Test
    {
        string initial;
        string title;
        string result;
    };
    const list<Test> tests =
    {
        {"",      "abc",   "\nabc\n===\n"},
        {"start", "title", "start\ntitle\n=====\n"}
    };
    for (auto &test : tests) {
        auto quote = [](std::string toQuote) { return '"' + toQuote + '"'; };

        GIVEN("A title: " + quote(test.title)) {
            CAPTURE(test.initial);
            CAPTURE(test.title);

            WHEN("Appending to: " + quote(test.initial)) {
                string output = test.initial;
                THEN("Result should be:\n" + quote(test.result)) {
                    appendTitle(output, test.title);
                    CHECK(output == test.result);
                }
            }
        }
    }
}

SCENARIO("isNotHexadecimal") {
    for (auto &str : {"a", "0", "012", "13", "ABC", "Oxa"}) {
        CAPTURE(str);
        CHECK(not isHexadecimal(str));
    }
}

SCENARIO("isHexadecimal") {
    for (auto str : {"0xa", "0X0", "0x012", "0x13", "0xConsider as hexa as starting with 0x"}) {
        CAPTURE(str);
        CHECK(isHexadecimal(str));
    }
}

} // namespace utility
