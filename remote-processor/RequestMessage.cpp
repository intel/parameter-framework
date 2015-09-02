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
#include <assert.h>
#include <algorithm>
#include <ctype.h>

using std::string;

const char* const CRequestMessage::gacDelimiters = " \t\n\v\f\r";

// Command Name
void CRequestMessage::setCommand(const string& strCommand)
{
    _strCommand = trim(strCommand);
}

const string& CRequestMessage::getCommand() const
{
    return _strCommand;
}

// Arguments
void CRequestMessage::addArgument(const string& strArgument)
{
    _argumentVector.push_back(trim(strArgument));
}

uint32_t CRequestMessage::getArgumentCount() const
{
    return _argumentVector.size();
}

const string& CRequestMessage::getArgument(uint32_t uiArgument) const
{
    assert(uiArgument < _argumentVector.size());

    return _argumentVector[uiArgument];
}

const string CRequestMessage::packArguments(uint32_t uiStartArgument, uint32_t uiNbArguments) const
{
    string strPackedArguments;

    assert(uiStartArgument + uiNbArguments <= _argumentVector.size());

    // Pack arguments, separating them with a space
    uint32_t uiArgument;

    bool bFirst = true;

    for (uiArgument = uiStartArgument; uiArgument < uiStartArgument + uiNbArguments; uiArgument++) {

        if (!bFirst) {

            strPackedArguments += " ";
        } else {

            bFirst = false;
        }

        strPackedArguments += _argumentVector[uiArgument];
    }

    return strPackedArguments;
}

// Fill data to send
std::vector<uint8_t> CRequestMessage::getDataToSend()
{
    std::vector<uint8_t> data;

    // Send command
    data.insert(data.end(), getCommand().begin(), getCommand().end());

    // Arguments
    uint32_t uiArgument;

    for (uiArgument = 0; uiArgument < getArgumentCount(); uiArgument++) {
        const string& arg = getArgument(uiArgument);
        data.push_back(static_cast<uint8_t>(' '));
        data.insert(data.end(), arg.begin(), arg.end());
    }
    return data;
}

// Collect received data
void CRequestMessage::processData(const std::vector<uint8_t> &data)
{
    // Receive command
    string strCommand(&data[0], &data[data.size()]);

    auto next = strCommand.find(' ');
    decltype(next) prev = 0;

    /* FIXME assert when no command name found? */
    setCommand(std::string(strCommand, prev, next));

    prev = next;
    next = strCommand.find(' ', prev + 1);

    // Arguments
    while (prev != std::string::npos) {

        addArgument(std::string(strCommand, prev, next));
        prev = next;
        next = strCommand.find(' ', prev + 1);
    }
}

// Trim input string
string CRequestMessage::trim(const string& strToTrim)
{
    // Trim string
    string strTrimmed = strToTrim;

    strTrimmed.erase(strTrimmed.find_last_not_of(gacDelimiters) + 1 );

    strTrimmed.erase(0, strTrimmed.find_first_not_of(gacDelimiters));

    return strTrimmed;
}
