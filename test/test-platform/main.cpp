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
#include <string.h>
#include <unistd.h>

using namespace std;

const int iDefaultPortNumber = 5001;

// Starts test-platform in blocking mode
static bool startBlockingTestPlatform(const char *filePath, int portNumber, string &strError)
{

    // Init semaphore
    sem_t sem;

    sem_init(&sem, false, 0);

    // Create param mgr
    CTestPlatform testPlatform(filePath, portNumber, sem);

    // Start platformmgr
    if (!testPlatform.load(strError)) {

        sem_destroy(&sem);

        return false;
    }

    // Block here
    sem_wait(&sem);

    sem_destroy(&sem);

    return true;
}

// Starts test-platform in daemon mode
static bool startDaemonTestPlatform(const char *filePath, int portNumber, string &strError)
{
    // Pipe used for communication between the child and the parent processes
    int pipefd[2];

    if (pipe(pipefd) == -1) {

        strError = "pipe failed";
        return false;
    }

    // Fork the current process:
    // - Child process is used to start test-platform
    // - Parent process is killed in order to turn its child into a daemon
    pid_t pid = fork();

    if (pid < 0) {

        strError = "fork failed!";
        return false;

    } else if (pid == 0) {

        // Child process : starts test-platform and notify the parent if it succeeds.

        // Close read side of the pipe
        close(pipefd[0]);

        // Init semaphore
        sem_t sem;

        sem_init(&sem, false, 0);

        // Create param mgr
        CTestPlatform testPlatform(filePath, portNumber, sem);

        // Message to send to parent process
        bool msgToParent;

        // Start platformmgr
        if (!testPlatform.load(strError)) {

            cerr << strError << endl;

            // Notify parent of failure;
            msgToParent = false;
            write(pipefd[1], &msgToParent, sizeof(msgToParent));

            sem_destroy(&sem);
        } else {

            // Notify parent of success
            msgToParent = true;
            write(pipefd[1], &msgToParent, sizeof(msgToParent));

            // Block here
            sem_wait(&sem);

            sem_destroy(&sem);
        }

        return msgToParent;

    } else {

        // Parent process : need to kill it once the child notifies the successs/failure to start
        // test-platform (this status is used as exit value of the program).

        // Close write side of the pipe
        close(pipefd[1]);

        // Message received from the child process
        bool msgFromChild = false;

        if (read(pipefd[0], &msgFromChild, sizeof(msgFromChild)) <= 0) {

            strError = "Read pipe failed";
        }

        // return success/failure in exit status
        return msgFromChild;
    }
}

static void usage()
{
    cerr << "Invalid arguments: test-platform [-d] <file path> [port number, default "
         << iDefaultPortNumber << "]" << endl;
}

int main(int argc, char *argv[])
{
    // Option found by call to getopt()
    int opt;

    // Port number to be used by test-platform
    int portNumber;

    // Daemon flag
    bool isDaemon = false;

    // Index of the <file path> argument in the arguments list provided
    int indexFilePath = 1;

    // Handle the -d option
    while ((opt = getopt(argc, argv, "d")) != -1) {
        switch (opt) {
        case 'd':
            isDaemon = true;
            indexFilePath = 2;
            break;

        default:
            usage();
            return -1;
        }
    }

    // Check the number of arguments
    if ((argc < indexFilePath + 1) || (argc > indexFilePath + 2)) {

        usage();
        return -1;
    }

    char *filePath = argv[indexFilePath];
    portNumber = argc > indexFilePath + 1 ? atoi(argv[indexFilePath + 1]) : iDefaultPortNumber;

    // Choose either blocking or daemon test-platform
    bool startError;
    string strError;

    if (isDaemon) {

        startError = startDaemonTestPlatform(filePath, portNumber, strError);
    } else {

        startError = startBlockingTestPlatform(filePath, portNumber, strError);
    }

    if (!startError) {

        cerr << "Test-platform error:" << strError.c_str() << endl;
        return -1;
    }
    return 0;
}
