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
#include "ParameterAccessContext.h"

#define base CErrorContext

CParameterAccessContext::CParameterAccessContext(string& strError, CParameterBlackboard* pParameterBlackboard, bool bValueSpaceIsRaw, bool bOutputRawFormatIsHex) :
    base(strError), _pParameterBlackboard(pParameterBlackboard),
    _bValueSpaceIsRaw(bValueSpaceIsRaw), _bOutputRawFormatIsHex(bOutputRawFormatIsHex),
    _bBigEndianSubsystem(false), _bAutoSync(true), _bDynamicAccess(false)
{
}

CParameterAccessContext::CParameterAccessContext(string& strError) :
    base(strError), _pParameterBlackboard(NULL), _bValueSpaceIsRaw(false),
    _bOutputRawFormatIsHex(false), _bBigEndianSubsystem(false), _bAutoSync(true), _bDynamicAccess(false)
{
}

// ParameterBlackboard
CParameterBlackboard* CParameterAccessContext::getParameterBlackboard()
{
    return _pParameterBlackboard;
}

void CParameterAccessContext::setParameterBlackboard(CParameterBlackboard* pBlackboard)
{
    _pParameterBlackboard = pBlackboard;
}

// Value Space
void CParameterAccessContext::setValueSpaceRaw(bool bIsRaw)
{
    _bValueSpaceIsRaw = bIsRaw;
}

bool CParameterAccessContext::valueSpaceIsRaw() const
{
    return _bValueSpaceIsRaw;
}

// Output Raw Format for user get value interpretation
void CParameterAccessContext::setOutputRawFormat(bool bIsHex)
{
    _bOutputRawFormatIsHex = bIsHex;
}

bool CParameterAccessContext::outputRawFormatIsHex()
{
    return _bOutputRawFormatIsHex;
}

// Endianness
void CParameterAccessContext::setBigEndianSubsystem(bool bBigEndian)
{
    _bBigEndianSubsystem = bBigEndian;
}

bool CParameterAccessContext::isBigEndianSubsystem() const
{
    return _bBigEndianSubsystem;
}

// Automatic synchronization to HW
void CParameterAccessContext::setAutoSync(bool bAutoSync)
{
    _bAutoSync = bAutoSync;
}

bool CParameterAccessContext::getAutoSync() const
{
    return _bAutoSync || _bDynamicAccess;
}

// Dynamic access
void CParameterAccessContext::setDynamicAccess(bool bDynamicAccess)
{
    _bDynamicAccess = bDynamicAccess;
}

bool CParameterAccessContext::isDynamicAccess() const
{
    return _bDynamicAccess;
}
