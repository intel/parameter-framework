#!/usr/bin/python2
# -*-coding:utf-8 -*

# Copyright (c) 2011-2015, Intel Corporation
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors
# may be used to endorse or promote products derived from this software without
# specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

"""
Create a test suite for all tests about SET/GET commands

Uses PfwSetTsetSuite to create a single instance of the HAL
for all the SET/GEt commands.

These commands are tested using the methods of the classes
"BooleanTestCase", etc...
"""

import sys
import os
import unittest
import shutil
from Util import PfwUnitTestLib

class Logger(object):

    def __init__(self, filename="Default.log"):
        self.terminal = sys.stdout
        self.log = open(filename, "a")

    def write(self, message):
        self.terminal.write(message)
        self.log.write(message)

def testsRunner(testDirectory):

    tests = unittest.defaultTestLoader.discover(testDirectory, pattern='t*.py')
    runner = unittest.TextTestRunner(verbosity=2)

    return runner.run(tests).wasSuccessful()

def main():

    pfw_root =  os.environ["PFW_ROOT"]
    pfw_result = os.environ["PFW_RESULT"]
    xml_path = "xml/configuration/ParameterFrameworkConfiguration.xml"

    os.environ["PFW_TEST_TOOLS"] = os.path.dirname(os.path.abspath(__file__))
    os.environ["PFW_TEST_CONFIGURATION"] = os.path.join(pfw_root, xml_path)

    try:
        # This directory must not exist. An exception will be raised if it does.
        os.makedirs(pfw_result)

        isAlive =  os.path.join(pfw_result,"isAlive")
        with open(isAlive, 'w') as fout:
            fout.write('true')

        needResync = os.path.join(pfw_result,"needResync")
        with open(needResync, 'w') as fout:
            fout.write('false')

        success = testsRunner('PfwTestCase')

    finally:
        shutil.rmtree(pfw_result)

    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()
