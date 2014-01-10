/*
 * INTEL CONFIDENTIAL
 * Copyright  2011 Intel
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
 * disclosed in any way without Intels prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Intel in writing.
 *
 * CREATED: 2011-11-25
 * UPDATED: 2011-11-25
 */

#include "TestPlatform.h"

#include <iostream>
#include <cstdlib>
#include <semaphore.h>

using namespace std;

const int iDefaultPortNumber = 5001;

int main(int argc, char *argv[])
{
    if (argc < 2) {

        cerr << "Missing arguments: test-platform <file path> [port number, default " << iDefaultPortNumber << "]" << endl;

        return -1;
    }

    string strError;

    // Init semaphore
    sem_t sem;

    sem_init(&sem, false, 0);

    // Create param mgr
    CTestPlatform testPlatform(argv[1], argc > 2 ? atoi(argv[2]) : iDefaultPortNumber, sem);

    // Start platformmgr
    if (!testPlatform.load(strError)) {

        cerr << strError << endl;

        sem_destroy(&sem);

        return -1;
    }

    // Change criteria

    // Block here
    sem_wait(&sem);

    sem_destroy(&sem);

    return 0;
}
