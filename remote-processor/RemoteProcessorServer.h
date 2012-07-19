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
#pragma once

#include <stdint.h>
#include <pthread.h>
#include "RemoteProcessorServerInterface.h"

class CListeningSocket;
class IRemoteCommandHandler;

class CRemoteProcessorServer : public IRemoteProcessorServerInterface
{
public:
    CRemoteProcessorServer(uint16_t uiPort, IRemoteCommandHandler* pCommandHandler);
    virtual ~CRemoteProcessorServer();

    // State
    virtual bool start();
    virtual void stop();
    virtual bool isStarted() const;

private:
    // Thread
    static void* thread_func(void* pData);
    void run();

    // New connection
    void handleNewConnection();

    // Port number
    uint16_t _uiPort;
    // Command handler
    IRemoteCommandHandler* _pCommandHandler;
    // State
    bool _bIsStarted;
    // Listening socket
    CListeningSocket* _pListeningSocket;
    // Inband pipe
    int _aiInbandPipe[2];
    // Thread
    pthread_t _ulThreadId;
};

