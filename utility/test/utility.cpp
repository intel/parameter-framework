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
#include "BinaryCopy.hpp"

#include <catch.hpp>
#include <functional>
#include <map>

using std::list;
using std::string;

namespace utility
{

SCENARIO("join<int>")
{
    struct Test
    {
        list<int> input;
        std::function<int(int, int)> binaryOpt;
        int empty;
        int result;
        int resultNoEmpty;
    };
    const list<Test> tests = {{{}, nullptr, 21, 21, 0},
                              {{5}, nullptr, -1, 5, 5},
                              {{5, 2}, [](int, int) { return 73; }, -1, 73, 73},
                              {{2, 3, 7}, [](int l, int r) { return l * r; }, -1, 42, 42},
                              {{1, 10, 100}, [](int l, int r) { return l + r; }, -1, 111, 111}};
    for (auto &test : tests) {
        CAPTURE(Catch::toString(test.input));
        const auto &first = begin(test.input);
        const auto &last = end(test.input);
        REQUIRE(join(first, last, test.binaryOpt, test.empty) == test.result);
        REQUIRE(join<int>(first, last, test.binaryOpt) == test.resultNoEmpty);
    }
}

SCENARIO("asString(list)")
{
    struct Test
    {
        string title;
        list<string> input;
        string separator;
        string result;
        string resultNoSep;
    };
    const list<Test> tests = {
        {"Empty list", {}, "aa", "", ""},
        {"One element", {"a"}, "<>", "a", "a"},
        {"Three elem list", {"1", "2", "3"}, "**", "1**2**3", "1\n2\n3"},
        {"No separator", {"12", "ab", "+-"}, "", "12ab+-", "12\nab\n+-"},
        {"empty elem list", {"a", "b", "", "d"}, "|", "a|b||d", "a\nb\n\nd"},
    };
    for (auto &test : tests) {
        CAPTURE(Catch::toString(test.input));
        WHEN ("Separator, " + test.title) {
            CAPTURE(test.separator);
            REQUIRE(asString(test.input, test.separator) == test.result);
        }
        THEN ("No separator, " + test.title) {
            REQUIRE(asString(test.input) == test.resultNoSep);
        }
    }
}

SCENARIO("asString(map)")
{
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
    const list<Test> tests = {{{}, "itemSep", "keyValueSep", "", "", ""},
                              {
                                  Map{{"a", "b"}, {"c", "d"}, {"e", "f"}}, // input
                                  " - ", "\n",                             // item & keyValue sep
                                  "a - b\nc - d\ne - f",                   // result
                                  "a:b\nc:d\ne:f",                         // resultNoKeyValueSep
                                  "a:b, c:d, e:f"                          // resultNoSep
                              }};
    for (const auto &test : tests) {
        CAPTURE(Catch::toString(test.input));
        CAPTURE(test.keyValueSep);
        CAPTURE(test.itemSep);
        REQUIRE(asString(test.input, test.keyValueSep, test.itemSep) == test.result);
        REQUIRE(asString(test.input, test.keyValueSep) == test.resultNoKeyValueSep);
        REQUIRE(asString(test.input) == test.resultNoSep);
    }
}

SCENARIO("appendTitle")
{
    struct Test
    {
        string initial;
        string title;
        string result;
    };
    const list<Test> tests = {{"", "abc", "\nabc\n===\n"},
                              {"start", "title", "start\ntitle\n=====\n"}};
    for (auto &test : tests) {
        auto quote = [](std::string toQuote) { return '"' + toQuote + '"'; };

        GIVEN ("A title: " + quote(test.title)) {
            CAPTURE(test.initial);
            CAPTURE(test.title);

            WHEN ("Appending to: " + quote(test.initial)) {
                string output = test.initial;
                THEN ("Result should be:\n" + quote(test.result)) {
                    appendTitle(output, test.title);
                    CHECK(output == test.result);
                }
            }
        }
    }
}

SCENARIO("isNotHexadecimal")
{
    for (auto &str : {"a", "0", "012", "13", "ABC", "Oxa"}) {
        CAPTURE(str);
        CHECK(not isHexadecimal(str));
    }
}

SCENARIO("isHexadecimal")
{
    for (auto str : {"0xa", "0X0", "0x012", "0x13", "0xConsider as hexa as starting with 0x"}) {
        CAPTURE(str);
        CHECK(isHexadecimal(str));
    }
}

template <class T1, class T2>
void checkBinaryEqual(T1 v1, T2 v2)
{
    // For some yet-unknown reason, GCC 4.8 complains about
    //     CHECK(a == b);
    // and suggests that parentheses should be added. This is related to catch
    // internals but such construcuts have been used without problem in lots of
    // other places...
    // Besides, GCC 4.9 does not seem to have a problem with that either.
    // As a workaround, captures variables and parenthesize the expressions.

    auto v2AsT1 = utility::binaryCopy<T1>(v2);
    CAPTURE(v1);
    CAPTURE(v2AsT1);
    CHECK((v1 == v2AsT1));

    auto v1AsT2 = utility::binaryCopy<T2>(v1);
    CAPTURE(v2);
    CAPTURE(v1AsT2);
    CHECK((v2 == v1AsT2));
}

SCENARIO("binaryCopy bit exactness")
{
    GIVEN ("Integer representations computed using http://babbage.cs.qc.cuny.edu/IEEE-754/") {

        THEN ("Floats should be coded on 32bits and fulfill IEEE-754."
              " That assumption is made in the Parameter Framework.") {
            REQUIRE(sizeof(float) == sizeof(uint32_t));
            REQUIRE(std::numeric_limits<float>::is_iec559);
        }
        WHEN ("Testing float <=> uint32_t conversion") {
            checkBinaryEqual<float, uint32_t>(1.23456f, 0x3f9e0610);
        }

        THEN ("Doubles should be coded on 64bits and fulfill IEEE-754."
              " That assumption is made in the Parameter Framework.") {
            REQUIRE(sizeof(double) == sizeof(uint64_t));
            REQUIRE(std::numeric_limits<double>::is_iec559);
        }
        WHEN ("Testing double <=> uint64_t conversion") {
            checkBinaryEqual<double, uint64_t>(987.65432109876, 0x408edd3c0cb3420e);
        }
    }

    WHEN ("Testing int8_t <=> uint8_t conversion") {
        checkBinaryEqual<int8_t, uint8_t>(-1, 0xff);
    }
}

} // namespace utility
