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
#include "FullIo.hpp"

#include <iostream>
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <cerrno>
#include <cassert>

using namespace std;

const int iDefaultPortNumber = 5001;

static void showUsage()
{
    cerr << "test-platform [-h] <file path> [port number, default "
         << iDefaultPortNumber << "]" << endl;
}

static void showInvalidUsage()
{
    cerr << "Invalid arguments: ";
    showUsage();
}

static void showHelp()
{
    showUsage();
    cerr << "<file path> must be a valid .xml file, oftenly ParameterFrameworkConfiguration.xml" << endl;
    cerr << "Arguments:" << endl
        << "    -h  display this help and exit" << endl;
}

int main(int argc, char *argv[])
{
    // Option found by call to getopt()
    int opt;

    // Port number to be used by test-platform
    int portNumber;

    // Index of the <file path> argument in the arguments list provided
    int indexFilePath = 1;

    // Handle the -h option
    while ((opt = getopt(argc, argv, "h")) != -1) {
        switch (opt) {
        case 'h':
            showHelp();
            return 0;
        default:
            showInvalidUsage();
            return -1;
        }
    }

    // Check the number of arguments
    if ((argc < indexFilePath + 1) || (argc > indexFilePath + 2)) {

        showInvalidUsage();
        return -1;
    }

    char *filePath = argv[indexFilePath];
    portNumber = argc > indexFilePath + 1 ? atoi(argv[indexFilePath + 1]) : iDefaultPortNumber;

    string strError;
    if (!CTestPlatform(filePath, portNumber).run(strError)) {

        cerr << "Test-platform error:" << strError.c_str() << endl;
        return -1;
    }
    return 0;
}
