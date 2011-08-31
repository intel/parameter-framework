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
#include "ExamplePlatform.h"
#include "ParameterMgrPlatformConnector.h"
#include <iostream>

// Logger
class CParameterMgrPlatformConnectorLogger : public CParameterMgrPlatformConnector::ILogger
{
public:
    CParameterMgrPlatformConnectorLogger() {}

    virtual void log(const std::string& strLog)
    {
        cout << strLog << endl;
    }
};

// CExamplePlatform Implementation
CExamplePlatform::CExamplePlatform(const string& strClass) : _pParameterMgrPlatformConnector(new CParameterMgrPlatformConnector(strClass)), _pParameterMgrPlatformConnectorLogger(new CParameterMgrPlatformConnectorLogger)
{
    // Logger
    _pParameterMgrPlatformConnector->setLogger(_pParameterMgrPlatformConnectorLogger);

    /// Criteria Types
    // Mode
    _pModeType = _pParameterMgrPlatformConnector->createSelectionCriterionType();
    _pModeType->addValuePair(0, "Normal");
    _pModeType->addValuePair(1, "InCsvCall");
    _pModeType->addValuePair(2, "InVoipCall");
    _pModeType->addValuePair(3, "RingTone");

    // InputDevice
    _pInputDeviceType = _pParameterMgrPlatformConnector->createSelectionCriterionType(true);
    _pInputDeviceType->addValuePair(0x1, "BuiltIn");
    _pInputDeviceType->addValuePair(0x2, "Headset");
    _pInputDeviceType->addValuePair(0x4, "BluetoothHeadsetSCO");
    _pInputDeviceType->addValuePair(0x8, "VoiceCall");

    // OutputDevice
    _pOutputDeviceType = _pParameterMgrPlatformConnector->createSelectionCriterionType(true);
    _pOutputDeviceType->addValuePair(0x1, "Headphones");
    _pOutputDeviceType->addValuePair(0x2, "Headset");
    _pOutputDeviceType->addValuePair(0x4, "Earpiece");
    _pOutputDeviceType->addValuePair(0x8, "IHF");
    _pOutputDeviceType->addValuePair(0x10, "BluetoothSCO");
    _pOutputDeviceType->addValuePair(0x20, "BluetoothHeadsetSCO");
    _pOutputDeviceType->addValuePair(0x40, "BluetoothCarKitSCO");
    _pOutputDeviceType->addValuePair(0x80, "BluetoothA2DP");

    /// Criteria
    _pMode = _pParameterMgrPlatformConnector->createSelectionCriterion("Mode", _pModeType);
    _pSelectedInputDevice = _pParameterMgrPlatformConnector->createSelectionCriterion("SelectedInputDevice", _pInputDeviceType);
    _pSelectedOutputDevice = _pParameterMgrPlatformConnector->createSelectionCriterion("SelectedOutputDevice", _pOutputDeviceType);

    // Init state
    setState(EInitState);
}

CExamplePlatform::~CExamplePlatform()
{
    delete _pParameterMgrPlatformConnectorLogger;
    delete _pParameterMgrPlatformConnector;
}

// Start
bool CExamplePlatform::start(string& strError)
{
    return _pParameterMgrPlatformConnector->start(strError);
}

// State
void CExamplePlatform::setState(CExamplePlatform::State eState)
{
    switch(eState) {
    case EInitState:
        _pMode->setCriterionState(0, false);
        _pSelectedInputDevice->setCriterionState(0, false);
        _pSelectedOutputDevice->setCriterionState(0x4);
        break;
    case EState1:
        _pMode->setCriterionState(0, false);
        _pSelectedInputDevice->setCriterionState(0, false);
        // Select Headset
        _pSelectedOutputDevice->setCriterionState(0x1);
        break;
    case EState2:
        break;
    case EState3:
        break;
    default:
        break;
    }
}

