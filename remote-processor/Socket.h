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
#pragma once

#include <string>
#include <stdint.h>

struct sockaddr_in;
struct in_addr;

/** Readable and writable socket.
 *
 * The class does not encapsulate completely it's internal file descriptor as
 * it can be retrieve by the getFd method.
 *
 * This "feature" means that it's state consistency can not
 * be enforced by the class but rather by clients.
 */
class CSocket
{
public:
    CSocket();
    CSocket(int iSockId);
    virtual ~CSocket();

    // Non blocking state
    void setNonBlocking(bool bNonBlocking);

    // Communication timeout
    void setTimeout(uint32_t uiMilliseconds);

    /* Read data
     *
     * On failure errno will be set appropriately (see send).
     * If the client disconnects, false will be returned and
     *     - hasPeerDisconnected will return true
     *     - errno is set to ECONNRESET.
     * @param[in] pvData - on success: will contain the sent data
     *                  - on failure: undefined
     * @param[in] uiSize size of the data to receive.
     *
     * @return true if all data could be read, false otherwise.
     */
    bool read(void* pvData, uint32_t uiSize);

    /* Write data
     *
     * On failure errno will be set (see recv)
     * @param[in] pvData data to send.
     * @param[in] uiSize is the size of the data to send.
     *
     * @return true if all data could be read, false otherwise.
     */
    bool write(const void* pvData, uint32_t uiSize);

    /** @return the managed file descriptor.
     *
     * The client can then bind/connect/accept/listen/... the socket.
     */
    int getFd() const;

    /** @return true if the peer has disconnected.
     *
     * The internal fd is returned by getFd and clients can use it for
     * bind/connect/read/write/... as a result it's state can not be tracked.
     *
     * Thus hasPeerDisconnected returns true only if the disconnection
     * was notified during a call to CSocket::write or CSocket::read.
     */
    bool hasPeerDisconnected();

protected:
    // Socket address init
    void initSockAddrIn(struct sockaddr_in* pSockAddrIn, uint32_t uiInAddr, uint16_t uiPort) const;
private:
    int _iSockFd;
    /** If the peer disconnected.
     *
     * This is not the state of _iSockFd (connected/disconnected)
     *
     * See hasPeerDisconnected for more details.
     */
    bool _disconnected;
    int mSendFlag;
};
