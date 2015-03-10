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
Integer parameter type testcases - INT8_Max

List of tested functions :
--------------------------
    - [setParameter]  function
    - [getParameter] function

Initial Settings :
------------------
    INT8_Max :
        - size = 8
        - range : [-128, 127]

Test cases :
------------
    - INT8_Max parameter min value = -128
    - INT8_Max parameter min value out of bounds = -129
    - INT8_Max parameter max value = 127
    - INT8_Max parameter max value out of bounds = 128
    - INT8_Max parameter in nominal case = 50
"""
import commands
from Util.PfwUnitTestLib import PfwTestCase
from Util import ACTLogging
log=ACTLogging.Logger()

# Test of type INT8_Max - range [-128, 127]
class TestCases(PfwTestCase):
    def setUp(self):
        self.param_name = "/Test/Test/TEST_DIR/INT8_Max"
        self.pfw.sendCmd("setTuningMode", "on")

    def tearDown(self):
        self.pfw.sendCmd("setTuningMode", "off")

    def test_Nominal_Case(self):
        """
        Testing INT8_Max in nominal case = 50
        -------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set INT8_Max parameter in nominal case = 50
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - INT8_Max parameter set to 50
                - Blackboard and filesystem values checked
        """
        print self.test_Nominal_Case.__doc__
        print "INFO : INT8_Max parameter in nominal case = 50"
        value = "50"
        hex_value = "0x32"
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, "Error when setting parameter %s : %s" % (self.param_name, err)
        assert out == "Done", out
        #Check parameter value on blackboard
        out, err = self.pfw.sendCmd("getParameter", self.param_name, "")
        assert err == None, "Error when setting parameter %s : %s" % (self.param_name, err)
        assert out == value, "BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name, value, out)
        #Check parameter value on filesystem
        assert commands.getoutput('cat $PFW_RESULT/INT8_Max') == hex_value, "FILESYSTEM : parameter update error"
        print "INFO : test OK"

    def test_TypeMin(self):
        """
        Testing INT8_Max minimal value = -128
        -------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set INT8_Max parameter min value = -128
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - INT8_Max parameter set to -128
                - Blackboard and filesystem values checked
        """
        print self.test_TypeMin.__doc__
        print "INFO : INT8_Max parameter min value = -128"
        value = "-128"
        hex_value = "0x80"
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, "Error when setting parameter %s : %s" % (self.param_name, err)
        assert out == "Done", out
        #Check parameter value on blackboard
        out, err = self.pfw.sendCmd("getParameter", self.param_name, "")
        assert err == None, "PFW : Error when setting parameter %s : %s" % (self.param_name, err)
        assert out == value, "BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name, value, out)
        #Check parameter value on filesystem
        assert commands.getoutput('cat $PFW_RESULT/INT8_Max') == hex_value, "FILESYSTEM : parameter update error"
        print "INFO : test OK"

    def test_TypeMin_Overflow(self):
        """
        Testing INT8_Max parameter value out of negative range
        ------------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set INT8_Max to -129
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - error detected
                - INT8_Max parameter not updated
                - Blackboard and filesystem values checked
        """
        print self.test_TypeMin_Overflow.__doc__
        print "INFO : INT8_Max parameter min value out of bounds = -129"
        value = "-129"
        param_check = commands.getoutput('cat $PFW_RESULT/INT8_Max')
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, "Error when setting parameter %s : %s" % (self.param_name, err)
        assert out != "Done", "PFW : Error not detected when setting parameter %s out of bounds" % (self.param_name)
        #Check parameter value on filesystem
        assert commands.getoutput('cat $PFW_RESULT/INT8_Max') == param_check, "FILESYSTEM : Forbiden parameter change"
        print "INFO : test OK"

    def test_TypeMax(self):
        """
        Testing INT8_Max parameter maximum value
        ----------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set INT8_Max to 127
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - INT8_Max parameter set to 127
                - Blackboard and filesystem values checked
        """
        print self.test_TypeMax.__doc__
        print "INFO : INT8_Max parameter max value = 127"
        value = "127"
        hex_value = "0x7f"
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, "Error when setting parameter %s : %s" % (self.param_name, err)
        assert out == "Done", out
        #Check parameter value on blackboard
        out, err = self.pfw.sendCmd("getParameter", self.param_name, "")
        assert err == None, "Error when setting parameter %s : %s" % (self.param_name, err)
        assert out == value, "BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name, value, out)
        #Check parameter value on filesystem
        assert commands.getoutput('cat $PFW_RESULT/INT8_Max') == hex_value, "FILESYSTEM : parameter update error"
        print "INFO : test OK"

    def test_TypeMax_Overflow(self):
        """
        Testing INT8_Max parameter value out of positive range
        ------------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set INT8_Max to 128
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - error detected
                - INT8_Max parameter not updated
                - Blackboard and filesystem values checked
        """
        print self.test_TypeMax_Overflow.__doc__
        print "INFO : INT8_Max parameter max value out of bounds = 128"
        value = "128"
        param_check = commands.getoutput('cat $PFW_RESULT/INT8_Max')
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, "Error when setting parameter %s : %s" % (self.param_name, err)
        assert out != "Done", "PFW : Error not detected when setting parameter %s out of bounds" % (self.param_name)
        #Check parameter value on filesystem
        assert commands.getoutput('cat $PFW_RESULT/INT8_Max') == param_check, "FILESYSTEM : Forbiden parameter change"
        print "INFO : test OK"
