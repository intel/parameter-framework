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

Tokenizer::Tokenizer(const string &input, const string &delimiters, bool skipEmpty)
    : _input(input), _delimiters(delimiters), _skipEmpty(skipEmpty), _position(0)
{
    // Deal with strings with no token
    if (_skipEmpty) {
        _position = _input.find_first_not_of(_delimiters);
    }
}

string Tokenizer::next()
{
    if (done()) {
        return "";
    }

    auto tokenStart = _position;
    _position = _input.find_first_of(_delimiters, tokenStart);
    string::size_type count;

    // We consumed all of the input
    if (done()) {
        count = string::npos;
    } else {
        count = _position - tokenStart;
        _position++;
        // Deal with future empty tokens (so that the user can reliably call done())
        if (_skipEmpty) {
           _position = _input.find_first_not_of(_delimiters, _position);
        }
    }

    return _input.substr(tokenStart, count);
}

vector<string> Tokenizer::split()
{
    vector<string> result;
    string token;

    while (not done()) {
        token = next();
        result.push_back(token);
    }

    return result;
}

bool Tokenizer::done()
{
    return _position > _input.size();
}
