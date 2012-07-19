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
#include "Socket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <assert.h>
#include <netdb.h>
#include <strings.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/time.h>

CSocket::CSocket() : _iSockFd(socket(AF_INET, SOCK_STREAM, 0))
{
    assert(_iSockFd != -1);
}

CSocket::CSocket(int iSockId) : _iSockFd(iSockId)
{
    assert(_iSockFd != -1);
}

CSocket::~CSocket()
{
    close(_iSockFd);
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

        int32_t iAccessedSize = ::recv(_iSockFd, &pucData[uiOffset], uiSize, MSG_NOSIGNAL);

        if (!iAccessedSize || iAccessedSize == -1) {

            return false;
        }
        uiSize -= iAccessedSize;
        uiOffset += iAccessedSize;
    }
    return true;
}

// Write
bool CSocket::write(const void* pvData, uint32_t uiSize)
{
    uint32_t uiOffset = 0;
    const uint8_t* pucData = (const uint8_t*)pvData;

    while (uiSize) {

        int32_t iAccessedSize = ::send(_iSockFd, &pucData[uiOffset], uiSize, MSG_NOSIGNAL);

        if (!iAccessedSize || iAccessedSize == -1) {

            return false;
        }
        uiSize -= iAccessedSize;
        uiOffset += iAccessedSize;
    }
    return true;
}

// Fd
int CSocket::getFd() const
{
    return _iSockFd;
}
