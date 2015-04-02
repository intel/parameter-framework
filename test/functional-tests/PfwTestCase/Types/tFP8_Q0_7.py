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
Fixed-Point parameter type testcases - FP8_Q0.7

List of tested functions :
--------------------------
    - [setParameter]  function
    - [getParameter] function

Initial Settings :
------------------
    FP8_Q0.7 :
        - size = 8 bits
        - 0 integer bits, 7 fractionnal bits
        - range : [-1, 0.992188]

Test cases :
------------
    - FP8_Q0.7 parameter min value = -1
    - FP8_Q0.7 parameter min value out of bounds = -1.000001
    - FP8_Q0.7 parameter max value = 0.992188
    - FP8_Q0.7 parameter max value out of bounds = 0.992189
    - FP8_Q0.7 parameter in nominal case = 0.50
"""
import commands
import unittest
from Util.PfwUnitTestLib import PfwTestCase
from Util import ACTLogging
log=ACTLogging.Logger()

# Test of type FP8_Q0.7 - range [-1, 0.992188]
class TestCases(PfwTestCase):
    def setUp(self):
        self.param_name = "/Test/Test/TEST_DIR/FP8_Q0.7"
        self.pfw.sendCmd("setTuningMode", "on")
        self.type_name = "FP8_Q0.7"

    def tearDown(self):
        self.pfw.sendCmd("setTuningMode", "off")

    def test_Nominal_Case(self):
        """
        Testing FP8_Q0.7 in nominal case = 0.50
        ---------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set FP8_Q0.7 parameter in nominal case = 0.50
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - FP8_Q0.7 parameter set to 0.50
                - Blackboard and filesystem values checked
        """
        log.D(self.test_Nominal_Case.__doc__)
        value = "0.50"
        hex_value = "0x40"
        log.I("Setting %s to value %s" % (self.type_name, value))
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, log.E("when setting parameter %s : %s"
                                  % (self.param_name, err))
        assert out == "Done", log.F("when setting parameter %s : %s"
                                  % (self.param_name, out))
        #Check parameter value on blackboard
        out, err = self.pfw.sendCmd("getParameter", self.param_name, "")
        assert err == None, log.E("when setting parameter %s : %s"
                                  % (self.param_name, err))
        assert round(float(out), 2) == float(value), log.F("BLACKBOARD - Incorrect value for %s, expected: %s, found: %s"
                                                        % (self.param_name, value, out))
        #Check parameter value on filesystem
        assert commands.getoutput('cat $PFW_RESULT/FP8_Q0.7') == hex_value, log.F("FILESYSTEM - parameter update error")
        log.I("test OK")

    def test_TypeMin(self):
        """
        Testing FP8_Q0.7 minimal value = -1
        -----------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set FP8_Q0.7 parameter min value = -1
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - FP8_Q0.7 parameter set to -1
                - Blackboard and filesystem values checked
        """
        log.D(self.test_TypeMin.__doc__)
        value = "-1"
        hex_value = "0x80"
        log.I("Setting %s to value %s" % (self.type_name, value))
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, log.E("when setting parameter %s : %s"
                                  % (self.param_name, err))
        assert out == "Done", log.F("when setting parameter %s : %s"
                                  % (self.param_name, out))
        #Check parameter value on blackboard
        out, err = self.pfw.sendCmd("getParameter", self.param_name, "")
        assert err == None, log.E("when setting parameter %s : %s"
                                  % (self.param_name, err))
        assert round(float(out), 6) == float(value), log.F("BLACKBOARD - Incorrect value for %s, expected: %s, found: %s"
                                                           % (self.param_name, value, out))
        #Check parameter value on filesystem
        assert commands.getoutput('cat $PFW_RESULT/FP8_Q0.7') == hex_value, log.F("FILESYSTEM - parameter update error")
        log.I("test OK")

    def test_TypeMin_Overflow(self):
        """
        Testing FP8_Q0.7 parameter value out of negative range
        ------------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set FP8_Q0.7 to -1.00001
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - error detected
                - FP8_Q0.7 parameter not updated
                - Blackboard and filesystem values checked
        """
        log.D(self.test_TypeMin_Overflow.__doc__)
        value = "-1.000001"
        param_check = commands.getoutput('cat $PFW_RESULT/FP8_Q0.7')
        log.I("Setting %s to value %s" % (self.type_name, value))
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, log.E("when setting parameter %s : %s"
                                  % (self.param_name, err))
        assert out != "Done", log.F("Error not detected when setting parameter %s out of bounds"
                                    % (self.param_name))
        #Check parameter value on filesystem
        assert commands.getoutput('cat $PFW_RESULT/FP8_Q0.7') == param_check, log.F("FILESYSTEM - Forbiden parameter change")
        log.I("test OK")

    @unittest.expectedFailure
    def test_TypeMax(self):
        """
        Testing FP8_Q0.7 parameter maximum value
        ----------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set FP8_Q0.7 to 0.992188
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - FP8_Q0.7 parameter set to 0.992188
                - Blackboard and filesystem values checked
        """
        log.D(self.test_TypeMax.__doc__)
        value = "0.992188"
        hex_value = "0x7f"
        log.I("Setting %s to value %s" % (self.type_name, value))
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, log.E("when setting parameter %s : %s"
                                  % (self.param_name, err))
        assert out == "Done", log.F("when setting parameter %s : %s"
                                  % (self.param_name, out))
        #Check parameter value on blackboard
        out, err = self.pfw.sendCmd("getParameter", self.param_name, "")
        assert err == None, log.E("when setting parameter %s : %s"
                                  % (self.param_name, err))
        assert round(float(out), 6) == float(value), "ERROR : BLACKBOARD - Incorrect value for %s, expected: %s, found: %s" % (self.param_name, value, out)
        #Check parameter value on filesystem
        assert commands.getoutput('cat $PFW_RESULT/FP8_Q0.7') == hex_value, "ERROR : FILESYSTEM - parameter update error"
        log.I("test OK")

    def test_TypeMax_Overflow(self):
        """
        Testing FP8_Q0.7 parameter value out of positive range
        ------------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set FP8_Q0.7 to 0.992189
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - error detected
                - FP8_Q0.7 parameter not updated
                - Blackboard and filesystem values checked
        """
        log.D(self.test_TypeMax_Overflow.__doc__)
        value = "0.992189"
        param_check = commands.getoutput('cat $PFW_RESULT/FP8_Q0.7')
        log.I("Setting %s to value %s" % (self.type_name, value))
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, log.E("when setting parameter %s : %s"
                                  % (self.param_name, err))
        assert out != "Done", log.F("Error not detected when setting parameter %s out of bounds"
                                    % (self.param_name))
        #Check parameter value on filesystem
        assert commands.getoutput('cat $PFW_RESULT/FP8_Q0.7') == param_check, log.F("FILESYSTEM - Forbiden parameter change")
        log.I("test OK")
