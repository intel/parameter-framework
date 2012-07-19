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

#include "Message.h"
#include "RemoteCommand.h"
#include <vector>

class CRequestMessage : public CMessage, public IRemoteCommand
{
public:
    CRequestMessage(const string& strCommand);
    CRequestMessage();

    // Command Name
    virtual const string& getCommand() const;

    // Arguments
    virtual void addArgument(const string& strArgument);
    virtual uint32_t getArgumentCount() const;
    virtual const string& getArgument(uint32_t uiArgument) const;
    virtual const string packArguments(uint32_t uiStartArgument, uint32_t uiNbArguments) const;

private:
    // Fill data to send
    virtual void fillDataToSend();
    // Collect received data
    virtual void collectReceivedData();
    // Size
    virtual uint32_t getDataSize() const;
    // Command
    void setCommand(const string& strCommand);
    // Trim input string
    static string trim(const string& strToTrim);

    // Command
    string _strCommand;
    // Arguments
    vector<string> _argumentVector;
};

