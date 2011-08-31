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
#include "SubsystemObject.h"
#include "InstanceConfigurableElement.h"
#include "ParameterBlackboard.h"
#include <assert.h>

CSubsystemObject::CSubsystemObject(CInstanceConfigurableElement* pInstanceConfigurableElement)
    : _pInstanceConfigurableElement(pInstanceConfigurableElement), _uiDataSize(pInstanceConfigurableElement->getFootPrint()), _pvSynchronizedLocation(NULL)
{
    // Syncer
    _pInstanceConfigurableElement->setSyncer(this);
}

CSubsystemObject::~CSubsystemObject()
{
    _pInstanceConfigurableElement->unsetSyncer();
}

// Synchronized location
void CSubsystemObject::setSynchronizedLocation(void* pvSynchronizedLocation)
{
    _pvSynchronizedLocation = pvSynchronizedLocation;
}

// Size
uint32_t CSubsystemObject::getSize() const
{
    return _uiDataSize;
}

// Synchronization
bool CSubsystemObject::sync(CParameterBlackboard& parameterBlackboard, bool bBack, string& strError)
{
    assert(_pvSynchronizedLocation);

#ifdef SIMULATION
    return true;
#endif

    // Synchronize to/from HW
    if (bBack) {

        // Read from HW
        if (!receiveFromHW()) {

            strError = "Unable to back synchronize configurable element " + _pInstanceConfigurableElement->getPath();

            return false;
        }

        // Write parameter block data
        parameterBlackboard.rawWrite(_pvSynchronizedLocation, _uiDataSize, _pInstanceConfigurableElement->getOffset());

    } else {

        // Read parameter block data
        parameterBlackboard.rawRead(_pvSynchronizedLocation, _uiDataSize, _pInstanceConfigurableElement->getOffset());

        // Send to HW
        if (!sendToHW()) {

            strError = "Unable to synchronize configurable element " + _pInstanceConfigurableElement->getPath();

            return false;
        }
    }
    return true;
}
