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

#include "ErrorContext.h"

class CParameterBlackboard;

class CParameterAccessContext : public CErrorContext
{
public:
    CParameterAccessContext(string& strError, CParameterBlackboard* pParameterBlackboard, bool bValueSpaceIsRaw, bool bOutputRawFormatIsHex = false);
    CParameterAccessContext(string& strError, bool bBigEndianSubsystem, CParameterBlackboard* pParameterBlackboard);
    CParameterAccessContext(string& strError);

    // ParameterBlackboard
    CParameterBlackboard* getParameterBlackboard();
    void setParameterBlackboard(CParameterBlackboard* pBlackboard);

    // Value interpretation as Real or Raw
    void setValueSpaceRaw(bool bIsRaw);
    bool valueSpaceIsRaw() const;

    // Output Raw Format for user get value interpretation
    void setOutputRawFormat(bool bIsHex);
    bool outputRawFormatIsHex();

    // Endianness
    void setBigEndianSubsystem(bool bBigEndian);
    bool isBigEndianSubsystem() const;

    // Automatic synchronization to HW
    void setAutoSync(bool bAutoSync);
    bool getAutoSync() const;

private:
    // Blackboard
    CParameterBlackboard* _pParameterBlackboard;
    // Value space
    bool _bValueSpaceIsRaw;
    // Output Raw Format
    bool _bOutputRawFormatIsHex;
    // Subsystem Endianness
    bool _bBigEndianSubsystem;
    // Automatic synchronization to HW
    bool _bAutoSync;
};

