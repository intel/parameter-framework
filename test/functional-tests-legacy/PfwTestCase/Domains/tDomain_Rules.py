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
Rules management testcases

List of tested functions :
--------------------------
    - [setRule]  function
    - [clearRule] function
    - [getRule] function

Test cases :
------------
    - Testing clearRule errors
    - Testing setRule errors
    - Testing getRule errors
    - Testing nominal case
"""
from Util.PfwUnitTestLib import PfwTestCase
from Util import ACTLogging
log=ACTLogging.Logger()

# Test of Domains - Rules
class TestCases(PfwTestCase):
    def setUp(self):
        self.pfw.sendCmd("setTuningMode", "on")
        self.domain_name = "domain_test"
        self.conf_1 = "conf_1"
        self.conf_2 = "conf_2"
        self.rule_1 = "Any{Crit_0 Includes State_0x2, Crit_1 IsNot State_1}"
        self.rule_2 = "All{Crit_0 Includes State_0x1, Crit_1 Is State_1}"
        self.rule_error_1 = "All{Crit_Error Includes State_0x1, Crit_1 Is State_1}"
        self.rule_error_2 = "Any{Crit_0 Includes State_0x2, Crit_0 IsNot State_1}"
        self.rule_error_3 = "Ay{Crit_0 Includes State_0x2, Crit_1 IsNot State_1}"
        self.rule_error_4 = "All{Crit_0 Includes State_0x4, Crit_1 IsNot State_1}"
        self.rule_error_5 = "All{Crit_0 Includes State_0x2, Crit_1 IsNot 1}"
        self.rule_error_nbr = 5

    def tearDown(self):
        self.pfw.sendCmd("setTuningMode", "off")

    def test_ClearRule_Errors(self):
        """
        Testing configuration creation error
        ------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - Clearing rule on a non-existent configuration
                - Clearing rule on a non-existent domain
                - Clearing rule with wrong parameters order
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [clearRule] function
                - [setRule] function
                - [getRule] function
                - [createDomain] function
                - [createConfiguration] function
                - [deleteDomain] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - all errors are detected
                - no rule is deleted
        """
        log.D(self.test_ClearRule_Errors.__doc__)
        # New domain creation for testing purpose
        log.I("New domain creation for testing purpose : %s" % (self.domain_name))
        log.I("command [createDomain]")
        out, err = self.pfw.sendCmd("createDomain",self.domain_name, "")
        assert out == "Done", out
        assert err == None, "ERROR : command [createDomain] - Error while creating domain %s" % (self.domain_name)
        log.I("command [createDomain] correctly executed")
        log.I("Domain %s created" % (self.domain_name))

        # New configurations creation for testing purpose
        log.I("New configuration %s creation for domain %s for testing purpose" % (self.conf_1,self.domain_name))
        log.I("command [createConfiguration]")
        out, err = self.pfw.sendCmd("createConfiguration",self.domain_name,self.conf_1)
        assert out == "Done", out
        assert err == None, "ERROR : command [createConfiguration] - Error while creating configuration %s" % (self.conf_1)
        log.I("command [createConfiguration] correctly executed")
        log.I("Configuration %s created for domain %s" % (self.conf_1,self.domain_name))
        log.I("New configuration %s creation for domain %s for testing purpose" % (self.conf_2,self.domain_name))
        log.I("command [createConfiguration]")
        out, err = self.pfw.sendCmd("createConfiguration",self.domain_name,self.conf_2)
        assert out == "Done", out
        assert err == None, "ERROR : command [createConfiguration] - Error while creating configuration %s" % (self.conf_2)
        log.I("command [createConfiguration] correctly executed")
        log.I("Configuration %s created for domain %s" % (self.conf_2,self.domain_name))

        # Applying rules to configurations
        log.I("Applying rules to configurations %s and %s from domain %s" % (self.conf_1,self.conf_2,self.domain_name))
        log.I("command [setRule]")
        out, err = self.pfw.sendCmd("setRule",self.domain_name,self.conf_1,self.rule_1)
        assert err == None, "ERROR : command [setRule] - Error while setting rule for configurations %s" % (self.conf_1)
        assert out == "Done", "FAIL : command [setRule] - Error while setting rule for configuration %s" % (self.conf_1)
        log.I("command [setRule] correctly executed")
        log.I("rule correctly created for configuration %s" % (self.conf_1))
        out, err = self.pfw.sendCmd("setRule",self.domain_name,self.conf_2,self.rule_2)
        assert err == None, "ERROR : command [setRule] - Error while setting rule for configurations %s" % (self.conf_2)
        assert out == "Done", "FAIL : command [setRule] - Error while setting rule for configuration %s" % (self.conf_2)
        log.I("command [setRule] correctly executed")
        log.I("rule correctly created for configuration %s" % (self.conf_2))

        # Clearing rule errors
        log.I("Clearing a rule on domain %s to a non-existent configuration" % (self.domain_name))
        log.I("command [clearRule]")
        out, err = self.pfw.sendCmd("clearRule",self.domain_name,"Wrong_Config_Name")
        assert err == None, "ERROR : command [clearRule] - Error while clearing rule on domain %s to a non-existent configuration" % (self.domain_name)
        assert out != "Done", "ERROR : command [clearRule] - Error not detected while clearing rule on domain %s to a non-existent configuration" % (self.domain_name)
        log.I("error correctly detected when clearing a rule to a non-existent configuration")
        log.I("Clearing a rule on a non-existent domain")
        log.I("command [clearRule]")
        out, err = self.pfw.sendCmd("clearRule","Wrong_Domain_Name",self.conf_2)
        assert err == None, "ERROR : command [clearRule] - Error while clearing rule on a non-existent domain"
        assert out != "Done", "ERROR : command [clearRule] - Error not detected while clearing rule on a non-existent domain"
        log.I("error correctly detected while clearing rule on a non-existent domain")
        log.I("Clearing a rule with wrong parameters order")
        log.I("command [clearRule]")
        out, err = self.pfw.sendCmd("clearRule",self.conf_1,self.domain_name)
        assert err == None, "ERROR : command [clearRule] - Error when clearing a rule with incorrect paramaters order"
        assert out != "Done", "ERROR : command [clearRule] - Error not detected when clearing a rule with incorrect paramaters order"
        log.I("error correctly detected when clearing a rule with incorrect paramaters order on domain %s and configuration %s" % (self.domain_name,self.conf_1))

        #Checking that no rule has been cleared
        out, err = self.pfw.sendCmd("getRule",self.domain_name,self.conf_1)
        assert out == self.rule_1, "FAIL : command [clearRule] - clearRule error has affected configuration %s" % (self.conf_1)
        out, err = self.pfw.sendCmd("getRule",self.domain_name,self.conf_2)
        assert out == self.rule_2, "FAIL : command [clearRule] - clearRule error has affected configuration %s" % (self.conf_2)
        log.I("command [ClearRule] correctly executed, no impact due to clearing errors")
        log.I("no rule removed from configurations %s and %s on domain %s" % (self.conf_1,self.conf_2,self.domain_name))

        # New domain deletion
        log.I("Domain %s deletion" % (self.domain_name))
        log.I("command [deleteDomain]")
        out, err = self.pfw.sendCmd("deleteDomain",self.domain_name, "")
        assert out == "Done", out
        assert err == None, "ERROR : command [deleteDomain] - Error while delting domain %s" % (self.domain_name)
        log.I("command [deleteDomain] correctly executed")
        log.I("Domain %s deleted" % (self.domain_name))

    def test_SetRule_Errors(self):
        """
        Testing setRule errors
        ----------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - Setting rule on a non-existent configuration
                - Setting rule on a non-existent domain
                - Setting various incorrect format rules
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setRule] function
                - [getRule] function
                - [createDomain] function
                - [createConfiguration] function
                - [deleteDomain] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - all errors are detected
                - no new rule is created
        """
        log.D(self.test_SetRule_Errors.__doc__)
        # New domain creation for testing purpose
        log.I("New domain creation for testing purpose : %s" % (self.domain_name))
        log.I("command [createDomain]")
        out, err = self.pfw.sendCmd("createDomain",self.domain_name, "")
        assert out == "Done", out
        assert err == None, "ERROR : command [createDomain] - Error while creating domain %s" % (self.domain_name)
        log.I("command [createDomain] correctly executed")
        log.I("Domain %s created" % (self.domain_name))

        # New configuration creation for testing purpose
        log.I("New configuration %s creation for domain %s for testing purpose" % (self.conf_1,self.domain_name))
        log.I("command [createConfiguration]")
        out, err = self.pfw.sendCmd("createConfiguration",self.domain_name,self.conf_1)
        assert out == "Done", out
        assert err == None, "ERROR : command [createConfiguration] - Error while creating configuration %s" % (self.conf_1)
        log.I("command [createConfiguration] correctly executed")
        log.I("Configuration %s created for domain %s" % (self.conf_1,self.domain_name))

        # setRule :basic error cases
        log.I("Applying a new rule on domain %s to a non-existent configuration" % (self.domain_name))
        log.I("command [setRule]")
        out, err = self.pfw.sendCmd("setRule",self.domain_name,"Wrong_Config_Name",self.rule_1)
        assert err == None, "ERROR : command [setRule] - Error while setting rule on domain %s to a non-existent configuration" % (self.domain_name)
        assert out != "Done", "ERROR : command [setRule] - Error not detected while setting rule on domain %s to a non-existent configuration" % (self.domain_name)
        log.I("error correctly detected when creating a rule to a non-existent configuration")
        log.I("Applying a new rule on a non-existent domain")
        log.I("command [setRule]")
        out, err = self.pfw.sendCmd("setRule","Wrong_Domain_Name",self.conf_1,self.rule_1)
        assert err == None, "ERROR : command [setRule] - Error while setting rule on a non-existent domain"
        assert out != "Done", "ERROR : command [setRule] - Error not detected while setting rule on a non-existent domain"
        log.I("error correctly detected while setting rule on a non-existent domain")
        log.I("Applying a new rule with incorrect format")
        log.I("command [setRule]")
        out, err = self.pfw.sendCmd("setRule",self.domain_name,self.conf_1,"Wrong_Rule_Format")
        assert err == None, "ERROR : command [setRule] - Error when setting incorrect format rule"
        assert out != "Done", "ERROR : command [setRule] - Error not detected when setting incorrect format rule"
        log.I("error correctly detected when setting incorrect format rule on domain %s and configuration %s" % (self.domain_name,self.conf_1))

        # setRule : various rules errors
        log.I("Various rules errors setting :")
        for index in range (self.rule_error_nbr):
            log.I("Rule error number %s" % (str(index)))
            rule_name = "".join(["self.rule_error_", "_", str(index)])
            out, err = self.pfw.sendCmd("setRule",self.domain_name,self.conf_1, rule_name)
            assert err == None, "ERROR : command [setRule] - Error when setting incorrect format rule %s" % (str(rule_name))
            assert out != "Done", "ERROR : command [setRule] - Error not detected when setting incorrect format rule %s" % (str(rule_name))
            log.I("error correctly detected when setting incorrect format rule on domain %s and configuration %s" % (self.domain_name,self.conf_1))

        #Checking that no rule has been created
        out, err = self.pfw.sendCmd("getRule",self.domain_name,self.conf_1)
        assert out == "<none>", "FAIL : command [setRule] - setRule not working for configuration %s" % (self.conf_1)
        log.I("command [setRule] correctly executed, no impact due to setting errors")
        log.I("no rule added to configurations %s on domain %s" % (self.conf_1,self.domain_name))

        # New domain deletion
        log.I("Domain %s deletion" % (self.domain_name))
        log.I("command [deleteDomain]")
        out, err = self.pfw.sendCmd("deleteDomain",self.domain_name, "")
        assert out == "Done", out
        assert err == None, "ERROR : command [deleteDomain] - Error while delting domain %s" % (self.domain_name)
        log.I("command [deleteDomain] correctly executed")
        log.I("Domain %s deleted" % (self.domain_name))

    def test_GetRule_Errors(self):
        """
        Testing getRule errors
        ----------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - Getting rule on a non-existent configuration
                - Getting rule on a non-existent domain
                - Getting rule with wrong parameters order
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [getRule] function
                - [setRule] function
                - [clearRule] function
                - [createDomain] function
                - [createConfiguration] function
                - [deleteDomain] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - all errors are detected
        """
        log.D(self.test_GetRule_Errors.__doc__)
        # New domain creation for testing purpose
        log.I("New domain creation for testing purpose : %s" % (self.domain_name))
        log.I("command [createDomain]")
        out, err = self.pfw.sendCmd("createDomain",self.domain_name, "")
        assert out == "Done", out
        assert err == None, "ERROR : command [createDomain] - Error while creating domain %s" % (self.domain_name)
        log.I("command [createDomain] correctly executed")
        log.I("Domain %s created" % (self.domain_name))

        # New configurations creation for testing purpose
        log.I("New configuration %s creation for domain %s for testing purpose" % (self.conf_1,self.domain_name))
        log.I("command [createConfiguration]")
        out, err = self.pfw.sendCmd("createConfiguration",self.domain_name,self.conf_1)
        assert out == "Done", out
        assert err == None, "ERROR : command [createConfiguration] - Error while creating configuration %s" % (self.conf_1)
        log.I("command [createConfiguration] correctly executed")
        log.I("Configuration %s created for domain %s" % (self.conf_1,self.domain_name))
        log.I("New configuration %s creation for domain %s for testing purpose" % (self.conf_2,self.domain_name))
        log.I("command [createConfiguration]")
        out, err = self.pfw.sendCmd("createConfiguration",self.domain_name,self.conf_2)
        assert out == "Done", out
        assert err == None, "ERROR : command [createConfiguration] - Error while creating configuration %s" % (self.conf_2)
        log.I("command [createConfiguration] correctly executed")
        log.I("Configuration %s created for domain %s" % (self.conf_2,self.domain_name))

        # Applying rules to configurations
        log.I("Applying rules to configurations %s and %s from domain %s" % (self.conf_1,self.conf_2,self.domain_name))
        log.I("command [setRule]")
        out, err = self.pfw.sendCmd("setRule",self.domain_name,self.conf_1,self.rule_1)
        assert err == None, "ERROR : command [setRule] - Error while setting rule for configurations %s" % (self.conf_1)
        assert out == "Done", "FAIL : command [setRule] - Error while setting rule for configuration %s" % (self.conf_1)
        log.I("command [setRule] correctly executed")
        log.I("rule correctly created for configuration %s" % (self.conf_1))
        out, err = self.pfw.sendCmd("setRule",self.domain_name,self.conf_2,self.rule_2)
        assert err == None, "ERROR : command [setRule] - Error while setting rule for configurations %s" % (self.conf_2)
        assert out == "Done", "FAIL : command [setRule] - Error while setting rule for configuration %s" % (self.conf_2)
        log.I("command [setRule] correctly executed")
        log.I("rule correctly created for configuration %s" % (self.conf_2))

        # Getting rule errors
        log.I("Getting a rule on domain %s from a non-existent configuration" % (self.domain_name))
        log.I("command [getRule]")
        out, err = self.pfw.sendCmd("getRule",self.domain_name,"Wrong_Config_Name")
        assert err == None, "ERROR : command [getRule] - Error when getting rule on domain %s from a non-existent configuration" % (self.domain_name)
        assert out != "Done", "ERROR : command [getRule] - Error not detected while getting rule on domain %s from a non-existent configuration" % (self.domain_name)
        log.I("error correctly detected when getting a rule from a non-existent configuration")
        log.I("getting a rule from a non-existent domain")
        log.I("command [getRule]")
        out, err = self.pfw.sendCmd("getRule","Wrong_Domain_Name",self.conf_2)
        assert err == None, "ERROR : command [getRule] - Error when getting rule from a non-existent domain"
        assert out != "Done", "ERROR : command [getRule] - Error not detected while getting rule from a non-existent domain"
        log.I("error correctly detected when getting rule from a non-existent domain")
        log.I("getting a rule with wrong parameters order")
        log.I("command [getRule]")
        out, err = self.pfw.sendCmd("getRule",self.conf_1,self.domain_name)
        assert err == None, "ERROR : command [getRule] - Error when getting a rule with incorrect paramaters order"
        assert out != "Done", "ERROR : command [getRule] - Error not detected when getting a rule with incorrect paramaters order"
        log.I("error correctly detected when getting a rule with incorrect paramaters order on domain %s and configuration %s" % (self.domain_name,self.conf_1))

        # New domain deletion
        log.I("Domain %s deletion" % (self.domain_name))
        log.I("command [deleteDomain]")
        out, err = self.pfw.sendCmd("deleteDomain",self.domain_name, "")
        assert out == "Done", out
        assert err == None, "ERROR : command [deleteDomain] - Error while delting domain %s" % (self.domain_name)
        log.I("command [deleteDomain] correctly executed")
        log.I("Domain %s deleted" % (self.domain_name))

    def test_Nominal_Case(self):
        """
        Testing nominal case
        --------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - setting rules for configurations
                - getting rules from configurations
                - Clear created rules
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [getRule] function
                - [setRule] function
                - [clearRule] function
                - [createDomain] function
                - [createConfiguration] function
                - [deleteConfiguration] function
                - [deleteDomain] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - all operations succeed
        """
        log.D(self.test_Nominal_Case.__doc__)
        # New domain creation for testing purpose
        log.I("New domain creation for testing purpose : %s" % (self.domain_name))
        log.I("command [createDomain]")
        out, err = self.pfw.sendCmd("createDomain",self.domain_name, "")
        assert out == "Done", out
        assert err == None, "ERROR : command [createDomain] - Error while creating domain %s" % (self.domain_name)
        log.I("command [createDomain] correctly executed")
        log.I("Domain %s created" % (self.domain_name))

        # New configurations creation for testing purpose
        log.I("New configuration %s creation for domain %s for testing purpose" % (self.conf_1,self.domain_name))
        log.I("command [createConfiguration]")
        out, err = self.pfw.sendCmd("createConfiguration",self.domain_name,self.conf_1)
        assert out == "Done", out
        assert err == None, "ERROR : command [createConfiguration] - Error while creating configuration %s" % (self.conf_1)
        log.I("command [createConfiguration] correctly executed")
        log.I("Configuration %s created for domain %s" % (self.conf_1,self.domain_name))
        log.I("New configuration %s creation for domain %s for testing purpose" % (self.conf_2,self.domain_name))
        log.I("command [createConfiguration]")
        out, err = self.pfw.sendCmd("createConfiguration",self.domain_name,self.conf_2)
        assert out == "Done", out
        assert err == None, "ERROR : command [createConfiguration] - Error while creating configuration %s" % (self.conf_2)
        log.I("command [createConfiguration] correctly executed")
        log.I("Configuration %s created for domain %s" % (self.conf_2,self.domain_name))

        # Applying rules to configurations
        log.I("Applying rules to configurations %s and %s from domain %s" % (self.conf_1,self.conf_2,self.domain_name))
        log.I("command [setRule]")
        out, err = self.pfw.sendCmd("setRule",self.domain_name,self.conf_1,self.rule_1)
        assert err == None, "ERROR : command [setRule] - Error while setting rule for configurations %s" % (self.conf_1)
        assert out == "Done", "FAIL : command [setRule] - Error while setting rule for configuration %s" % (self.conf_1)
        log.I("command [setRule] correctly executed")
        log.I("rule correctly created for configuration %s" % (self.conf_1))
        out, err = self.pfw.sendCmd("setRule",self.domain_name,self.conf_2,self.rule_2)
        assert err == None, "ERROR : command [setRule] - Error while setting rule for configurations %s" % (self.conf_2)
        assert out == "Done", "FAIL : command [setRule] - Error while setting rule for configuration %s" % (self.conf_2)
        log.I("command [setRule] correctly executed")
        log.I("rule correctly created for configuration %s" % (self.conf_2))

        # Checking rules recovered
        log.I("Recovering rules for configurations %s and %s from domain %s" % (self.conf_1,self.conf_2,self.domain_name))
        log.I("command [getRule]")
        out, err = self.pfw.sendCmd("getRule",self.domain_name,self.conf_1)
        assert err == None, "ERROR : command [getRule] - Error while setting rule to configurations %s" % (self.conf_1)
        assert out == str(self.rule_1), "FAIL : command [getRule] - Error while recovering rule from configuration %s, incorrect value" % (self.conf_1)
        log.I("command [getRule] correctly executed")
        log.I("rule correctly recovered from configuration %s" % (self.conf_1))
        out, err = self.pfw.sendCmd("getRule",self.domain_name,self.conf_2)
        assert err == None, "ERROR : command [getRule] - Error while setting rule to configurations %s" % (self.conf_2)
        assert out == str(self.rule_2), "FAIL : command [getRule] - Error while recovering rule from configuration %s, incorrect value" % (self.conf_2)
        log.I("command [getRule] correctly executed")
        log.I("rule correctly recovered from configuration %s" % (self.conf_2))

        # Clearing rules
        log.I("Clear rules for configurations %s and %s from domain %s" % (self.conf_1,self.conf_2,self.domain_name))
        log.I("command [clearRule]")
        out, err = self.pfw.sendCmd("clearRule",self.domain_name,self.conf_1)
        assert err == None, "ERROR : command [clearRule] - Error on clearRule for configuration %s" % (self.conf_1)
        assert out == "Done", "FAIL : command [clearRule] - Error on clearRule for configuration %s" % (self.conf_1)
        out, err = self.pfw.sendCmd("getRule",self.domain_name,self.conf_1)
        assert out == "<none>", "ERROR : command [clearRule] - ClearRule not working for configuration %s" % (self.conf_1)
        out, err = self.pfw.sendCmd("clearRule",self.domain_name,self.conf_2)
        assert err == None, "ERROR : command [clearRule] - Error on clearRule for configuration %s" % (self.conf_2)
        assert out == "Done", "FAIL : command [clearRule] - Error on clearRule for configuration %s" % (self.conf_2)
        out, err = self.pfw.sendCmd("getRule",self.domain_name,self.conf_2)
        assert out == "<none>", "ERROR : command [clearRule] - ClearRule not working for configuration %s" % (self.conf_2)
        log.I("command [clearRule] correctly executed")
        log.I("ClearRule effective for configurations %s and %s" % (self.conf_1,self.conf_2))

        # New domain deletion
        log.I("Domain %s deletion" % (self.domain_name))
        log.I("command [deleteDomain]")
        out, err = self.pfw.sendCmd("deleteDomain",self.domain_name, "")
        assert out == "Done", out
        assert err == None, "ERROR : command [deleteDomain] - Error while delting domain %s" % (self.domain_name)
        log.I("command [deleteDomain] correctly executed")
        log.I("Domain %s deleted" % (self.domain_name))
