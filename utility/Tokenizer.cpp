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

Tokenizer::Tokenizer(const string &input, const string &delimiters)
    : _input(input), _delimiters(delimiters), _position(0)
{
}

string Tokenizer::next()
{
    string token;

    // Skip all leading delimiters
    string::size_type tokenStart = _input.find_first_not_of(_delimiters, _position);

    // Special case if there isn't any token anymore (string::substr's
    // throws when pos==npos)
    if (tokenStart == string::npos) {
        return "";
    }

    // Starting from the token's start, find the first delimiter
    string::size_type tokenEnd = _input.find_first_of(_delimiters, tokenStart);

    _position = tokenEnd;

    return _input.substr(tokenStart, tokenEnd - tokenStart);
}

vector<string> Tokenizer::split()
{
    vector<string> result;
    string token;

    while (true) {
        token = next();
        if (token.empty()) {
            return result;
        }
        result.push_back(token);
    }
}
