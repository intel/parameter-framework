#! python2.7
# Copyright (c) 2015, Intel Corporation
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

import sys
import os
import subprocess
import difflib
import unittest
import copy

class PfConfig:
    def __init__(self, toplevel, criteria, schemas):
        self.toplevel = toplevel
        self.criteria = criteria
        self.schemas = schemas

class TestVector:
    def __init__(self, initialSettings=None, edds=[], domains=[]):
        self.initialSettings = initialSettings
        self.edds = edds
        self.domains = domains

class Tester(object):
    def __init__(self, pfConfig, testVector):
        self.command = [sys.executable, "domainGenerator.py",
                        "--validate",
                        "--verbose",
                        "--toplevel-config", pfConfig.toplevel,
                        "--criteria", pfConfig.criteria,
                        "--schemas-dir", pfConfig.schemas]

        if testVector.initialSettings:
            self.command += ["--initial-settings", testVector.initialSettings]
        if testVector.edds:
            self.command += ["--add-edds"] + testVector.edds
        if testVector.domains:
            self.command += ["--add-domains"] + testVector.domains

    def check(self, reference=None, expectedErrors=0):
        process = subprocess.Popen(self.command, stdout=subprocess.PIPE)
        actual = process.stdout.read().splitlines()

        if process.wait() != expectedErrors:
            raise AssertionError("Expected {} errors, found {}".format(
                expectedErrors,
                process.returncode))

        if not reference:
            # The caller only wants to check the number of errors
            return

        # The generation has succeeded as expected - let's compare with the reference.
        if reference != actual:
            unified = difflib.unified_diff(reference,
                                           actual,
                                           fromfile="reference.xml",
                                           tofile="-",
                                           lineterm="")
            raise AssertionError("The result and the reference don't match:" + "\n".join(unified))


basedir = os.path.dirname(sys.argv[0])

config_dir = os.path.join(basedir, "PFConfig")
vector_dir = os.path.join(basedir, "testVector")
class TestCase(unittest.TestCase):
    def setUp(self):
        self.nominal_reference = open(os.path.join(vector_dir, "reference.xml")).read().splitlines()
        self.nominal_pfconfig = PfConfig(os.path.join(config_dir, "configuration.xml"),
                                         os.path.join(config_dir, "criteria.txt"),
                                         os.path.join(basedir, "../../schemas"))
        self.nominal_vector = TestVector(os.path.join(vector_dir, "initialSettings.xml"),
                                         [os.path.join(vector_dir, "first.pfw"),
                                          os.path.join(vector_dir, "second.pfw"),
                                          os.path.join(vector_dir, "complex.pfw")],
                                         [os.path.join(vector_dir, "third.xml"),
                                          os.path.join(vector_dir, "fourth.xml")])

    def test_nominal(self):
        tester = Tester(self.nominal_pfconfig, self.nominal_vector)
        tester.check(self.nominal_reference)

    def test_nonfatalError(self):
        self.nominal_vector.edds.append(os.path.join(vector_dir, "duplicate.pfw"))

        tester = Tester(self.nominal_pfconfig, self.nominal_vector)
        tester.check(self.nominal_reference, expectedErrors=1)

    def test_conflicting(self):
        vector = TestVector(edds=[os.path.join(vector_dir, "conflicting.pfw")])

        tester = Tester(self.nominal_pfconfig, vector)
        tester.check(expectedErrors=1)

    def test_fail_criteria(self):
        self.nominal_pfconfig.criteria = os.path.join(config_dir, "duplicate_criterion_value.txt")
        # Empty test vector: we want to make sure that the erroneous criterion
        # is the only source of error
        empty_vector = TestVector()

        tester = Tester(self.nominal_pfconfig, empty_vector)
        tester.check(expectedErrors=1)

if __name__ == "__main__":
    unittest.main()
