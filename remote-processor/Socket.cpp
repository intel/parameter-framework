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
#include "Socket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <assert.h>
#include <netdb.h>
#include <strings.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/time.h>
#include <signal.h>

CSocket::CSocket() : _iSockFd(socket(AF_INET, SOCK_STREAM, 0)), mSendFlag(0)
{
    assert(_iSockFd != -1);

    int iNoDelay = 1;
    // (see man 7 tcp)
    // Setting TCP_NODELAY allows us sending commands and responses as soon as
    // they are ready to be sent, instead of waiting for more data on the
    // socket.
    setsockopt(_iSockFd, IPPROTO_TCP, TCP_NODELAY, (char *)&iNoDelay, sizeof(iNoDelay));

    // Disable sigpipe reception on send
#   if not defined(SIGPIPE)
        // Pipe signal does not exist, there no sigpipe to ignore on send
#   elif defined(SO_NOSIGPIPE)
        const int set = 1;
        setsockopt(_iSockFd, SOL_SOCKET, SO_NOSIGPIPE, &set, sizeof(set));
#   elif defined(MSG_NOSIGNAL)
        // Use flag NOSIGNAL on send call
        mSendFlag = MSG_NOSIGNAL;
#   else
#       error Can not disable SIGPIPE
#   endif
}

CSocket::CSocket(int iSockId) : _iSockFd(iSockId)
{
    assert(_iSockFd != -1);
}

CSocket::~CSocket()
{
    // fd might be invalide if send had an error.
    // valgrind displays a warning if closing an invalid fd.
    if (_iSockFd != -1) {
        close(_iSockFd);
    }
}

// Socket address init
void CSocket::initSockAddrIn(struct sockaddr_in* pSockAddrIn, uint32_t uiInAddr, uint16_t uiPort) const
{
    // Fill server address
    pSockAddrIn->sin_family = AF_INET;
    pSockAddrIn->sin_port = htons(uiPort);
    pSockAddrIn->sin_addr.s_addr = uiInAddr;
    bzero(&pSockAddrIn->sin_zero, sizeof(pSockAddrIn->sin_zero));
}

// Non blocking state
void CSocket::setNonBlocking(bool bNonBlocking)
{
    int iFlags = fcntl(_iSockFd, F_GETFL, 0);

    assert(iFlags != -1);

    if (bNonBlocking) {

        iFlags |= O_NONBLOCK;
    } else {

        iFlags &= ~O_NONBLOCK;
    }
    fcntl(_iSockFd, F_SETFL, iFlags);
}

// Communication timeout
void CSocket::setTimeout(uint32_t uiMilliseconds)
{
    struct timeval tv;
    tv.tv_sec = uiMilliseconds / 1000;
    tv.tv_usec = (uiMilliseconds % 1000) * 1000;

    setsockopt(_iSockFd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
    setsockopt(_iSockFd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

// Read
bool CSocket::read(void* pvData, uint32_t uiSize)
{
    uint32_t uiOffset = 0;
    uint8_t* pucData = (uint8_t*)pvData;

    while (uiSize) {

        int32_t iAccessedSize = ::recv(_iSockFd, &pucData[uiOffset], uiSize, 0);

        switch (iAccessedSize) {
        case 0:
            // recv return value is 0 when the peer has performed an orderly shutdown.
            _disconnected = true;
            errno = ECONNRESET; // Warn the client that the client disconnected.
            return false;

        case -1:
            // errno == EINTR => The recv system call was interrupted, try again
            if (errno != EINTR) {
                return false;
            }
            break;

        default:
            uiSize -= iAccessedSize;
            uiOffset += iAccessedSize;
        }
    }
    return true;
}

// Write
bool CSocket::write(const void* pvData, uint32_t uiSize)
{
    uint32_t uiOffset = 0;
    const uint8_t* pucData = (const uint8_t*)pvData;

    while (uiSize) {

        int32_t iAccessedSize = ::send(_iSockFd, &pucData[uiOffset], uiSize, mSendFlag);

        if (iAccessedSize == -1) {
            if (errno == EINTR) {
                // The send system call was interrupted, try again
                continue;
            }

            // An error occured, forget this socket
            _disconnected = true;
            close(_iSockFd);
            _iSockFd = -1; // Avoid writing again on the same socket
            return false;
        } else {
            uiSize -= iAccessedSize;
            uiOffset += iAccessedSize;
        }
    }
    return true;
}

// Fd
int CSocket::getFd() const
{
    return _iSockFd;
}

bool CSocket::hasPeerDisconnected() {
    return _disconnected;
}
