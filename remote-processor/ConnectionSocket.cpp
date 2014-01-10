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
#include "ConnectionSocket.h"
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>
#include <sstream>

#define base CSocket

CConnectionSocket::CConnectionSocket()
{
}

// Connection
bool CConnectionSocket::connect(const string& strRemote, uint16_t uiPort, string& strError)
{
    struct sockaddr_in server_addr;

    // Host entry
    struct hostent* host = gethostbyname(strRemote.c_str());

    // Check host
    if (!host) {

        strError = "Target not found :-(";

        return false;
    }

    // Fill server address
    initSockAddrIn(&server_addr, *((uint32_t*)host->h_addr), uiPort);

    // Connect
    if (::connect(getFd(), (struct sockaddr *)&server_addr, sizeof(struct sockaddr))) {

        ostringstream oss;
        oss << "CConnectionSocket::connect::connect on port: " << uiPort;
        perror(oss.str().c_str());

        strError = "Unable to connnect to target :-(";

        return false;
    }
    return true;
}
