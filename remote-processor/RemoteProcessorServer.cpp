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
#include "RemoteProcessorServer.h"
#include <iostream>
#include <memory>
#include <assert.h>
#include <string.h>
#include "RequestMessage.h"
#include "AnswerMessage.h"
#include "RemoteCommandHandler.h"
#include "Socket.h"

using std::string;

CRemoteProcessorServer::CRemoteProcessorServer(uint16_t uiPort)
    : _uiPort(uiPort), _io_service(), _acceptor(_io_service), _socket(_io_service)
{
}

CRemoteProcessorServer::~CRemoteProcessorServer()
{
    stop();
}

// State
bool CRemoteProcessorServer::start(string &error)
{
    using namespace asio;

    try {
        ip::tcp::endpoint endpoint(ip::tcp::v6(), _uiPort);

        _acceptor.open(endpoint.protocol());

        _acceptor.set_option(ip::tcp::acceptor::reuse_address(true));
        _acceptor.set_option(asio::socket_base::linger(true, 0));
        _acceptor.set_option(socket_base::enable_connection_aborted(true));

        _acceptor.bind(endpoint);
        _acceptor.listen();
    } catch (std::exception &e) {
        error = "Unable to listen on port " + std::to_string(_uiPort) + ": " + e.what();
        return false;
    }

    return true;
}

bool CRemoteProcessorServer::stop()
{
    _io_service.stop();

    return true;
}

void CRemoteProcessorServer::acceptRegister(IRemoteCommandHandler &commandHandler)
{
    auto peerHandler = [this, &commandHandler](asio::error_code ec) {
        if (ec) {
            std::cerr << "Accept failed: " << ec.message() << std::endl;
            return;
        }

        _socket.set_option(asio::ip::tcp::no_delay(true));
        handleNewConnection(commandHandler);

        _socket.close();

        acceptRegister(commandHandler);
    };

    _acceptor.async_accept(_socket, peerHandler);
}

bool CRemoteProcessorServer::process(IRemoteCommandHandler &commandHandler)
{
    acceptRegister(commandHandler);

    asio::error_code ec;

    _io_service.run(ec);

    if (ec) {
        std::cerr << "Server failed: " << ec.message() << std::endl;
    }

    return ec.value() == 0;
}

// New connection
void CRemoteProcessorServer::handleNewConnection(IRemoteCommandHandler &commandHandler)
{
    // Process all incoming requests from the client
    while (true) {

        // Process requests
        // Create command message
        CRequestMessage requestMessage;

        string strError;
        ///// Receive command
        CRequestMessage::Result res;
        res = requestMessage.serialize(Socket(_socket), false, strError);

        switch (res) {
        case CRequestMessage::error:
            std::cout << "Error while receiving message: " << strError << std::endl;
        // fall through
        case CRequestMessage::peerDisconnected:
            // Consider peer disconnection as normal, no log
            return; // Bail out
        case CRequestMessage::success:
            break; // No error, continue
        }

        // Actually process the request
        bool bSuccess;

        string strResult;

        bSuccess = commandHandler.remoteCommandProcess(requestMessage, strResult);

        // Send back answer
        // Create answer message
        CAnswerMessage answerMessage(strResult, bSuccess);

        ///// Send answer
        res = answerMessage.serialize(_socket, true, strError);

        switch (res) {
        case CRequestMessage::peerDisconnected:
        // Peer should not disconnect while waiting for an answer
        // Fall through to log the error and bail out
        case CRequestMessage::error:
            std::cout << "Error while receiving message: " << strError << std::endl;
            return; // Bail out
        case CRequestMessage::success:
            break; // No error, continue
        }
    }
}
