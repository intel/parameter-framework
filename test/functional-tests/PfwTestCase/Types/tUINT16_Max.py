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
Integer parameter type testcases - UINT16_Max

List of tested functions :
--------------------------
    - [setParameter]  function
    - [getParameter] function

Initial Settings :
------------------
    UINT16_Max :
        - unsigned
        - size = 16
        - range : [0, 65535]

Test cases :
------------
    - UINT16_Max parameter min value = 0
    - UINT16_Max parameter min value out of bounds = -1
    - UINT16_Max parameter max value = 65535
    - UINT16_Max parameter max value out of bounds = 65536
    - UINT16_Max parameter in nominal case = 50
"""
import commands
from Util.PfwUnitTestLib import PfwTestCase
from Util import ACTLogging
log=ACTLogging.Logger()


# Test of type UINT16_Max - range [0, 65535]
class TestCases(PfwTestCase):
    def setUp(self):
        self.param_name = "/Test/Test/TEST_DIR/UINT16_Max"
        self.pfw.sendCmd("setTuningMode", "on")

    def tearDown(self):
        self.pfw.sendCmd("setTuningMode", "off")

    def test_Nominal_Case(self):
        """
        Testing UINT16_Max in nominal case = 50
        ---------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set UINT16_Max parameter in nominal case = 50
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - UINT16_Max parameter set to 50
                - Blackboard and filesystem values checked
        """
        log.D(self.test_Nominal_Case.__doc__)
        log.I("UINT16_Max parameter in nominal case = 50")
        value = "50"
        hex_value = "0x32"
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
        assert out == value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s"
                                   % (self.param_name, value, out))
        #Check parameter value on filesystem
        assert commands.getoutput('cat $PFW_RESULT/UINT16_Max') == hex_value, log.F("FILESYSTEM : parameter update error")
        log.I("test OK")

    def test_TypeMin(self):
        """
        Testing UINT16_Max minimal value = 0
        ------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set UINT16_Max parameter min value = 0
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - UINT16_Max parameter set to 0
                - Blackboard and filesystem values checked
        """
        log.D(self.test_TypeMin.__doc__)
        log.I("UINT16_Max parameter min value = 0")
        value = "0"
        hex_value = "0x0"
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
        assert out == value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s"
                                   % (self.param_name, value, out))
        #Check parameter value on filesystem
        assert commands.getoutput('cat $PFW_RESULT/UINT16_Max') == hex_value, log.F("FILESYSTEM : parameter update error")
        log.I("test OK")

    def test_TypeMin_Overflow(self):
        """
        Testing UINT16_Max parameter value out of negative range
        --------------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set UINT16_Max to -1
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - error detected
                - UINT16_Max parameter not updated
                - Blackboard and filesystem values checked
        """
        log.D(self.test_TypeMin_Overflow.__doc__)
        log.I("UINT16_Max parameter min value out of bounds = -1")
        value = "-1"
        param_check = commands.getoutput('cat $PFW_RESULT/UINT16_Max')
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, log.E("when setting parameter %s : %s"
                                  % (self.param_name, err))
        assert out != "Done", log.F("PFW : Error not detected when setting parameter %s out of bounds"
                                    % (self.param_name))
        #Check parameter value on filesystem
        assert commands.getoutput('cat $PFW_RESULT/UINT16_Max') == param_check, log.F("FILESYSTEM : Forbiden parameter change")
        log.I("test OK")

    def test_TypeMax(self):
        """
        Testing UINT16_Max parameter maximum value
        ------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set UINT16_Max to 65535
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - UINT16_Max parameter set to 65535
                - Blackboard and filesystem values checked
        """
        log.D(self.test_TypeMax.__doc__)
        log.I("UINT16_Max parameter max value = 65535")
        value = "65535"
        hex_value = "0xffff"
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
        assert out == value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s"
                                   % (self.param_name, value, out))
        #Check parameter value on filesystem
        assert commands.getoutput('cat $PFW_RESULT/UINT16_Max') == hex_value, log.F("FILESYSTEM : parameter update error")
        log.I("test OK")

    def test_TypeMax_Overflow(self):
        """
        Testing UINT16_Max parameter value out of positive range
        --------------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set UINT16_Max to 65536
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - error detected
                - UINT16_Max parameter not updated
                - Blackboard and filesystem values checked
        """
        log.D(self.test_TypeMax_Overflow.__doc__)
        log.I("UINT16_Max parameter max value out of bounds = 65536")
        value = "65536"
        param_check = commands.getoutput('cat $PFW_RESULT/UINT16_Max')
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, log.E("when setting parameter %s : %s"
                                  % (self.param_name, err))
        assert out != "Done", log.F("PFW : Error not detected when setting parameter %s out of bounds"
                                    % (self.param_name))
        #Check parameter value on filesystem
        assert commands.getoutput('cat $PFW_RESULT/UINT16_Max') == param_check, log.F("FILESYSTEM : Forbiden parameter change")
        log.I("test OK")
