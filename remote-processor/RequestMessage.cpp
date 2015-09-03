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
#include "RequestMessage.h"
#include "Tokenizer.h"
#include <assert.h>
#include <algorithm>
#include <ctype.h>

using std::string;

// Command Name
void CRequestMessage::setCommand(const string& strCommand)
{
    _strCommand = strCommand;
}

const string& CRequestMessage::getCommand() const
{
    return _strCommand;
}

// Arguments
void CRequestMessage::addArgument(const string& strArgument)
{
    _arguments.push_back(strArgument);
}

uint32_t CRequestMessage::getArgumentCount() const
{
    return _arguments.size();
}

const string& CRequestMessage::getArgument(uint32_t uiArgument) const
{
    assert(uiArgument < _arguments.size());

    return _arguments[uiArgument];
}

const string CRequestMessage::packArguments(uint32_t uiStartArgument, uint32_t uiNbArguments) const
{
    string strPackedArguments;

    assert(uiStartArgument + uiNbArguments <= _arguments.size());

    // Pack arguments, separating them with a space
    uint32_t uiArgument;

    bool bFirst = true;

    for (uiArgument = uiStartArgument; uiArgument < uiStartArgument + uiNbArguments; uiArgument++) {

        if (!bFirst) {

            strPackedArguments += " ";
        } else {

            bFirst = false;
        }

        strPackedArguments += _arguments[uiArgument];
    }

    return strPackedArguments;
}

// Fill data to send
std::vector<uint8_t> CRequestMessage::getDataToSend()
{
    std::vector<uint8_t> data;

    // Send command
    data.insert(data.end(), getCommand().begin(), getCommand().end());

    for (const auto &arg: _arguments) {
        /* Add a separator */
        data.push_back(static_cast<uint8_t>('\0'));

        data.insert(data.end(), arg.begin(), arg.end());
    }
    return data;
}

// Collect received data
void CRequestMessage::processData(const std::vector<uint8_t> &data)
{
    // Receive command
    string strCommand(&data[0], &data[data.size()]);

    Tokenizer tok(strCommand, std::string("\0", 1), false);

    setCommand(tok.next());

    // Arguments
    for (auto &arg : tok.split()) {
        addArgument(arg);
    }
}

