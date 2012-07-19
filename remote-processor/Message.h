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
#include <string>

using namespace std;

class CSocket;

class CMessage
{
public:
    CMessage(uint8_t ucMsgId);
    CMessage();
    virtual ~CMessage();

    // Send/Receive
    bool serialize(CSocket* pSocket, bool bOut);

protected:
    // Msg Id
    uint8_t getMsgId() const;
    // Data
    void writeData(const void* pvData, uint32_t uiSize);
    void readData(void* pvData, uint32_t uiSize);
    void writeString(const string& strData);
    void readString(string& strData);
    uint32_t getStringSize(const string& strData) const;
    // Remaining data size
    uint32_t getRemainingDataSize() const;
private:
    // Data allocation
    void allocateData(uint32_t uiDataSize);
    // Fill data to send
    virtual void fillDataToSend() = 0;
    // Collect received data
    virtual void collectReceivedData() = 0;
    // Size
    virtual uint32_t getDataSize() const = 0;
    // Checksum
    uint8_t computeChecksum() const;

    // MsgId
    uint8_t _ucMsgId;
    // Data
    uint8_t* _pucData;
    // Data size
    uint32_t _uiDataSize;
    // Read/Write Index
    uint32_t _uiIndex;
};
