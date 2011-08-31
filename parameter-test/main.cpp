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
#include <iostream>
#include "ParameterMgr.h"
#include "ParameterMgrPlatformConnector.h"
#include <semaphore.h>

using namespace std;

#ifdef SIMULATION
const char* gpcParameterFrameworkConfigurationFolderPath = "/home/pat/projects/qt/parameter-framework/XML";
#else
const char* gpcParameterFrameworkConfigurationFolderPath = "/etc/parameter-framework";
#endif

//#define SIMPLE_TEST

int main(int argc, char *argv[])
{    
#ifndef SIMPLE_TEST
    if (argc != 4) {

        cerr << "Wrong number of arguments" << endl;

        return -1;
    }
#else
    if (argc < 2) {

        cerr << "Missing arguments" << endl;

        return -1;
    }
#endif
    string strError;

    //CParameterMgrPlatformConnector parameterMgrPlatformConnector(argv[1]);
    CParameterMgr parameterMgr(gpcParameterFrameworkConfigurationFolderPath, argv[1]);

    // Load data structure
    if (!parameterMgr.load(strError)) {

        cerr << strError << endl;

        return -1;
    }

    // Init flow
    if (!parameterMgr.init(strError)) {

        cerr << strError << endl;

        return -1;
    }
#ifndef SIMPLE_TEST
    // Set Tuning Mode on
    if (!parameterMgr.setTuningMode(true, strError)) {

        cerr << strError << endl;

        return -1;
    }

    // Restore some configuration
    if (!parameterMgr.restoreConfiguration("Multimedia.OutputDevice.Private.Selected", "Earpiece", strError)) {

        cerr << strError << endl;

        return -1;
    }
    // Save some configuration
    if (!parameterMgr.saveConfiguration("Multimedia.OutputDevice.Public.Selected", "IHF", strError)) {

        cerr << strError << endl;

        return -1;
    }

    // Log content
    string strContent;
    parameterMgr.logStructureContent(strContent);

    cout << strContent;

    //parameterMgr.setValueSpaceRaw(true);

    string strValue;
    if (!parameterMgr.getValue(argv[2], strValue, strError)) {

        cerr << strError << endl;

        return -1;
    }
    cout << "Before setting: " << strValue << endl;

    if (!parameterMgr.setValue(argv[2], argv[3], strError)) {

        cerr << strError << endl;

        return -1;
    }

    if (!parameterMgr.getValue(argv[2], strValue, strError)) {

        cerr << strError << endl;

        return -1;
    }
    cout << "After setting: " << strValue << endl;

    // Save some configuration
    if (!parameterMgr.saveConfiguration("Multimedia.OutputDevice.Private.Selected", "WiredSpeakers", strError)) {

        cerr << strError << endl;

        return -1;
    }

    // Save some configuration
    if (!parameterMgr.saveConfiguration("Multimedia.OutputDevice.Private.Selected", "BluetoothSCO", strError)) {

        cerr << strError << endl;

        return -1;
    }
    // Export configurations
    if (!parameterMgr.exportDomainsXml(string(gpcParameterFrameworkConfigurationFolderPath) + "/Settings/Audio/AudioSettingsExport.xml", true, strError)) {

        cerr << strError << endl;

        return -1;
    }
    // Import configurations
    if (!parameterMgr.importDomainsXml(string(gpcParameterFrameworkConfigurationFolderPath) + "/Settings/Audio/AudioSettingsExport.xml", true, strError)) {

        cerr << strError << endl;

        return -1;
    }

    // Export configuration as binary
    if (!parameterMgr.exportDomainsBinary(string(gpcParameterFrameworkConfigurationFolderPath) + "/Settings/Audio/AudioSettingsExport.bin", strError)) {

        cerr << strError << endl;

        return -1;
    }

    // Import configuration as binary
    if (!parameterMgr.importDomainsBinary(string(gpcParameterFrameworkConfigurationFolderPath) + "/Settings/Audio/AudioSettingsExport.bin", strError)) {

        cerr << strError << endl;

        return -1;
    }

    // Set Tuning Mode off
    if (!parameterMgr.setTuningMode(false, strError)) {

        cerr << strError << endl;

        return -1;
    }
#endif
    // Block here
    sem_t sem;

    sem_init(&sem, false, 0);

    sem_wait(&sem);

    sem_destroy(&sem);

    return 0;
}

