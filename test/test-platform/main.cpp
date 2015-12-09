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
#include "convert.hpp"
#include "Utility.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using std::cerr;
using std::endl;
using std::string;

static const uint16_t defaultPortNumber = 5001;

static void showUsage()
{
    cerr << "test-platform [-h|--help] <file path> [port number, default " << defaultPortNumber
         << "]" << endl;
}

static void showInvalidUsage(const string &error)
{
    cerr << "Invalid arguments: " << error;
    showUsage();
}

static void showHelp()
{
    showUsage();
    cerr << "<file path> must be a valid Paramter top level config file, "
         << "often named ParameterFrameworkConfiguration.xml.\n"
         << "Arguments:" << endl
         << "    -h|--help  display this help and exit" << endl;
}

int main(int argc, char *argv[])
{
    using Options = std::list<string>;
    // argv[0] is the program name, not an option
    Options options(argv + 1, argv + argc);

    // Handle help option
    auto helpOpts = {"-h", "--help"};
    auto match = std::find_first_of(begin(options), end(options), begin(helpOpts), end(helpOpts));
    if (match != end(options)) {
        showHelp();
        return 0;
    }

    if (options.empty()) {
        showInvalidUsage("Expected a path to a Parameter Framework config file.");
        return 1;
    }

    auto filePath = options.front();
    options.pop_front();

    // Handle optional port number argument
    uint16_t portNumber = defaultPortNumber;

    if (not options.empty()) {
        if (not convertTo(options.front(), portNumber)) {
            showInvalidUsage("Could not convert \"" + options.front() +
                             "\" to a socket port number.");
            return 2;
        };
        options.pop_front();
    }

    // All arguments should have been consumed
    if (not options.empty()) {
        showInvalidUsage("Unexpected extra arguments: " + utility::asString(options));
        return 3;
    }

    string strError;
    if (!CTestPlatform(filePath, portNumber).run(strError)) {

        cerr << "Test-platform error:" << strError.c_str() << endl;
        return -1;
    }
    return 0;
}
