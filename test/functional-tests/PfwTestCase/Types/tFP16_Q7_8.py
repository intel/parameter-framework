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
Fixed-Point parameter type testcases - FP16_Q7.8

List of tested functions :
--------------------------
    - [setParameter]  function
    - [getParameter] function

Initial Settings :
------------------
    FP16_Q7.8 :
        - size = 16 bits
        - 7 integer bits, 8 fractionnal bits
        - range : [-128, 127.996]

Test cases :
------------
    - FP16_Q7.8 parameter min value = -128
    - FP16_Q7.8 parameter min value out of bounds = -128.001
    - FP16_Q7.8 parameter max value = 127.996
    - FP16_Q7.8 parameter max value out of bounds = 127.997
    - FP16_Q7.8 parameter in nominal case = 23.59
"""
import commands
from Util.PfwUnitTestLib import PfwTestCase
from Util import ACTLogging
log=ACTLogging.Logger()

# Test of type FP16_Q7.8 - range [-128, 127.996]
class TestCases(PfwTestCase):
    def setUp(self):
        self.param_name = "/Test/Test/TEST_DIR/FP16_Q7.8"
        self.pfw.sendCmd("setTuningMode", "on")
        self.type_name = "FP16_Q7.8"

    def tearDown(self):
        self.pfw.sendCmd("setTuningMode", "off")

    def test_Nominal_Case(self):
        """
        Testing FP16_Q7.8 in nominal case = 23.59
        -----------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set FP16_Q7.8 parameter in nominal case = 23.59
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - FP16_Q7.8 parameter set to 23.59
                - Blackboard and filesystem values checked
        """
        log.D(self.test_Nominal_Case.__doc__)
        log.I("FP16_Q7.8 parameter in nominal case = 23.59")
        value = "23.590"
        hex_value = "0x1797"
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, log.E("when setting parameter %s : %s" % (self.param_name, err))
        assert out == "Done", log.F("when setting parameter %s : %s" % (self.param_name, out))
        #Check parameter value on blackboard
        out, err = self.pfw.sendCmd("getParameter", self.param_name, "")
        assert err == None, log.E("when setting parameter %s : %s" % (self.param_name, err))
        assert round(float(out),2) == float(value), log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s"
                                                          % (self.param_name, value, out))
        #Check parameter value on filesystem
        assert commands.getoutput('cat $PFW_RESULT/FP16_Q7.8') == hex_value, log.F("FILESYSTEM : parameter update error")
        log.I("test OK")

    def test_TypeMin(self):
        """
        Testing FP16_Q7.8 minimal value = -128
        --------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set FP16_Q7.8 parameter min value = -128
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - FP16_Q7.8 parameter set to -128
                - Blackboard and filesystem values checked
        """
        log.D(self.test_TypeMin.__doc__)
        log.I("FP16_Q7.8 parameter min value = -128")
        value = "-128"
        hex_value = "0x8000"
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, log.E("when setting parameter %s : %s" % (self.param_name, err))
        assert out == "Done", log.F("when setting parameter %s : %s" % (self.param_name, out))
        #Check parameter value on blackboard
        out, err = self.pfw.sendCmd("getParameter", self.param_name, "")
        assert err == None, log.E("when setting parameter %s : %s" % (self.param_name, err))
        assert round(float(out), 3) == float(value), log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s"
                                                           % (self.param_name, value, out))
        #Check parameter value on filesystem
        assert commands.getoutput('cat $PFW_RESULT/FP16_Q7.8') == hex_value, log.F("FILESYSTEM : parameter update error")
        log.I("test OK")

    def test_TypeMin_Overflow(self):
        """
        Testing FP16_Q7.8 parameter value out of negative range
        -------------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set FP16_Q7.8 to -128.001
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - error detected
                - FP16_Q7.8 parameter not updated
                - Blackboard and filesystem values checked
        """
        log.D(self.test_TypeMin_Overflow.__doc__)
        log.I("FP16_Q7.8 parameter min value out of bounds = -128.001")
        value = "-128.001"
        param_check = commands.getoutput('cat $PFW_RESULT/FP16_Q7.8')
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, log.E("when setting parameter %s : %s" % (self.param_name, err))
        assert out != "Done", log.F("PFW : Error not detected when setting parameter %s out of bounds"
                                    % (self.param_name))
        #Check parameter value on filesystem
        assert commands.getoutput('cat $PFW_RESULT/FP16_Q7.8') == param_check, log.F("FILESYSTEM : Forbiden parameter change")
        log.I("test OK")

    def test_TypeMax(self):
        """
        Testing FP16_Q7.8 parameter maximum value
        -----------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set FP16_Q7.8 to 127.996
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - FP16_Q7.8 parameter set to 127.996
                - Blackboard and filesystem values checked
        """
        log.D(self.test_TypeMax.__doc__)
        log.I("FP16_Q7.8 parameter max value = 127.996")
        value = "127.996"
        hex_value = "0x7fff"
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, log.E("when setting parameter %s : %s" % (self.param_name, err))
        assert out == "Done", log.F("when setting parameter %s : %s" % (self.param_name, out))
        #Check parameter value on blackboard
        out, err = self.pfw.sendCmd("getParameter", self.param_name, "")
        assert err == None, log.E("when setting parameter %s : %s" % (self.param_name, err))
        assert round(float(out), 3) == float(value), log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s"
                                                           % (self.param_name, value, out))
        #Check parameter value on filesystem
        assert commands.getoutput('cat $PFW_RESULT/FP16_Q7.8') == hex_value, log.F("FILESYSTEM : parameter update error")
        log.I("test OK")

    def test_TypeMax_Overflow(self):
        """
        Testing FP16_Q7.8 parameter value out of positive range
        -------------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set FP16_Q7.8 to 127.997
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - error detected
                - FP16_Q7.8 parameter not updated

        """
        log.D(self.test_TypeMax_Overflow.__doc__)
        log.I("FP16_Q7.8 parameter max value out of bounds = 127.997")
        value = "127.997"
        param_check = commands.getoutput('cat $PFW_RESULT/FP16_Q7.8')
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, log.E("when setting parameter %s : %s" % (self.param_name, err))
        assert out != "Done", log.F("PFW : Error not detected when setting parameter %s out of bounds" % (self.param_name))
        #Check parameter value on filesystem
        assert commands.getoutput('cat $PFW_RESULT/FP16_Q7.8') == param_check, log.F("FILESYSTEM : Forbiden parameter change")
        log.I("test OK")
