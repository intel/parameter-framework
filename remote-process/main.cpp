/* 
 * INTEL CONFIDENTIAL
 * Copyright © 2011 Intel 
 * Corporation All Rights Reserved.
 * 
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its suppliers
 * or licensors. Title to the Material remains with Intel Corporation or its
 * suppliers and licensors. The Material contains trade secrets and proprietary
 * and confidential information of Intel or its suppliers and licensors. The
 * Material is protected by worldwide copyright and trade secret laws and
 * treaty provisions. No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel’s prior express written permission.
 * 
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Intel in writing.
 * 
 * CREATED: 2011-06-01
 * UPDATED: 2011-07-27
 */
#include <iostream>
#include <stdlib.h>
#include "RequestMessage.h"
#include "AnswerMessage.h"
#include "ConnectionSocket.h"

using namespace std;

// hostname port command [argument[s]]
int main(int argc, char *argv[])
{
    // Enough args?
    if (argc < 4) {

        cerr << "Missing arguments" << endl;
        cerr << "Usage: " << argv[0] << " hostname port command [argument[s]]" << endl;

        return -1;
    }
    // Get port number
    uint16_t uiPort = (uint16_t)strtoul(argv[2], NULL, 0);

    // Connect to target
    CConnectionSocket connectionSocket;

    // Set timeout
    connectionSocket.setTimeout(5000);

    string strError;
    // Connect
    if (!connectionSocket.connect(argv[1], uiPort, strError)) {

        cerr << strError << endl;

        return -2;
    }

    // Create command message
    CRequestMessage requestMessage(argv[3]);

    // Add arguments
    uint32_t uiArg;

    for (uiArg = 4; uiArg < (uint32_t)argc; uiArg++) {

        requestMessage.addArgument(argv[uiArg]);
    }

    ///// Send command
    if (!requestMessage.serialize(&connectionSocket, true)) {

        cerr << "Unable to send command to target" << endl;

        return -3;
    }

    // Create answer message
    CAnswerMessage answerMessage;

    ///// Get answer
    if (!answerMessage.serialize(&connectionSocket, false)) {

        cerr << "No answer received from target" << endl;

        return -4;
    }

    // Success?
    if (!answerMessage.success()) {

        // Display error answer
        cerr << answerMessage.getAnswer() << endl;
    } else {

        // Display success answer
        cout << answerMessage.getAnswer() << endl;
    }

    // Program status
    return answerMessage.success() ? 0 : -5;
}
