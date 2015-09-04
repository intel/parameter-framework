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
#include <iostream>
#include <string>
#include <cstring>
#include <stdlib.h>
#include "RequestMessage.h"
#include "AnswerMessage.h"

using namespace std;

bool sendAndDisplayCommand(asio::ip::tcp::socket &socket, CRequestMessage &requestMessage)
{
    string strError;

    CMessage service(socket);

    std::vector<uint8_t> payload = requestMessage.serialize();

    auto res = service.send(payload);
    if (res.first != CMessage::Code::success) {

        cerr << "Unable to send command to target: " << res.second << endl;
        return false;
    }

    ///// Get answer
    res = service.recv(payload);
    if (res.first != CMessage::Code::success) {

        cerr << "Unable to received answer from target: " << res.second << endl;
        return false;
    }

    CAnswerMessage answerMessage;

    answerMessage.deserialize(payload);

    // Success?
    if (!answerMessage.success()) {

        // Display error answer
        cerr << answerMessage.getAnswer() << endl;
        return false;
    }

    // Display success answer
    cout << answerMessage.getAnswer() << endl;

    return true;
}

// hostname port command [argument[s]]
// or
// hostname port < commands
int main(int argc, char *argv[])
{
    // Enough args?
    if (argc < 4) {

        cerr << "Missing arguments" << endl;
        cerr << "Usage: " << endl;
        cerr << "Send a single command:" << endl;
        cerr << "\t" << argv[0] << " hostname port command [argument[s]]" << endl;

        return 1;
    }
    using asio::ip::tcp;
    asio::io_service io_service;
    tcp::resolver resolver(io_service);

    tcp::socket connectionSocket(io_service);

    asio::error_code ec;
    asio::connect(connectionSocket, resolver.resolve(tcp::resolver::query(argv[1], (argv[2]))), ec);

    if (ec) {

        cerr << "Connexion failed: " << ec.message() << endl;

        return 1;
    }

    // Create command message
    CRequestMessage requestMessage(argv[3]);

    // Add arguments
    uint32_t uiArg;
    for (uiArg = 4; uiArg < (uint32_t)argc; uiArg++) {

        requestMessage.addArgument(argv[uiArg]);
    }

    if (!sendAndDisplayCommand(connectionSocket, requestMessage)) {
        return 1;
    }

    // Program status
    return 0;
}
