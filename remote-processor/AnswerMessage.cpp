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
#include "AnswerMessage.h"
#include "RemoteProcessorProtocol.h"
#include <assert.h>

#define base CMessage

CAnswerMessage::CAnswerMessage(const string& strAnswer, bool bSuccess) : base(bSuccess ? ESuccessAnswer : EFailureAnswer), _strAnswer(strAnswer)
{
}

CAnswerMessage::CAnswerMessage()
{
}

// Answer
void CAnswerMessage::setAnswer(const string& strAnswer)
{
    _strAnswer = strAnswer;
}

const string& CAnswerMessage::getAnswer() const
{
    return _strAnswer;
}

// Status
bool CAnswerMessage::success() const
{
    return getMsgId() == ESuccessAnswer;
}

// Size
uint32_t CAnswerMessage::getDataSize() const
{
    // Answer
    return getStringSize(getAnswer());
}

// Fill data to send
void CAnswerMessage::fillDataToSend()
{
    // Send answer
    writeString(getAnswer());
}

// Collect received data
void CAnswerMessage::collectReceivedData()
{
    // Receive answer
    string strAnswer;

    readString(strAnswer);

    setAnswer(strAnswer);
}
