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
Integer parameter type testcases - INT32

List of tested functions :
--------------------------
    - [setParameter]  function
    - [getParameter] function

Initial Settings :
------------------
    INT32 :
        - size = 32
        - range : [-1000, 1000]

Test cases :
------------
    - INT32 parameter min value = -1000
    - INT32 parameter min value out of bounds = -1001
    - INT32 parameter max value = 1000
    - INT32 parameter max value out of bounds = 1001
    - INT32 parameter in nominal case = 50
"""
import commands
from Util.PfwUnitTestLib import PfwTestCase
from Util import ACTLogging
log=ACTLogging.Logger()

# Test of type INT32 - range [-1000, 1000]
class TestCases(PfwTestCase):
    def setUp(self):
        self.param_name = "/Test/Test/TEST_DIR/INT32"
        self.pfw.sendCmd("setTuningMode", "on")

    def tearDown(self):
        self.pfw.sendCmd("setTuningMode", "off")

    def test_Nominal_Case(self):
        """
        Testing INT32 in nominal case = 50
        ----------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set INT32 parameter in nominal case = 50
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - INT32 parameter set to 50
                - Blackboard and filesystem values checked
        """
        print self.test_Nominal_Case.__doc__
        print "INFO : INT32 parameter in nominal case = 50"
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
        assert commands.getoutput('cat $PFW_RESULT/INT32') == hex_value, "FILESYSTEM : parameter update error"
        print "INFO : test OK"

    def test_TypeMin(self):
        """
        Testing INT32 minimal value = -1000
        -----------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set INT32 parameter min value = -1000
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - INT32 parameter set to -1000
                - Blackboard and filesystem values checked
        """
        print self.test_TypeMin.__doc__
        print "INFO : INT32 parameter min value = -1000"
        value = "-1000"
        hex_value = "0xfffffc18"
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, "Error when setting parameter %s : %s" % (self.param_name, err)
        assert out == "Done", out
        #Check parameter value on blackboard
        out, err = self.pfw.sendCmd("getParameter", self.param_name, "")
        assert err == None, "PFW : Error when setting parameter %s : %s" % (self.param_name, err)
        assert out == value, "BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name, value, out)
        #Check parameter value on filesystem
        assert commands.getoutput('cat $PFW_RESULT/INT32') == hex_value, "FILESYSTEM : parameter update error"
        print "INFO : test OK"

    def test_TypeMin_Overflow(self):
        """
        Testing INT32 parameter value out of negative range
        ---------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set INT32 to -1001
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - error detected
                - INT32 parameter not updated
                - Blackboard and filesystem values checked
        """
        print self.test_TypeMin_Overflow.__doc__
        print "INFO : INT32 parameter min value out of bounds = -1001"
        value = "-1001"
        param_check = commands.getoutput('cat $PFW_RESULT/INT32')
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, "Error when setting parameter %s : %s" % (self.param_name, err)
        assert out != "Done", "PFW : Error not detected when setting parameter %s out of bounds" % (self.param_name)
        #Check parameter value on filesystem
        assert commands.getoutput('cat $PFW_RESULT/INT32') == param_check, "FILESYSTEM : Forbiden parameter change"
        print "INFO : test OK"

    def test_TypeMax(self):
        """
        Testing INT32 parameter maximum value
        -------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set INT32 to 1000
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - INT32 parameter set to 1000
                - Blackboard and filesystem values checked
        """
        print self.test_TypeMax.__doc__
        print "INFO : INT32 parameter max value = 1000"
        value = "1000"
        hex_value = "0x3e8"
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, "Error when setting parameter %s : %s" % (self.param_name, err)
        assert out == "Done", out
        #Check parameter value on blackboard
        out, err = self.pfw.sendCmd("getParameter", self.param_name, "")
        assert err == None, "Error when setting parameter %s : %s" % (self.param_name, err)
        assert out == value, "BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name, value, out)
        #Check parameter value on filesystem
        assert commands.getoutput('cat $PFW_RESULT/INT32') == hex_value, "FILESYSTEM : parameter update error"
        print "INFO : test OK"

    def test_TypeMax_Overflow(self):
        """
        Testing INT32 parameter value out of positive range
        ---------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set INT32 to 1001
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - error detected
                - INT32 parameter not updated
                - Blackboard and filesystem values checked
        """
        print self.test_TypeMax_Overflow.__doc__
        print "INFO : INT32 parameter max value out of bounds = 1001"
        value = "1001"
        param_check = commands.getoutput('cat $PFW_RESULT/INT32')
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, "Error when setting parameter %s : %s" % (self.param_name, err)
        assert out != "Done", "PFW : Error not detected when setting parameter %s out of bounds" % (self.param_name)
        #Check parameter value on filesystem
        assert commands.getoutput('cat $PFW_RESULT/INT32') == param_check, "FILESYSTEM : Forbiden parameter change"
        print "INFO : test OK"
