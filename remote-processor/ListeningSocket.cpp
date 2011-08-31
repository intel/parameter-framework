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
#include "ListeningSocket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <assert.h>
#include <netdb.h>
#include <strings.h>

#include <stdio.h>
#include <errno.h>

#define base CSocket

CListeningSocket::CListeningSocket()
{
    int iOption = true;
    // Reuse option
    setsockopt(getFd(), SOL_SOCKET, SO_REUSEADDR, &iOption, sizeof(iOption));
}

// Listen
bool CListeningSocket::listen(uint16_t uiPort)
{
    struct sockaddr_in server_addr;

    // Fill server address
    initSockAddrIn(&server_addr, INADDR_ANY, uiPort);

    // Bind
    if (bind(getFd(), (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) == -1) {

        perror("CListeningSocket::listen::bind");

        return false;
    }

    if (::listen(getFd(), 5) == -1) {

        perror("CListeningSocket::listen::bind");

        return false;
    }
    return true;
}

// Accept
CSocket* CListeningSocket::accept()
{
    struct sockaddr_in client_addr;
    socklen_t ulClientAddrLen = sizeof(client_addr);

    int iSockId = ::accept(getFd(), (struct sockaddr*)&client_addr, &ulClientAddrLen);

    if (iSockId == -1) {

        perror("CListeningSocket::accept::accept");

        return NULL;
    }
    return new CSocket(iSockId);
}
