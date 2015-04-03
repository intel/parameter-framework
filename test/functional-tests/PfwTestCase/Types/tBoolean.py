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
Boolean parameter type testcases.

List of tested functions :
--------------------------
    - [setParameter]  function
    - [getParameter] function


Test cases :
------------
    - Testing minimum
    - Testing maximum
    - Testing negative value
    - Testing overflow
"""

from Util.PfwUnitTestLib import PfwTestCase
from Util import ACTLogging
log=ACTLogging.Logger()

# Class containing SET/GET tests on a Boolean parameter
class TestCases(PfwTestCase):
    def setUp(self):
        self.param_name = "/Test/Test/TEST_DIR/BOOL"
        self.pfw.sendCmd("setTuningMode", "on")

    def tearDown(self):
        self.pfw.sendCmd("setTuningMode", "off")

    def testBooleanMaximum(self):
        """
        Testing maximum value for boolean parameter
        -------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - Set a boolean parameter to the max value 1
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - Boolean set to 1
        """
        log.D(self.testBooleanMaximum.__doc__)
        value = "1"
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name, err))
        assert out == "Done", log.F("When setting parameter %s : %s" % (self.param_name, out))
        out, err = self.pfw.sendCmd("getParameter", self.param_name, "")
        assert out == value, log.F("incorrect value for %s, expected: %s, found: %s" % (self.param_name, value, out))

    def testBooleanMinimum(self):
        """
        Testing minimum value for boolean parameter
        -------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - Set a boolean parameter to the min value 0
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - Boolean set to 0
        """
        log.D(self.testBooleanMinimum.__doc__)
        value = "0"
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name, err))
        assert out == "Done", log.F("When setting parameter %s : %s" % (self.param_name, out))
        out, err = self.pfw.sendCmd("getParameter", self.param_name, "")
        assert err == None, log.E("Error when setting parameter %s : %s" % (self.param_name, err))
        assert out == value, log.F("Incorrect value for %s, expected: %s, found: %s" % (self.param_name, value, out))

    def testBooleanNegative(self):
        """
        Testing negative value for boolean parameter
        --------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - Set a boolean parameter to -1
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - Error detected, boolean not updated
        """
        print self.testBooleanNegative.__doc__
        value = "-1"
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name, err))
        assert out != "Done", log.F("When setting parameter %s : %s" % (self.param_name, out))
        out, err = self.pfw.sendCmd("getParameter", self.param_name, "")
        assert out != value, log.F("incorrect value for %s, expected: %s, found: %s") % (self.param_name, value, out)


    def testBooleanOverflow(self):
        """
        Testing overflowed value for boolean parameter
        ----------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - Set a boolean parameter to 2
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - Error detected, boolean not updated
        """
        print self.testBooleanOverflow.__doc__
        value = "2"
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name, err))
        assert out != "Done", log.F("When setting parameter %s : %s" % (self.param_name, out))
        out, err = self.pfw.sendCmd("getParameter", self.param_name, "")
        assert out != value, log.F("incorrect value for %s, expected: %s, found: %s") % (self.param_name, value, out)
