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

import commands, string, random
from Util.PfwUnitTestLib import PfwTestCase
from Util import ACTLogging
log=ACTLogging.Logger()

# Test of type UINT8_S - range [-100, 100]
class TestCases(PfwTestCase):

    def setUp(self):
        self.param_name = "/Test/Test/TEST_DIR/STR_CHAR128"
        self.pfw.sendCmd("setTuningMode", "on")
        self.size_max=128

    def tearDown(self):
        self.pfw.sendCmd("setTuningMode", "off")

    def test_Digits_String_Case(self):
        """
        |============================================================|
        | Testing data types - String                                |
        |                      max number of char = 128              |
        |============================================================|
        | File    : tSTRING_128.py                                   |
        | Version : 01                                               |
        |                                                            |
        | Test cases :                                               |
        | - STR_CHAR128 parameter nominal value = string_Conf_0      |
        | - STR_CHAR128 parameter empty value = ''                   |
        | - STR_CHAR128 parameter full value = generate randomly 128 |
        | letters characters                                         |
        | - STR_CHAR128 parameter space character value = test string|
        | - STR_CHAR128 parameter full digits value = generate       |
        | randomly 128 digits char                                   |
        | - STR_CHAR128 parameter oversize value = generate randomly |
        | 129 char                                                   |
        |                                                            |
        |============================================================|
        | STR_CHAR128 parameter in digits case = 128 digits char     |
        |============================================================|
        | Test Case description :                                    |
        | - STR_CHAR128 parameter in digit case = 128 digits char    |
        | Tested commands :                                          |
        | * setParameter                                             |
        | - getParameter                                             |
        | Expected result :                                          |
        | - STR_CHAR128 parameter set to the same 128 digits char    |
        |   (blackboard and filesystem values checked)               |
        |============================================================|
        """
        log.D(self.test_Digits_String_Case.__doc__)
        log.I("STR_CHAR128 parameter initial state = string_Conf_0")
        value = ""
        for i in range(self.size_max-1):
            value=value+str(random.choice(string.digits))
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, log.E("when setting parameter %s -> %s" % (self.param_name, err))
        assert out == "Done", log.F(out)
        #Check parameter value on blackboard
        out, err = self.pfw.sendCmd("getParameter", self.param_name, "")
        assert err == None, log.E("when getting parameter %s -> %s" % (self.param_name, err))
        assert out == value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name, value, out))
        #Check parameter value on filesystem
        assert commands.getoutput('cat $PFW_RESULT/STR_CHAR128') == value, log.F("FILESYSTEM : parameter update error")

    def test_Empty_String_Case(self):
        """
        |============================================================|
        | STR_CHAR128 parameter empty string = \'\'                    |
        |============================================================|
        | Test Case description :                                    |
        | - STR_CHAR128 parameter in empty string case = \'\'          |
        | Tested commands :                                          |
        | * setParameter                                             |
        | - getParameter                                             |
        | Expected result :                                          |
        | - STR_CHAR128 parameter set empty                          |
        |   (blackboard and filesystem values checked)               |
        |============================================================|
        """
        log.D(self.test_Empty_String_Case.__doc__)
        log.I("STR_CHAR128 parameter empty string = \'\'")
        value = "\"\""
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, log.E("when setting parameter %s -> %s" % (self.param_name, err))
        assert out == "Done", log.F(out)
        #Check parameter value on blackboard
        out, err = self.pfw.sendCmd("getParameter", self.param_name, "")
        assert err == None, log.E("when getting parameter %s -> %s" % (self.param_name, err))
        assert out == "", log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name, value, out))
        #Check parameter value on filesystem
        assert commands.getoutput('cat $PFW_RESULT/STR_CHAR128') == "", log.F("FILESYSTEM : parameter update error")

    def test_OverSize_String_Case(self):
        """
        |============================================================|
        | STR_CHAR128 parameter oversize                             |
        |============================================================|
        | Test Case description :                                    |
        | - STR_CHAR128 parameter in oversize case = 129 random char |
        | Tested commands :                                          |
        | * setParameter                                             |
        | - getParameter                                             |
        | Expected result :                                          |
        | - error detected                                           |
        | - STR_CHAR128 parameter not updated                        |
        |============================================================|
        """
        log.D(self.test_OverSize_String_Case.__doc__)
        log.I("STR_CHAR128 parameter size max=128 character")
        value=""
        for i in range(self.size_max+1):
            value=value+str(random.choice(string.letters))
        param_check = commands.getoutput('cat $PFW_RESULT/STR_CHAR128')
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, log.E("when setting parameter %s -> %s" % (self.param_name, err))
        assert out != "Done", log.F("Error not detected when setting parameter %s over size" % (self.param_name))
        #Check parameter value on filesystem
        assert commands.getoutput('cat $PFW_RESULT/STR_CHAR128') == param_check, log.F("FILESYSTEM : Forbiden parameter change")

    def test_Full_Letters_String_Case(self):
        """
        |============================================================|
        | STR_CHAR128 parameter full size test case                  |
        |============================================================|
        | Test Case description :                                    |
        | - STR_CHAR128 parameter in fullsize case = 128 random char |
        | Tested commands :                                          |
        | * setParameter                                             |
        | - getParameter                                             |
        | Expected result :                                          |
        | - STR_CHAR128 parameter set to the same 128 letters char   |
        |   (blackboard and filesystem values checked)               |
        |============================================================|
        """
        log.D(self.test_Full_Letters_String_Case.__doc__)
        log.I("STR_CHAR128 parameter initial state : string")
        value = ""
        for i in range(self.size_max-1):
            value=value+str(random.choice(string.letters))
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, log.E("when setting parameter %s -> %s" % (self.param_name, err))
        assert out == "Done", log.F("Expected : Done, result : %s" % (out))
        #Check parameter value on blackboard
        out, err = self.pfw.sendCmd("getParameter", self.param_name, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name, err))
        assert out == value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name, value, out))
        #Check parameter value on filesystem
        assert commands.getoutput('cat $PFW_RESULT/STR_CHAR128') == value, log.F("FILESYSTEM : parameter update error")

    def test_Nominal_String_Case(self):
        """
        |============================================================|
        | STR_CHAR128 parameter Nominal test case                    |
        |============================================================|
        | Test Case description :                                    |
        | - STR_CHAR128 parameter in nominal case = TestString       |
        | Tested commands :                                          |
        | * setParameter                                             |
        | - getParameter                                             |
        | Expected result :                                          |
        | - STR_CHAR128 parameter set to TestString                  |
        |   (blackboard and filesystem values checked)               |
        |============================================================|
        """
        log.D(self.test_Nominal_String_Case.__doc__)
        log.I("STR_CHAR128 parameter nominal string = TestString")
        value = "TestString"
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, log.E("When setting parameter %s -> %s" % (self.param_name, err))
        assert out == "Done", log.F("Expected : Done, found : %s" % (out))
        #Check parameter value on blackboard
        out, err = self.pfw.sendCmd("getParameter", self.param_name, "")
        assert err == None, log.E("When setting parameter %s -> %s" % (self.param_name, err))
        assert out == value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name, value, out))
        #Check parameter value on filesystem
        assert commands.getoutput('cat $PFW_RESULT/STR_CHAR128') == value, log.F("FILESYSTEM : parameter update error")

    def test_Punctuation_Empty_Parenthese_String_Case(self):
        """
        |============================================================|
        | STR_CHAR128 parameter empty Parenthese char test case      |
        |============================================================|
        | Test Case description :                                    |
        | - STR_CHAR128 parameter = TestParenthese()                 |
        | Tested commands :                                          |
        | * setParameter                                             |
        | - getParameter                                             |
        | Expected result :                                          |
        | - Not Determined now                                       |
        |============================================================|
        """
        log.D(self.test_Punctuation_Empty_Parenthese_String_Case.__doc__)
        value = "ParentheseTest()"
        log.I("STR_CHAR128 parameter Parenthese Char = %s" % (value))
        param_check = commands.getoutput('cat $PFW_RESULT/STR_CHAR128')
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, "'%s'" % (value))
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name, err))
        assert out == "Done", log.F("Expected : Done, found : %s" % (out))
        #Get parameter value
        out, err = self.pfw.sendCmd("getParameter", self.param_name)
        assert err == None, log.E("When getting parameter %s : %s" % (self.param_name, err))
        assert out == value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name, value, out))
        #Check parameter value on filesystem
        assert commands.getoutput('cat $PFW_RESULT/STR_CHAR128') == value, log.F("FILESYSTEM : parameter update error")

    def test_Punctuation_Full_Parenthese_String_Case(self):
        """
        |============================================================|
        | STR_CHAR128 parameter full Parenthese char test case       |
        |============================================================|
        | Test Case description :                                    |
        | - STR_CHAR128 parameter = TestParenthese(test)             |
        | Tested commands :                                          |
        | * setParameter                                             |
        | - getParameter                                             |
        | Expected result :                                          |
        | - Not Determined now                                       |
        |============================================================|
        """
        log.D(self.test_Punctuation_Full_Parenthese_String_Case.__doc__)
        value = "ParentheseTest(test)"
        log.I("STR_CHAR128 parameter Parenthese Char = %s" % (value))
        param_check = commands.getoutput('cat $PFW_RESULT/STR_CHAR128')
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, "'%s'" % value)
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name, err))
        assert out == "Done", log.F("Expected : Done, found : %s" % (out))
        #Get parameter value
        out, err = self.pfw.sendCmd("getParameter", self.param_name)
        assert err == None, log.E("When getting parameter %s : %s" % (self.param_name, err))
        assert out == value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name, value, out))
        #Check parameter value on filesystem
        assert commands.getoutput('cat $PFW_RESULT/STR_CHAR128') == value, log.F("FILESYSTEM : parameter update error")

    def test_SpaceChar_String_Case(self):
        """
        |============================================================|
        | STR_CHAR128 parameter space char test case                 |
        |============================================================|
        | Test Case description :                                    |
        | - STR_CHAR128 parameter = Test String                      |
        | Tested commands :                                          |
        | * setParameter                                             |
        | - getParameter                                             |
        | Expected result :                                          |
        | - Not Determined now                                       |
        |============================================================|
        """
        log.D(self.test_SpaceChar_String_Case.__doc__)
        value = "Test String"
        log.I("STR_CHAR128 parameter Parenthese Char = %s" % (value))
        value_check = "Test String"
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name, err))
        assert out == "Done", log.F("Expected : Done, found : %s" % (out))
       #Check parameter value on blackboard
        out, err = self.pfw.sendCmd("getParameter", self.param_name, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name, err))
        assert out == value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name, value_check, out))
        #Check parameter value on filesystem
        assert commands.getoutput('cat $PFW_RESULT/STR_CHAR128') == value_check, log.F("FILESYSTEM : parameter update error")
