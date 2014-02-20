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
#include "ConnectionSocket.h"
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>
#include <sstream>

#define base CSocket

using std::string;

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

	std::ostringstream oss;
        oss << "CConnectionSocket::connect::connect on port: " << uiPort;
        perror(oss.str().c_str());

        strError = "Unable to connnect to target :-(";

        return false;
    }
    return true;
}
