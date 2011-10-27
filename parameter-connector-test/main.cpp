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
#include <semaphore.h>
#include "ExamplePlatform.h"

using namespace std;

const char* gpcParameter = "/Audio/MSIC/SOUND_CARD/PLAYBACK_ROGUE/HEADPHONE/VOLUME";
const char* gpcParameterValue = "51 53";

int main(int argc, char *argv[])
{
    if (argc < 2) {

        cerr << "Missing arguments" << endl;

        return -1;
    }

    string strError;

    // Create param mgr
    CExamplePlatform examplePlatform(argv[1]);

    // Start platformmgr
    if (!examplePlatform.start(strError)) {

        cerr << strError << endl;

        return -1;
    }

    // Get parameter
    string strValue;

    if (!examplePlatform.getValue(gpcParameter, strValue, strError)) {

        cerr << "Unable to get parameter: " << strError << endl;

        return -1;
    }
    cout << gpcParameter << " = " << strValue << endl;

    // Change parameter
    cout << "Setting " << gpcParameter << " to " << gpcParameterValue << endl;

    if (!examplePlatform.setValue(gpcParameter, gpcParameterValue, strError)) {

        cerr << "Unable to set parameter: " << strError << endl;

        return -1;
    }

    // Check parameter
    if (!examplePlatform.getValue(gpcParameter, strValue, strError)) {

        cerr << "Unable to get parameter: " << strError << endl;

        return -1;
    }
    cout << gpcParameter << " = " << strValue << endl;

    // Change criteria

    // Block here
    sem_t sem;

    sem_init(&sem, false, 0);

    sem_wait(&sem);

    sem_destroy(&sem);

    return 0;
}
