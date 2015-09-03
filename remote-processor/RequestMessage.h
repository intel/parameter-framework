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
#pragma once

#include "Message.h"
#include "RemoteCommand.h"
#include <vector>
#include <string>

class CRequestMessage : public CMessage, public IRemoteCommand
{
public:
    CRequestMessage(const std::string& strCommand) : _strCommand(strCommand) {}
    CRequestMessage() {}

    // Command Name
    void setCommand(const std::string& strCommand);
    virtual const std::string& getCommand() const;

    // Arguments
    virtual void addArgument(const std::string& strArgument);
    virtual uint32_t getArgumentCount() const;
    virtual const std::string& getArgument(uint32_t uiArgument) const;
    virtual const std::string packArguments(uint32_t uiStartArgument, uint32_t uiNbArguments) const;

private:
    virtual std::vector<uint8_t> getDataToSend();
    virtual void processData(const std::vector<uint8_t> &data);

    // Command
    std::string _strCommand;
    // Arguments
    std::vector<std::string> _argumentVector;
};

