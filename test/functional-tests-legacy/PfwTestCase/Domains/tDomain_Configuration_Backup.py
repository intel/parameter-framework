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
Save and restore configuration testcases

List of tested functions :
--------------------------
    - [saveConfiguration]  function
    - [restoreConfiguration] function

Test cases :
------------
    - Testing nominal case
    - Testing saveConfiguration errors
    - Testing restoreConfiguration errors
"""

from Util.PfwUnitTestLib import PfwTestCase
from Util import ACTLogging
log=ACTLogging.Logger()

# Test of Domains - save/restore configuration
class TestCases(PfwTestCase):
    def setUp(self):
        self.pfw.sendCmd("setTuningMode", "on")
        self.domain_name = "Domain_0"
        self.conf_1 = "Conf_0"
        self.conf_2 = "Conf_1"
        self.param_name = "/Test/Test/TEST_DIR/INT8"

    def tearDown(self):
        self.pfw.sendCmd("setTuningMode", "off")

    def test_Nominal_Case(self):
        """
        Testing nominal case
        --------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - save a configuration
                - restore a configuration
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [saveConfiguration] function
                - [restoreConfiguration] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
            - all operations succeed
        """
        log.D(self.test_Nominal_Case.__doc__)

        # Saving original parameter value
        log.I("restoring configuration %s from domain %s" % (self.conf_1, self.domain_name))
        out, err = self.pfw.sendCmd("restoreConfiguration", self.domain_name, self.conf_1)
        assert err == None, "Error when restoring configuration %s from domain %s : %s" % (self.conf_1, self.domain_name, err)
        assert out == "Done", out
        out, err = self.pfw.sendCmd("getParameter", self.param_name)
        assert err == None, "Error when getting parameter %s : %s" % (self.param_name, err)
        Param_saved_1 = int(out)
        log.I("saved parameter %s value on %s from domain %s = %s" % (self.param_name, self.conf_1, self.domain_name, Param_saved_1))

        # Modifying parameter value
        log.I("modifying parameter %s value on configuration %s from domain %s" % (self.param_name, self.conf_1, self.domain_name))
        out, err = self.pfw.sendCmd("setParameter", self.param_name, str(Param_saved_1+1))
        assert err == None, "Error when setting parameter %s : %s" % (self.param_name, err)
        log.I("new parameter %s value = %s in place of %s" % (self.param_name, str(Param_saved_1+1), Param_saved_1))

        # Saving new parameter value
        log.I("saving configuration %s from domain %s" % (self.conf_1, self.domain_name))
        out, err = self.pfw.sendCmd("saveConfiguration", self.domain_name, self.conf_1)
        assert err == None, "Error when saving configuration %s from domain %s : %s" % (self.conf_1, self.domain_name, err)
        assert out == "Done", out
        out, err = self.pfw.sendCmd("getParameter", self.param_name)
        assert err == None, "Error when getting parameter %s : %s" % (self.param_name, err)
        Param_saved_1 = int(out)
        log.I("new saved parameter %s value on %s from domain %s = %s" % (self.param_name, self.conf_1, self.domain_name, Param_saved_1))

        # Modifying and restoring parameter value
        log.I("modifying parameter %s value on configuration %s from domain %s" % (self.param_name, self.conf_1, self.domain_name))
        out, err = self.pfw.sendCmd("setParameter", self.param_name, str(Param_saved_1+1))
        assert err == None, "Error when setting parameter %s : %s" % (self.param_name, err)
        out, err = self.pfw.sendCmd("getParameter", self.param_name)
        assert err == None, "Error when getting parameter %s : %s" % (self.param_name, err)
        Param_saved_2 = int(out)
        log.I("new parameter %s value on %s = %s in place of %s" % (self.param_name, self.conf_1, str(Param_saved_2), Param_saved_1))
        log.I("restoring configuration %s from domain %s" % (self.conf_1, self.domain_name))
        out, err = self.pfw.sendCmd("restoreConfiguration", self.domain_name, self.conf_1)
        assert err == None, "Error when restoring configuration %s from domain %s : %s" % (self.conf_1, self.domain_name, err)
        assert out == "Done", out
        out, err = self.pfw.sendCmd("getParameter", self.param_name)
        assert err == None, "Error when getting parameter %s : %s" % (self.param_name, err)
        Param_saved_2 = int(out)
        assert Param_saved_2 == Param_saved_1, "Error when restoring configuration %s from domain %s" % (self.conf_1, self.domain_name)
        log.I("saving and restoring configuration works fine")

    def test_Save_Config_Error(self):
        """
        Testing saveConfiguration error
        -------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - save a configuration with a missing argument
                - save a configuration with a wrong domain name
                - save a configuration with a wrong configuration name
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [saveConfiguration] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
            - errors correctly detected
        """
        log.D(self.test_Save_Config_Error.__doc__)
        # Saving original parameter value and setting a new value to parameter for testing purpose
        log.I("restoring configuration %s from domain %s" % (self.conf_1, self.domain_name))
        out, err = self.pfw.sendCmd("restoreConfiguration", self.domain_name, self.conf_1)
        assert err == None, "Error when restoring configuration %s from domain %s : %s" % (self.conf_1, self.domain_name, err)
        assert out == "Done", out
        out, err = self.pfw.sendCmd("getParameter", self.param_name)
        assert err == None, "Error when getting parameter %s : %s" % (self.param_name, err)
        Param_saved_1 = int(out)
        log.I("saved parameter %s value on %s from domain %s = %s" % (self.param_name, self.conf_1, self.domain_name, Param_saved_1))
        log.I("modifying parameter %s value on configuration %s from domain %s" % (self.param_name, self.conf_1, self.domain_name))
        out, err = self.pfw.sendCmd("setParameter", self.param_name, str(Param_saved_1+1))
        assert err == None, "Error when setting parameter %s : %s" % (self.param_name, err)
        log.I("new parameter %s value = %s in place of %s" % (self.param_name, str(Param_saved_1+1), Param_saved_1))

        # Configuration saving errors
        log.I("saving configuration error test cases :")
        log.I("saving configuration with a missing argument")
        out, err = self.pfw.sendCmd("saveConfiguration", self.domain_name)
        assert err == None, "ERROR : Error when saving configuration with a missing argument"
        assert out != "Done", "ERROR : Error not detected when saving configuration with a missing argument"
        log.I("saving configuration with a wrong domain name")
        out, err = self.pfw.sendCmd("saveConfiguration", "Wrong_Domain_Name", self.conf_1)
        assert err == None, "ERROR : Error when saving configuration with a wrong domain name"
        assert out != "Done", "ERROR : Error not detected when saving configuration with a wrong domain name"
        log.I("saving configuration with a wrong configuration name")
        out, err = self.pfw.sendCmd("saveConfiguration", self.domain_name, "Wrong_Configuration_Name")
        assert err == None, "ERROR : Error when saving configuration with a wrong configuration name"
        assert out != "Done", "ERROR : Error not detected when saving configuration with a wrong configuration name"
        log.I("saving configuration error test cases : errors correctly detected")

        # Checking that no error has affected original configuration save
        log.I("restoring configuration %s from domain %s" % (self.conf_1, self.domain_name))
        out, err = self.pfw.sendCmd("restoreConfiguration", self.domain_name, self.conf_1)
        assert err == None, "error when restoring configuration %s from domain %s : %s" % (self.conf_1, self.domain_name, err)
        assert out == "Done", out
        out, err = self.pfw.sendCmd("getParameter", self.param_name)
        assert err == None, "error when getting parameter %s : %s" % (self.param_name, err)
        Param_saved_2 = int(out)
        assert Param_saved_2 == Param_saved_1, "error when restoring configuration %s from domain %s, parameter %s affected by configuration saving error" % (self.conf_1, self.domain_name, Param_saved_1)
        log.I("Test passed : saving errors correctly detected, no impact on previously saved configuration")

    def test_Restore_Config_Error(self):
        """
        Testing restoreConfiguration error
        ----------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - restore a configuration with a missing argument
                - restore a configuration with a wrong domain name
                - restore a configuration with a wrong configuration name
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [restoreConfiguration] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
            - errors correctly detected
            - configuration's parameters not affected by errors
        """

        log.D(self.test_Restore_Config_Error.__doc__)
        # Saving original parameter value and setting a new value to parameter for testing purpose
        log.I("restore configuration %s from domain %s" % (self.conf_1, self.domain_name))
        out, err = self.pfw.sendCmd("restoreConfiguration", self.domain_name, self.conf_1)
        assert err == None, "Error when restoring configuration %s from domain %s : %s" % (self.conf_1, self.domain_name, err)
        assert out == "Done", out
        out, err = self.pfw.sendCmd("getParameter", self.param_name)
        assert err == None, "Error when getting parameter %s : %s" % (self.param_name, err)
        Param_saved_1 = int(out)
        log.I("saved parameter %s value on %s from domain %s = %s" % (self.param_name, self.conf_1, self.domain_name, Param_saved_1))
        log.I("modifying parameter %s value on configuration %s from domain %s" % (self.param_name, self.conf_1, self.domain_name))
        out, err = self.pfw.sendCmd("setParameter", self.param_name, str(Param_saved_1+1))
        assert err == None, "Error when setting parameter %s : %s" % (self.param_name, err)
        log.I("new parameter %s value = %s in place of %s" % (self.param_name, str(Param_saved_1+1), Param_saved_1))
        out, err = self.pfw.sendCmd("getParameter", self.param_name)
        assert err == None, "Error when getting parameter %s : %s" % (self.param_name, err)
        Param_saved_2 = int(out)

        # Configuration restore errors
        log.I("restoring configuration error test cases :")
        log.I("restoring configuration with a missing argument")
        out, err = self.pfw.sendCmd("restoreConfiguration", self.domain_name)
        assert err == None, "ERROR : Error when restoring configuration with a missing argument"
        assert out != "Done", "ERROR : Error not detected when restoring configuration with a missing argument"
        log.I("restoring configuration with a wrong domain name")
        out, err = self.pfw.sendCmd("restoreConfiguration", "Wrong_Domain_Name", self.conf_1)
        assert err == None, "ERROR : Error when restoring configuration with a wrong domain name"
        assert out != "Done", "ERROR : Error not detected when restoring configuration with a wrong domain name"
        log.I("restoring configuration with a wrong configuration name")
        out, err = self.pfw.sendCmd("restoreConfiguration", self.domain_name, "Wrong_Configuration_Name")
        assert err == None, "ERROR : Error when restoring configuration with a wrong configuration name"
        assert out != "Done", "ERROR : Error not detected when restoring configuration with a wrong configuration name"
        log.I("restoring configuration error test cases : errors correctly detected")

        # Checking that no error has affected configuration's parameter value
        out, err = self.pfw.sendCmd("getParameter", self.param_name)
        assert err == None, "error when getting parameter %s : %s" % (self.param_name, err)
        Param_saved_1 = int(out)
        assert Param_saved_2 == Param_saved_1, "error when restoring configuration %s from domain %s, parameter %s affected by configuration restoration error" % (self.conf_1, self.domain_name, Param_saved_1)
        log.I("Test passed : restoring errors correctly detected, no impact on previously modified configuration's parameter")
