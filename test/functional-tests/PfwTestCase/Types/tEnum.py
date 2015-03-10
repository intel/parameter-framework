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
Enum parameter type testcases.

List of tested functions :
--------------------------
    - [setParameter]  function
    - [getParameter] function

Initial Settings :
------------------
    Enum size = 8bits; 5 components :
        - max range [-127,128]

Test cases :
------------
    - Enum parameter nominal value = ENUM_NOMINAL : 5
    - Enum parameter min value = ENUM_MIN : -127
    - Enum parameter max value = ENUM_MAX : 128
    - Enum parameter out of bound value = ENUM_OOB : 255
    - Enum parameter out of size value = ENUM_OOS : 256
    - Enum parameter undefined value = UNDEF
"""
import commands
from Util.PfwUnitTestLib import PfwTestCase
from Util import ACTLogging
log=ACTLogging.Logger()

# Test of type UINT16 - range [0, 1000]
class TestCases(PfwTestCase):
    def setUp(self):
        self.param_name = "/Test/Test/TEST_TYPES/ENUM"
        self.filesystem_name="$PFW_RESULT/ENUM"
        self.pfw.sendCmd("setTuningMode", "on")

    def tearDown(self):
        self.pfw.sendCmd("setTuningMode", "off")


    def test_Nominal_Case(self):
        """
        Testing Enum parameter in nominal case
        --------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - ENUM parameter in nominal case = ENUM_NOMINAL
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - ENUM parameter set to ENUM_NOMINAL
                - FILESYSTEM set to 0x5
        """
        log.D(self.test_Nominal_Case.__doc__)
        value = "ENUM_NOMINAL"
        filesystem_value="0x5"
        log.I("Set parameter %s to %s"%(self.param_name,value))
        out,err = self.pfw.sendCmd("setParameter",self.param_name, value)
        assert err == None, log.E("setParameter %s %s : %s" % (self.param_name, value, err))
        assert out == "Done", log.F("setParameter %s %s - expected : Done : %s" % (self.param_name, value,out))
        log.I("Check Enum parameter state")
        out, err = self.pfw.sendCmd("getParameter",self.param_name)
        assert err == None, log.E("getParameter %s : %s" % (self.param_name, err))
        assert out == value, log.F("getParameter %s - expected : %s , found : %s" % (self.param_name,value,out))
        log.I("Check filesystem value")
        assert commands.getoutput("cat %s"%(self.filesystem_name)) == filesystem_value, log.F("FILESYSTEM : parameter update error for %s"%(self.param_name))

    def test_TypeMin(self):
        """
        Testing minimal value for Enum parameter
        ----------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - ENUM parameter in min case = ENUM_MIN
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - ENUM parameter set to ENUM_MIN
                - FILESYSTEM set to 0x80
        """
        log.D(self.test_TypeMin.__doc__)
        value = "ENUM_MIN"
        filesystem_value="0x80"
        log.I("Set parameter %s to %s"%(self.param_name,value))
        out,err = self.pfw.sendCmd("setParameter",self.param_name, value)
        assert err == None, log.E("setParameter %s %s : %s" % (self.param_name, value, err))
        assert out == "Done", log.F("setParameter %s %s - expected : Done : %s" % (self.param_name, value,out))
        log.I("Check Enum parameter state")
        out, err = self.pfw.sendCmd("getParameter",self.param_name)
        assert err == None, log.E("getParameter %s : %s" % (self.param_name, err))
        assert out == value, log.F("getParameter %s - expected : %s , found : %s" % (self.param_name,value,out))
        log.I("Check filesystem value")
        assert commands.getoutput("cat %s"%(self.filesystem_name)) == filesystem_value, log.F("FILESYSTEM : parameter update error for %s"%(self.param_name))

    def test_TypeMax(self):
        """
        Testing maximal value for Enum parameter
        ----------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - ENUM parameter in max case = ENUM_MAX
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - ENUM parameter set to ENUM_MAX
                - FILESYSTEM set to 0x7F
        """
        log.D(self.test_TypeMax.__doc__)
        value = "ENUM_MAX"
        filesystem_value="0x7f"
        log.I("Set parameter %s to %s"%(self.param_name,value))
        out,err = self.pfw.sendCmd("setParameter",self.param_name, value)
        assert err == None, log.E("setParameter %s %s : %s" % (self.param_name, value, err))
        assert out == "Done", log.F("setParameter %s %s - expected : Done : %s" % (self.param_name, value,out))
        log.I("Check Enum parameter state")
        out, err = self.pfw.sendCmd("getParameter",self.param_name)
        assert err == None, log.E("getParameter %s : %s" % (self.param_name, err))
        assert out == value, log.F("getParameter %s - expected : %s , found : %s" % (self.param_name,value,out))
        log.I("Check filesystem value")
        assert commands.getoutput("cat %s"%(self.filesystem_name)) == filesystem_value, log.F("FILESYSTEM : parameter update error for %s"%(self.param_name))

    def test_TypeUndefined(self):
        """
        Testing ENUM parameter in undefined reference case
        --------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - ENUM parameter = UNDEF
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - error detected, parameter must not change
                - FILESYSTEM must not change
        """
        log.D(self.test_TypeUndefined.__doc__)
        value = "UNDEF"
        log.I("Check parameter %s initial value"%(self.param_name))
        init_parameter_value, err=self.pfw.sendCmd("getParameter",self.param_name)
        init_filesystem_value=commands.getoutput("cat %s"%(self.filesystem_name))
        log.I("Set parameter %s to %s"%(self.param_name,value))
        out,err = self.pfw.sendCmd("setParameter",self.param_name, value)
        assert err == None, log.E("setParameter %s %s : %s" % (self.param_name, value, err))
        assert out != "Done", log.F("Error not detected when setParameter %s %s" % (self.param_name, value))
        log.I("Check Enum parameter state")
        out, err = self.pfw.sendCmd("getParameter",self.param_name)
        assert err == None, log.E("getParameter %s : %s" % (self.param_name, err))
        assert out == init_parameter_value, log.F("getParameter %s - expected : %s , found : %s" % (self.param_name,init_parameter_value,out))
        log.I("Check filesystem value")
        assert commands.getoutput("cat %s"%(self.filesystem_name)) == init_filesystem_value, log.F("FILESYSTEM : parameter update error for %s"%(self.param_name))

    def test_TypeOutOfBound(self):
        """
        Testing ENUM parameter in out of range case
        -------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - ENUM parameter in max case = ENUM_OOB : 255
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - error detected, parameter must not change
                - FILESYSTEM must not change
        """
        log.D(self.test_TypeOutOfBound.__doc__)
        value = "ENUM_OOB"
        log.I("Check parameter %s initial value"%(self.param_name))
        init_parameter_value, err=self.pfw.sendCmd("getParameter",self.param_name)
        init_filesystem_value=commands.getoutput("cat %s"%(self.filesystem_name))
        log.I("Set parameter %s to %s"%(self.param_name,value))
        out,err = self.pfw.sendCmd("setParameter",self.param_name, value)
        assert err == None, log.E("setParameter %s %s : %s" % (self.param_name, value, err))
        assert out != "Done", log.F("Error not detected when setParameter %s %s" % (self.param_name, value))
        log.I("Check Enum parameter state")
        out, err = self.pfw.sendCmd("getParameter",self.param_name)
        assert err == None, log.E("getParameter %s : %s" % (self.param_name, err))
        assert out == init_parameter_value, log.F("getParameter %s - expected : %s , found : %s" % (self.param_name,init_parameter_value,out))
        log.I("Check filesystem value")
        assert commands.getoutput("cat %s"%(self.filesystem_name)) == init_filesystem_value, log.F("FILESYSTEM : parameter update error for %s"%(self.param_name))

    def test_TypeOutOfSize(self):
        """
        Testing ENUM parameter in out of size case
        ------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - ENUM parameter in max case = ENUM_OOS : 256
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - error detected, parameter must not change
                - FILESYSTEM must not change
        """
        log.D(self.test_TypeOutOfBound.__doc__)
        value = "ENUM_OOS"
        log.I("Check parameter %s initial value"%(self.param_name))
        init_parameter_value, err=self.pfw.sendCmd("getParameter",self.param_name)
        init_filesystem_value=commands.getoutput("cat %s"%(self.filesystem_name))
        log.I("Set parameter %s to %s"%(self.param_name,value))
        out,err = self.pfw.sendCmd("setParameter",self.param_name, value)
        assert err == None, log.E("setParameter %s %s : %s" % (self.param_name, value, err))
        assert out != "Done", log.F("Error not detected when setParameter %s %s" % (self.param_name, value))
        log.I("Check Enum parameter state")
        out, err = self.pfw.sendCmd("getParameter",self.param_name)
        assert err == None, log.E("getParameter %s : %s" % (self.param_name, err))
        assert out == init_parameter_value, log.F("getParameter %s - expected : %s , found : %s" % (self.param_name,init_parameter_value,out))
        log.I("Check filesystem value")
        assert commands.getoutput("cat %s"%(self.filesystem_name)) == init_filesystem_value, log.F("FILESYSTEM : parameter update error for %s"%(self.param_name))
