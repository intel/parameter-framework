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
#include "SubsystemObject.h"
#include "InstanceConfigurableElement.h"
#include "ParameterBlackboard.h"
#include "MappingContext.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <stdarg.h>

CSubsystemObject::CSubsystemObject(CInstanceConfigurableElement* pInstanceConfigurableElement)
    : _pInstanceConfigurableElement(pInstanceConfigurableElement),
      _uiDataSize(pInstanceConfigurableElement->getFootPrint()),
      _pucBlackboardLocation(NULL),
      _uiAccessedIndex(0)
{
    // Syncer
    _pInstanceConfigurableElement->setSyncer(this);
}

CSubsystemObject::~CSubsystemObject()
{
    _pInstanceConfigurableElement->unsetSyncer();
}

// Blackboard data location
uint8_t* CSubsystemObject::getBlackboardLocation() const
{
    return _pucBlackboardLocation;
}

// Size
uint32_t CSubsystemObject::getSize() const
{
    return _uiDataSize;
}

// Conversion utility
uint32_t CSubsystemObject::asInteger(const string& strValue)
{
    return strtoul(strValue.c_str(), NULL, 0);
}

string CSubsystemObject::asString(uint32_t uiValue)
{
    ostringstream ostr;

    ostr << uiValue;

    return ostr.str();
}

// Synchronization
bool CSubsystemObject::sync(CParameterBlackboard& parameterBlackboard, bool bBack, string& strError)
{
    // Get blackboard location
    _pucBlackboardLocation = parameterBlackboard.getLocation(_pInstanceConfigurableElement->getOffset());
    // Access index init
    _uiAccessedIndex = 0;

#ifdef SIMULATION
    return true;
#endif

    // Synchronize to/from HW
    if (bBack) {

        // Read from HW
        if (!accessHW(true, strError)) {

            strError = "Unable to back synchronize configurable element " + _pInstanceConfigurableElement->getPath() + ": " + strError;

            return false;
        }

    } else {

        // Send to HW
        if (!accessHW(false, strError)) {

            strError = "Unable to synchronize configurable element " + _pInstanceConfigurableElement->getPath() + ": " + strError;

            return false;
        }
    }
    return true;
}

// Sync to/from HW
bool CSubsystemObject::sendToHW(string& strError)
{
    strError = "Send to HW interface not implemented at subsystsem level!";

    return false;
}

bool CSubsystemObject::receiveFromHW(string& strError)
{
    strError = "Receive from HW interface not implemented at subsystsem level!";

    return false;
}

// Fall back HW access
bool CSubsystemObject::accessHW(bool bReceive, string& strError)
{
    // Default access fall back
    if (bReceive) {

        return receiveFromHW(strError);
    } else {

        return sendToHW(strError);
    }
}

// Blackboard access from subsystems
void CSubsystemObject::blackboardRead(void* pvData, uint32_t uiSize)
{
    assert(_uiAccessedIndex + uiSize <= _uiDataSize);

    memcpy(pvData, _pucBlackboardLocation + _uiAccessedIndex, uiSize);

    _uiAccessedIndex += uiSize;
}

void CSubsystemObject::blackboardWrite(const void* pvData, uint32_t uiSize)
{
    assert(_uiAccessedIndex + uiSize <= _uiDataSize);

    memcpy(_pucBlackboardLocation + _uiAccessedIndex, pvData, uiSize);

    _uiAccessedIndex += uiSize;
}

// Logging
void CSubsystemObject::log(const string& strMessage, ...) const
{
    char acBuffer[512];
    va_list listPointer;

    va_start(listPointer, strMessage);

    vsnprintf(acBuffer, sizeof(acBuffer), strMessage.c_str(), listPointer);

    va_end(listPointer);

    _pInstanceConfigurableElement->log(acBuffer);
}

// Amendment
string CSubsystemObject::formatMappingValue(const string& strMappingValue, uint32_t uiFirstAmendKey, uint32_t uiNbAmendKeys, const CMappingContext& context)
{
    string strFormattedValue = strMappingValue;

    // Search for amendment (only one supported for now)
    size_t uiPercentPos = strFormattedValue.find('%', 0);

    // Amendment limited to one digit (values from 1 to 9)
    assert((uiNbAmendKeys > 0) && (uiNbAmendKeys <= 9));

    // Check we found one and that there's room for value
    if (uiPercentPos != string::npos && uiPercentPos < strFormattedValue.size() - 1) {

        // Get Amend number
        uint32_t uiAmendNumber = strFormattedValue[uiPercentPos + 1] - '0';

        // Valid?
        if (uiAmendNumber && uiAmendNumber <= uiNbAmendKeys) {

            uint32_t uiAmendType = uiFirstAmendKey + uiAmendNumber - 1;

            // Set?
            if (context.iSet(uiAmendType)) {

                // Make the amendment on the part of the string after the current Amend
                string strEndOfLine = strFormattedValue.substr(uiPercentPos + 2, strFormattedValue.size() - uiPercentPos - 2);
                string strEndOfLineAmended = formatMappingValue(strEndOfLine, uiFirstAmendKey, uiNbAmendKeys, context);

                // Get current Amend value
                string strAmendValue = context.getItem(uiAmendType);

                // Make the amendment
                strFormattedValue = strFormattedValue.substr(0, uiPercentPos) + strAmendValue + strEndOfLineAmended;

            }
        }
    }
    return strFormattedValue;
}

// Configurable element retrieval
const CInstanceConfigurableElement* CSubsystemObject::getConfigurableElement() const
{
    return _pInstanceConfigurableElement;
}
