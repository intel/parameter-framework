/* <auto_header>
 * <FILENAME>
 * 
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
 *  AUTHOR: Patrick Benavoli (patrickx.benavoli@intel.com)
 * CREATED: 2011-06-01
 * UPDATED: 2011-07-27
 * 
 * 
 * </auto_header>
 */
#include "RemoteProcessorServer.h"
#include "ListeningSocket.h"
#include <assert.h>
#include <poll.h>
#include <unistd.h>
#include <strings.h>
#include "RequestMessage.h"
#include "AnswerMessage.h"
#include "RemoteCommandHandler.h"

CRemoteProcessorServer::CRemoteProcessorServer(uint16_t uiPort, IRemoteCommandHandler* pCommandHandler) :
    _uiPort(uiPort), _pCommandHandler(pCommandHandler), _bIsStarted(false), _pListeningSocket(NULL), _ulThreadId(0)
{
    // Create inband pipe
    pipe(_aiInbandPipe);
}

CRemoteProcessorServer::~CRemoteProcessorServer()
{
    stop();
}

// State
bool CRemoteProcessorServer::start()
{
    assert(!_bIsStarted);

    // Create server socket
    _pListeningSocket = new CListeningSocket;

    if (!_pListeningSocket->listen(_uiPort)) {

        // Remove listening socket
        delete _pListeningSocket;
        _pListeningSocket = NULL;

        return false;
    }

    // Create thread
    pthread_create(&_ulThreadId, NULL, thread_func, this);

    // State
    _bIsStarted = true;

    return true;
}

void CRemoteProcessorServer::stop()
{
    // Check state
    if (!_bIsStarted) {

        return;
    }

    // Cause exiting of the thread
    uint8_t ucData = 0;
    write(_aiInbandPipe[1], &ucData, sizeof(ucData));

    // Join thread
    pthread_join(_ulThreadId, NULL);

    _bIsStarted = false;

    // Remove listening socket
    delete _pListeningSocket;
    _pListeningSocket = NULL;
}

bool CRemoteProcessorServer::isStarted() const
{
    return _bIsStarted;
}

// Thread
void* CRemoteProcessorServer::thread_func(void* pData)
{
    reinterpret_cast<CRemoteProcessorServer*>(pData)->run();

    return NULL;
}

void CRemoteProcessorServer::run()
{
    struct pollfd _aPollFds[2];

    bzero(_aPollFds, sizeof(_aPollFds));

    // Build poll elements
    _aPollFds[0].fd = _pListeningSocket->getFd();
    _aPollFds[1].fd = _aiInbandPipe[0];
    _aPollFds[0].events = POLLIN;
    _aPollFds[1].events = POLLIN;

    while (true) {

        poll(_aPollFds, 2, -1);

        if (_aPollFds[0].revents & POLLIN) {

            // New incoming connection
            handleNewConnection();
        }
        if (_aPollFds[1].revents & POLLIN) {

            // Consume exit request
            uint8_t ucData;
            read(_aiInbandPipe[0], &ucData, sizeof(ucData));

            // Exit
            return;
        }
    }
}

// New connection
void CRemoteProcessorServer::handleNewConnection()
{
    CSocket* pClientSocket = _pListeningSocket->accept();

    if (!pClientSocket) {

        return;
    }

    // Set timeout
    pClientSocket->setTimeout(5000);

    // Process all incoming requests from the client
    while (true) {

        // Process requests
        // Create command message
        CRequestMessage requestMessage;

        ///// Receive command
        if (!requestMessage.serialize(pClientSocket, false)) {

            // Bail out
            break;
        }

        // Actually process the request
        bool bSuccess;

        string strResult;

        if (_pCommandHandler) {

            bSuccess = _pCommandHandler->remoteCommandProcess(requestMessage, strResult);

        } else {

            strResult = "No handler!";

            bSuccess = false;
        }

        // Send back answer
        // Create answer message
        CAnswerMessage answerMessage(strResult, bSuccess);

        ///// Send answer
        if (!answerMessage.serialize(pClientSocket, true)) {

            // Bail out
            break;
        }
    }
    // Remove client socket
    delete pClientSocket;
}
