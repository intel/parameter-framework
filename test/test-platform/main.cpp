/*
 * Copyright (c) 2011-2014, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
