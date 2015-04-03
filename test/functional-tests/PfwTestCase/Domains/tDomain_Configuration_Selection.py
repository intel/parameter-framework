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
Effect of criteria changes on configuration testcases

List of tested functions :
--------------------------
    - [applyConfigurations] function
    - [setCriterionState] function

Test cases :
------------
    - test_Combinatorial_Criteria
"""
import os
from Util.PfwUnitTestLib import PfwTestCase
from Util import ACTLogging
log=ACTLogging.Logger()

class TestCases(PfwTestCase):

    def setUp(self):
        self.pfw.sendCmd("setTuningMode", "on")
        self.reference_xml = "$PFW_TEST_TOOLS/xml/XML_Test/Reference_Criteria.xml"
        self.temp_domain="f_Domains_Backup"
        self.temp_status="f_Config_Status"
        # Expected results are defined by Reference_Criteria.xml configuration settings
        self.expected_result = [["Conf_1_1", "<none>",   "Conf_3_0"] ,
                                ["Conf_1_1", "Conf_2_1", "Conf_3_1"] ,
                                ["Conf_1_1", "Conf_2_1", "Conf_3_0"] ,
                                ["Conf_1_1", "Conf_2_0", "Conf_3_0"] ,
                                ["Conf_1_0", "Conf_2_0", "Conf_3_0"] ,
                                ["Conf_1_1", "Conf_2_0", "Conf_3_0"] ,
                                ["Conf_1_1", "Conf_2_0", "Conf_3_1"]]
        self.criterion_setup = [["0x2", "1"] ,
                                ["0x2", "0"] ,
                                ["0x1", "0"] ,
                                ["0x1", "1"] ,
                                ["0x3", "4"] ,
                                ["0x0", "1"]]
        # names used in this test refer to names used in Reference_Criteria.xml
        self.new_domain_name = "Domain"
        self.crit_change_iteration = 6

    def tearDown(self):
        self.pfw.sendCmd("setTuningMode", "off")

    def test_Combinatorial_Criteria(self):
        """
        Testing combinatorial criteria
        ------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                Checking that PFW behaviour is in line with expectations when setting criteria states.
                    - Test of all combinatorial of criteria and possible errors
                        - nominal case in configuration selection
                        - conflict in configuration selection
                        - no configuration selected
                        - error in criterion setting
                    - test of compound rules : All / Any
                    - test of matches cases  : Is / IsNot / Include / Exclude
                    - test of criteria types : Inclusive / Exclusive
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [applyConfigurations] function
                - [setCriterionState] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - Configurations setting conform to expected behavior
        """
        log.D(self.test_Combinatorial_Criteria.__doc__)

        # Import a reference XML file
        log.I("Import Domains with settings from %s"%(self.reference_xml))
        out, err = self.pfw.sendCmd("importDomainsWithSettingsXML",self.reference_xml, "")
        assert err == None, log.E("Command [importDomainsWithSettingsXML %s] : %s"%(self.reference_xml,err))
        assert out == "Done", log.F("When using function importDomainsWithSettingsXML %s]"%(self.reference_xml))

        # Check number of domain
        log.I("Current domains listing")
        log.I("Command [listDomains]")
        out, err = self.pfw.sendCmd("listDomains","","")
        assert err == None, log.E("Command [listDomains] : %s"%(err))
        log.I("Command [listDomains] : correctly executed")

        # Domains listing backup
        f_Domains_Backup = open(self.temp_domain, "w")
        f_Domains_Backup.write(out)
        f_Domains_Backup.close()
        f_Domains_Backup = open(self.temp_domain, "r")
        domains_nbr = 0
        line=f_Domains_Backup.readline()
        while line!="":
            line=f_Domains_Backup.readline()
            domains_nbr+=1
        f_Domains_Backup.close()

        # Applying default configurations
        out, err = self.pfw.sendCmd("setTuningMode", "off")
        assert err == None, log.E("Command [setTuningMode]")
        out, err = self.hal.sendCmd("applyConfigurations")
        assert err == None, log.E("Command HAL [applyConfigurations]")

        # Saving default status
        out, err = self.pfw.sendCmd("status")
        f_Config_Status = open(self.temp_status, "w")
        f_Config_Status.write(out)
        f_Config_Status.close()

        # Test cases iterations
        for iteration in range (self.crit_change_iteration+1):

            # Criteria settings
            # No criteria are set at the first iteration for testing default configurations
            if iteration != 0:
                log.I("Setting criterion %s to %s" % ("Crit_0", str(self.criterion_setup[iteration-1][0])))
                state = str(self.criterion_setup[iteration-1][0])
                out, err = self.hal.sendCmd("setCriterionState", "Crit_0", state)
                assert err == None, log.E("Command HAL [setCriterionState]")
                log.I("Setting criterion %s to %s" % ("Crit_1", str(self.criterion_setup[iteration-1][1])))
                state = str(self.criterion_setup[iteration-1][1])
                out, err = self.hal.sendCmd("setCriterionState", "Crit_1", state)
                assert err == None, log.E("Command HAL [setCriterionState]")
                log.I("Applaying new configurations")
                out, err = self.hal.sendCmd("applyConfigurations")
                assert err == None, log.E("Command HAL [applyConfigurations]")
                out, err = self.pfw.sendCmd("status")
                assert err == None, log.E("Command [status]")
                os.remove(self.temp_status)
                f_Config_Status = open(self.temp_status, "w")
                f_Config_Status.write(out)
                f_Config_Status.close()
            else :
                log.I("Default Configurations - no criteria are set :")
                out, err = self.pfw.sendCmd("status")
                os.remove(self.temp_status)
                f_Config_Status = open(self.temp_status, "w")
                f_Config_Status.write(out)
                f_Config_Status.close()

            # Configurations checking
            for domain in range (domains_nbr):
                domain_name = "".join([self.new_domain_name, "_", str(domain+1), "[<none>]"])
                config = str(self.expected_result[iteration][domain])
                log.I("Checking that domain %s is set to configuration : %s" % (domain_name,config))
                for line in open(self.temp_status, "r"):
                    if domain_name in line:
                        line = line.replace(domain_name,'')
                        line = line.replace(":","")
                        line = line.replace(' ','')
                        line = line.replace("\n","")
                        assert line == config, log.F("Domain %s - Expected configuration : %s, found : %s" % (domain_name,config,line))
                        log.I("Domain %s - configuration correctly set to %s" % (domain_name,line))

        # Temporary files deletion
        os.remove(self.temp_domain)
        os.remove(self.temp_status)
