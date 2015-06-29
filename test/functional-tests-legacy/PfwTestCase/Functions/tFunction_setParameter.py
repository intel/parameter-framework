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
setParameter testcase

List of tested functions :
--------------------------
    - [setParameter]  function

Test cases :
------------
    This function is intensively tested through all the tests of data types.
    We test only function commands errors in that script
"""
from Util.PfwUnitTestLib import PfwTestCase
from Util import ACTLogging
log=ACTLogging.Logger()

class TestCases(PfwTestCase):
    def setUp(self):
        self.pfw.sendCmd("setTuningMode", "on")
        self.domain_name = "Domain_0"
        self.param_name = "/Test/Test/TEST_DIR/INT8"


    def tearDown(self):
        self.pfw.sendCmd("setTuningMode", "off")

    def test_Function_Commands_Errors(self):
        """
        Testing function command errors
        ----------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - setParameter with a wrong parameter name
                - setParameter with a forbiden character value
                - setParameter with a Float into an Integer parameter
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - errors correctly detected
        """
        log.D(self.test_Function_Commands_Errors.__doc__)
        #Set undefined parameter value
        log.I("Set undefined parameter value")
        out, err = self.pfw.sendCmd("setParameter", "Undefined_parameter", "0")
        assert err == None, "Error when setting parameter : %s" % (err)
        assert out != "Done", "Error not detected when setting an undefined parameter"
        #Set parameter with a forbiden character value
        log.I("Set parameter with a forbiden character value")
        out, err = self.pfw.sendCmd("setParameter", self.param_name, "Wrong_Value")
        assert err == None, "Error when setting parameter : %s" % (err)
        assert out != "Done", "Error not detected when setting a parameter with a forbiden character value"
        log.I("Errors correctly detected")
        #Set parameter with a Float into an Integer parameter
        log.I("Set parameter with a Float into an Integer parameter")
        out, err = self.pfw.sendCmd("setParameter", self.param_name, "1.2345")
        assert err == None, "Error when setting parameter : %s" % (err)
        assert out != "Done", "Error not detected when setting a Float into an Integer parameter"
        log.I("Errors correctly detected")
