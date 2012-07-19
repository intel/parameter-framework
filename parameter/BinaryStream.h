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
#include <fstream>

using namespace std;

class CBinaryStream
{
public:
    CBinaryStream(const string& strFileName, bool bOut, uint32_t uiDataSize, uint8_t uiStructureChecksum);
    ~CBinaryStream();

    // Open close
    bool open(string& strError);
    void close();

    // Seek
    void reset();

    // Read/Write
    void write(const uint8_t* puiData, uint32_t uiSize);
    void read(uint8_t* puiData, uint32_t uiSize);

    // Direction
    bool isOut() const;
private:
    // Checksum
    uint8_t computeChecksum() const;

    // File name
    string _strFileName;
    // Serialization direction
    bool _bOut;
    // Data size
    uint32_t _uiDataSize;
    // System structure checksum
    uint8_t _uiStructureChecksum;
    // Read/Write data
    uint8_t* _puiData;
    // File
    fstream _fileStream;
    // Ops in faile
    uint32_t _uiPos;
    // File state
    bool _bOpen;
};

