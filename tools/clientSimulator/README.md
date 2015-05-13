Copyright (c) 2014-2015, Intel Corporation
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Client simulator

## Project
This project allows user to test Parameter Framework configurations.
It allows to write scenario, mixing criterion change and arbitrary
script execution.

It ease the development of automatic Parameter Framework configuration
functional test.

## How to

You can run tests using pfClientSimulator.py file.

You have to run the script with at least the test directory argument:

* test\_directory: path to the test directory

Different options are available:

* -s, --scenario    : precise the scenario number to launch
* -i, --interactive : run in interactive mode to test new vectors
* -v, --verbose     : make the script talk on stdin
* -c, --coverage    : generate the html coverage report at the end of the script
* -h, --help        : show options recap

To see available scenarios in the test directory, you can run:

``` ./pfClientSimulator.py path/to/tests/ ```

The script will wait for a choice and the desired scenario will be
launched. You can also precise a scenario with -s option.

