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

using std::string;
using std::vector;

const string Tokenizer::defaultDelimiters = " \n\r\t\v\f";

Tokenizer::Tokenizer(const string &input, const string &delimiters, bool mergeDelimiters)
    : _input(input), _delimiters(delimiters), _mergeDelimiters(mergeDelimiters)
{
}

vector<string> Tokenizer::split()
{
    vector<string> result;
    string token;
    bool leftover = false;

    for (const auto character : _input) {
        if (_delimiters.find(character) != string::npos) {
            if (_mergeDelimiters) {
                leftover = false;
                if (token.empty()) {
                    // skip consecutive delimiters
                    continue;
                }
            } else {
                // We've encountered a delimiter, which means that there is a
                // left-hand token and a right-side token. We are going to add
                // the left-hand one but must not forget that there is a
                // right-hand one (possibly empty)
                leftover = true;
            }

            result.push_back(token);
            token.clear();
            continue;
        }
        token += character;
        leftover = true;
    }

    // push any leftover token:
    if (leftover) {
        result.push_back(token);
    }

    return result;
}
