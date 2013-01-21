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
#include "HardwareBackSynchronizer.h"
#include "ConfigurableElement.h"

#define base CBackSynchronizer

CHardwareBackSynchronizer::CHardwareBackSynchronizer(const CConfigurableElement* pConfigurableElement, CParameterBlackboard* pParameterBlackboard)
    : base(pConfigurableElement), _pParameterBlackboard(pParameterBlackboard)
{
    // Fill back syncer set
    list<const CConfigurableElement*>::const_iterator it;

    for (it = _needingBackSyncList.begin(); it != _needingBackSyncList.end(); ++it) {

        const CConfigurableElement* pConfigurableElement = *it;

        pConfigurableElement->fillSyncerSet(_backSyncerSet);
    }
}

// Back synchronization
void CHardwareBackSynchronizer::sync()
{
    // Perform back synchronization
    _backSyncerSet.sync(*_pParameterBlackboard, true, NULL);
}
