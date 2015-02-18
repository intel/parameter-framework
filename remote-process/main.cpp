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
#include "ConnectionSocket.h"
#include "NaiveTokenizer.h"

using namespace std;

class CRequestMessageGenerator
{
private:
    istream& _input; // File to read the commands from

public:
    CRequestMessageGenerator(istream& input) : _input(input) {}

    enum EStatus {
        OK,
        STOP,
        EMPTY_LINE,
        ERROR
    };

    EStatus next(CRequestMessage& requestMessage)
    {
        string sLine;
        char* pcLine;
        char* pcLine_backup; // pcLine will be modified by NaiveTokenizer
                             // so we need to keep track of its original value
        char* pcToken;

        // Read a single line from the input file
        getline(_input, sLine);
        if (_input.eof() && (_input.gcount() == 0)) {
            return STOP; // No more commands
        }
        if (_input.fail()) {
            return ERROR; // Error while reading file
        }

        pcLine = strdup(sLine.c_str());
        pcLine_backup = pcLine;
        if (!pcLine) {
            return ERROR;
        }

        // Set the first word as the command
        pcToken = NaiveTokenizer::getNextToken(&pcLine);
        if (!pcToken) {
            free(pcLine_backup);
            return EMPTY_LINE;
        }
        requestMessage.setCommand(pcToken);

        while ((pcToken = NaiveTokenizer::getNextToken(&pcLine)) != NULL) {

            // Add each word as arguments to the command
            requestMessage.addArgument(pcToken);
        }

        free(pcLine_backup);

        return OK;
    }
};

bool sendAndDisplayCommand(CConnectionSocket &connectionSocket, CRequestMessage &requestMessage)
{
    string strError;

    if (requestMessage.serialize(&connectionSocket, true, strError)
            != CRequestMessage::success) {

        cerr << "Unable to send command to target: " << strError << endl;
        return false;
    }

    ///// Get answer
    CAnswerMessage answerMessage;
    if (answerMessage.serialize(&connectionSocket, false, strError)
            != CRequestMessage::success) {

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
    bool bFromStdin = false; // Read commands from stdin instead of arguments

    // Enough args?
    if (argc < 3) {

        cerr << "Missing arguments" << endl;
        cerr << "Usage: " << endl;
        cerr << "Send a single command:" << endl;
        cerr << "\t" << argv[0] << " hostname port command [argument[s]]" << endl;
        cerr << "Send several commands, read from stdin:" << endl;
        cerr << "\t" << argv[0] << " hostname port" << endl;

        return 1;
    } else if (argc < 4) {
        bFromStdin = true;
    }
    // Get port number
    uint16_t uiPort = (uint16_t)strtoul(argv[2], NULL, 0);

    // Connect to target
    CConnectionSocket connectionSocket;

    string strError;
    // Connect
    if (!connectionSocket.connect(argv[1], uiPort, strError)) {

        cerr << strError << endl;

        return 1;
    }

    if (bFromStdin) {

        CRequestMessageGenerator generator(cin);
        CRequestMessage requestMessage;
        CRequestMessageGenerator::EStatus status;

        while (true) {
            status = generator.next(requestMessage);

            switch (status) {
            case CRequestMessageGenerator::OK:
                if (!sendAndDisplayCommand(connectionSocket, requestMessage)) {
                    return 1;
                }
                break;
            case CRequestMessageGenerator::STOP:
                return 0;
            case CRequestMessageGenerator::ERROR:
                cerr << "Error while reading the input" << endl;
                return 1;
            case CRequestMessageGenerator::EMPTY_LINE:
                continue;
            }
        }
    } else {
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
    }

    // Program status
    return 0;
}
