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
#include "ListeningSocket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <assert.h>
#include <netdb.h>
#include <strings.h>
#include <sstream>

#include <stdio.h>
#include <errno.h>
#include <cstring>

#define base CSocket

using std::string;

CListeningSocket::CListeningSocket()
{
    int iOption = true;
    // Reuse option
    setsockopt(getFd(), SOL_SOCKET, SO_REUSEADDR, &iOption, sizeof(iOption));
}

// Listen
bool CListeningSocket::listen(uint16_t uiPort, string &strError)
{
    struct sockaddr_in server_addr;

    // Fill server address
    initSockAddrIn(&server_addr, INADDR_ANY, uiPort);

    // Bind
    if (bind(getFd(), (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) == -1) {

        std::ostringstream oss;
        oss << uiPort;
        strError = "Could not bind socket to port " + oss.str() + ": " + strerror(errno);
        return false;
    }

    if (::listen(getFd(), 5) == -1) {

        std::ostringstream oss;
        oss << uiPort;
        strError = "Could not listen to port " + oss.str() + ": " + strerror(errno);
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
