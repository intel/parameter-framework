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
#pragma once

#include <string>
#include <vector>

/** Tokenizer class
 *
 * Must be initialized with a string to be tokenized and, optionally, a string
 * of delimiters (@see Tokenizer::defaultDelimiters).
 *
 * Multiple consecutive delimiters (even if different) are considered as a
 * single one. As a result, there can't be empty tokens.
 */
class Tokenizer
{
public:
    /** Constructs a Tokenizer
     *
     * @param[in] input The string to be tokenized
     * @param[in] delimiters A string containing all the token delimiters
     *            (hence, each delimiter can only be a single character)
     */
    Tokenizer(const std::string &input, const std::string &delimiters=defaultDelimiters);
    ~Tokenizer() {};

    /** Return the next token or an empty string if no more token
     *
     * Multiple consecutive delimiters are considered as a single one - i.e.
     * "a     bc d   " will be tokenized as ("a", "bc", "d") if the delimiter
     * is ' '.
     */
    std::string next();

    /** Return a vector of all tokens
     */
    std::vector<std::string> split();

    /** Default list of delimiters (" \n\r\t\v\f") */
    static const std::string defaultDelimiters;

private:
    const std::string _input; //< string to be tokenized
    const std::string _delimiters; //< token delimiters

    std::string::size_type _position; //< end of the last returned token
};
