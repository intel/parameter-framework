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
#include "RequestMessage.h"
#include "RemoteProcessorProtocol.h"
#include <assert.h>
#include <algorithm>
#include <ctype.h>

#define base CMessage

CRequestMessage::CRequestMessage(const string& strCommand) : base(ECommandRequest), _strCommand(strCommand)
{
}

CRequestMessage::CRequestMessage()
{
}

// Command Name
void CRequestMessage::setCommand(const string& strCommand)
{
    _strCommand = trim(strCommand);
}

const string& CRequestMessage::getCommand() const
{
    return _strCommand;
}

// Arguments
void CRequestMessage::addArgument(const string& strArgument)
{
    _argumentVector.push_back(trim(strArgument));
}

uint32_t CRequestMessage::getArgumentCount() const
{
    return _argumentVector.size();
}

const string& CRequestMessage::getArgument(uint32_t uiArgument) const
{
    assert(uiArgument < _argumentVector.size());

    return _argumentVector[uiArgument];
}

const string CRequestMessage::packArguments(uint32_t uiStartArgument, uint32_t uiNbArguments) const
{
    string strPackedArguments;

    assert(uiStartArgument + uiNbArguments <= _argumentVector.size());

    // Pack arguments, separating them with a space
    uint32_t uiArgument;

    bool bFirst = true;

    for (uiArgument = uiStartArgument; uiArgument < uiStartArgument + uiNbArguments; uiArgument++) {

        if (!bFirst) {

            strPackedArguments += " ";
        } else {

            bFirst = false;
        }

        strPackedArguments += _argumentVector[uiArgument];
    }

    return strPackedArguments;
}

// Fill data to send
void CRequestMessage::fillDataToSend()
{
    // Send command
    writeString(getCommand());

    // Arguments
    uint32_t uiArgument;

    for (uiArgument = 0; uiArgument < getArgumentCount(); uiArgument++) {

        writeString(getArgument(uiArgument));
    }
}

// Collect received data
void CRequestMessage::collectReceivedData()
{
    // Receive command
    string strCommand;

    readString(strCommand);

    setCommand(strCommand);

    // Arguments
    while (getRemainingDataSize()) {

        string strArgument;

        readString(strArgument);

        addArgument(strArgument);
    }
}

// Size
uint32_t CRequestMessage::getDataSize() const
{
    // Command
    uint32_t uiSize = getStringSize(getCommand());

    // Arguments
    uint32_t uiArgument;

    for (uiArgument = 0; uiArgument < getArgumentCount(); uiArgument++) {

        uiSize += getStringSize(getArgument(uiArgument));
    }
    return uiSize;
}

// Trim input string
string CRequestMessage::trim(const string& strToTrim)
{
    // Trim string
    string strTrimmed = strToTrim;

    strTrimmed.erase(remove_if(strTrimmed.begin(), strTrimmed.end(), ::isspace), strTrimmed.end());

    return strTrimmed;
}
