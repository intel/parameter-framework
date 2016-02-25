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

#include <asio.hpp>

#include <iostream>
#include <string>
#include <cstring>
#include <stdlib.h>
#include "RequestMessage.h"
#include "AnswerMessage.h"
#include "Socket.h"

using namespace std;

bool sendAndDisplayCommand(asio::ip::tcp::socket &socket, CRequestMessage &requestMessage)
{
    string strError;

    if (requestMessage.serialize(Socket(socket), true, strError) != CRequestMessage::success) {

        cerr << "Unable to send command to target: " << strError << endl;
        return false;
    }

    ///// Get answer
    CAnswerMessage answerMessage;
    if (answerMessage.serialize(Socket(socket), false, strError) != CRequestMessage::success) {

        cerr << "Unable to received answer from target: " << strError << endl;
        return false;
    }

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

    string host{argv[1]};
    string port{argv[2]};
    try {
        asio::connect(connectionSocket, resolver.resolve(tcp::resolver::query(host, port)));
    } catch (const asio::system_error &e) {
        cerr << "Connection to '" << host << ":" << port << "' failed: " << e.what() << endl;
        return 1;
    }

    // Create command message
    CRequestMessage requestMessage(argv[3]);

    // Add arguments
    for (int arg = 4; arg < argc; arg++) {

        requestMessage.addArgument(argv[arg]);
    }

    if (!sendAndDisplayCommand(connectionSocket, requestMessage)) {
        return 1;
    }

    // Program status
    return 0;
}
