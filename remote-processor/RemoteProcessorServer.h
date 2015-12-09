/*
 * Copyright (c) 2011-2015, Intel Corporation
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

#include "remote_processor_export.h"

#include <stdint.h>
#include "RemoteProcessorServerInterface.h"
#include <asio.hpp>

class IRemoteCommandHandler;

class REMOTE_PROCESSOR_EXPORT CRemoteProcessorServer : public IRemoteProcessorServerInterface
{
public:
    CRemoteProcessorServer(uint16_t uiPort);
    virtual ~CRemoteProcessorServer();

    // State
    virtual bool start(std::string &error);
    virtual bool stop();
    bool process(IRemoteCommandHandler &commandHandler);

private:
    void acceptRegister(IRemoteCommandHandler &commandHandler);

    // New connection
    void handleNewConnection(IRemoteCommandHandler &commandHandler);

    // Port number
    uint16_t _uiPort;

    asio::io_service _io_service;
    asio::ip::tcp::acceptor _acceptor;
    asio::ip::tcp::socket _socket;
};
