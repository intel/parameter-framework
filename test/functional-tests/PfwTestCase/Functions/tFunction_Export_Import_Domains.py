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
Export and import settings and domains from file testcases

List of tested functions :
--------------------------
    - [exportDomainsWithSettingsXML]  function
    - [importDomainsWithSettingsXML] function
    - [exportDomainsXML] function
    - [importDomainsXML] function
    - [importSettings] function
    - [exportSettings] function

Test cases :
------------
    - Testing importDomainsWithSettingsXML nominal case
    - Testing exportDomainsWithSettingsXML nominal case
    - Testing exportDomainsXML/importDomainsXML nominal case
    - Testing importSettings/exportSettings nominal case
    - Testing import errors
    - Testing export errors
"""
import os, commands
import unittest
from Util.PfwUnitTestLib import PfwTestCase
from Util import ACTLogging
log=ACTLogging.Logger()

class TestCases(PfwTestCase):

    def setUp(self):

        self.pfw.sendCmd("setTuningMode", "on")
        self.param_name_01 = "/Test/Test/TEST_DIR/UINT16"
        self.filesystem_01 = "$PFW_RESULT/UINT16"
        self.param_name_02 = "/Test/Test/TEST_DOMAIN_0/Param_00"
        self.filesystem_02 = "$PFW_RESULT/Param_00"
        self.param_name_03 = "/Test/Test/TEST_DOMAIN_1/Param_12"
        self.filesystem_03 = "$PFW_RESULT/Param_12"

        pfw_test_tools=os.getenv("PFW_TEST_TOOLS")
        self.reference_xml = pfw_test_tools+"/xml/XML_Test/Reference_Compliant.xml"
        self.initial_xml = pfw_test_tools+"/xml/TestConfigurableDomains.xml"
        self.temp_config="f_Config_Backup"
        self.temp_domain="f_Domains_Backup"
        self.temp_xml=pfw_test_tools+"/f_temp.xml"
        self.temp_binary=pfw_test_tools+"/f_temp.binary"

        self.nb_domains_in_reference_xml=3
        self.nb_conf_per_domains_in_reference_xml=[2,2,2]

    def tearDown(self):
        self.pfw.sendCmd("setTuningMode", "off")
        if os.path.exists(self.temp_domain):
            os.remove(self.temp_domain)
        if os.path.exists(self.temp_config):
            os.remove(self.temp_config)
        if os.path.exists(self.temp_xml):
            os.remove(self.temp_xml)
        if os.path.exists(self.temp_binary):
            os.remove(self.temp_binary)

    def test_01_importDomainsWithSettingsXML_Nominal_Case(self):
        """
        Testing importDomainsWithSettingsXML nominal case
        -------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - import a reference XML
                - check Domains
                - check Configuration
                - restore Configuration
                - check Parameters
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [createConfiguration] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [restoreConfiguration] function
                - [listDomains] function
                - [listConfiguration] function
                - [getRules] function
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - all operations succeed
        """
        log.D(self.test_01_importDomainsWithSettingsXML_Nominal_Case.__doc__)

        #Import a reference XML file
        log.I("Import Domains with settings from %s"%(self.reference_xml))
        out, err = self.pfw.sendCmd("importDomainsWithSettingsXML",self.reference_xml, "")
        assert err == None, log.E("Command [importDomainsWithSettingsXML %s] : %s"%(self.reference_xml,err))
        assert out == "Done", log.F("When using function importDomainsWithSettingsXML %s]"%(self.reference_xml))

        #Check number of domain(3 domains are setup in the reference XML, initially only one domains is declared)
        # Domains listing using "listDomains" command
        log.I("Current domains listing")
        log.I("Command [listDomains]")
        out, err = self.pfw.sendCmd("listDomains","","")
        assert err == None, log.E("Command [listDomains] : %s"%(err))
        log.I("Command [listDomains] - correctly executed")
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
        log.I("Actual domains number : %s" % domains_nbr)
        assert domains_nbr==self.nb_domains_in_reference_xml, log.F("Number of listed domains is not compliant with the file %s - expected : %s - found : %s"%(self.reference_xml,self.nb_domains_in_reference_xml, domains_nbr))

        #Check number of config per domain(2 config per domains are setup in the reference XML)
        # Config listing
        domain_basename="Domain_"
        for index in range(self.nb_domains_in_reference_xml):
            domain_name=domain_basename+str(index+1)
            log.I("Listing config for domain %s"%(domain_name))
            out, err = self.pfw.sendCmd("listConfigurations",domain_name,"")
            assert err == None, log.E("Command [listConfigurations %s] : %s"%(domain_name,err))
            log.I("Command [listConfigurations %s] - correctly executed"%(domain_name))
            f_Config_Backup = open(self.temp_config, "w")
            f_Config_Backup.write(out)
            f_Config_Backup.close()
            f_Config_Backup = open(self.temp_config, "r")
            config_nbr = 0
            line=f_Config_Backup.readline()
            while line!="":
                line=f_Config_Backup.readline()
                config_nbr+=1
            f_Config_Backup.close()
            assert config_nbr==self.nb_conf_per_domains_in_reference_xml[index], log.F("Number of listed config for %s is not compliant with the file %s - expected : %s - found : %s"%(domain_name, self.reference_xml,self.nb_conf_per_domains_in_reference_xml[index], domains_nbr))
        log.I("Config checking : OK")

        #Check number of config per domain(2 config per domains are setup in the reference XML)
        # Config listing
        conf_basename="Conf_"
        for index_domain in range(3):
            for index_conf in range(2):
                domain_name=domain_basename+str(index_domain+1)
                conf_name=conf_basename+str(index_domain+1)+"_"+str(index_conf)
                log.I("Get rule for domain %s - conf %s"%(domain_name,conf_name))
                out, err = self.pfw.sendCmd("getRule",domain_name,conf_name)
                assert err == None, log.E("Command [getRules %s %s] : %s"%(domain_name,conf_name,err))
                assert out !="", log.F("No rules loaded for domain %s conf %s"%(domain_name,conf_name))
        log.I("Rules checking : OK")

        #Restore config
        conf_basename="Conf_"
        for index_domain in range(3):
            for index_conf in range(2):
                domain_name=domain_basename+str(index_domain+1)
                conf_name=conf_basename+str(index_domain+1)+"_"+str(index_conf)
                log.I("Restore config %s for domain %s"%(conf_name,domain_name))
                out, err = self.pfw.sendCmd("restoreConfiguration",domain_name,conf_name)
                assert err == None, log.E("Command [restoreConfiguration %s %s] : %s"%(domain_name,conf_name,err))
                assert out =="Done", log.F("When restoring configuration %s for domain %s"%(conf_name,domain_name))
        log.I("Restore configurations: OK")

        #set Tuning Mode off to check parameter value
        self.pfw.sendCmd("setTuningMode", "off")

        #Check parameter values
        #UINT16
        expected_value="0"
        hex_value="0x0"
        log.I("UINT16 parameter in the conf Conf_1_1= %s"%(expected_value))
        out, err = self.pfw.sendCmd("getParameter", self.param_name_01, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name_01, err))
        assert out == expected_value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name_01, expected_value, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_01)) == hex_value, log.F("FILESYSTEM : parameter %s update error"%self.param_name_01)
        #Param_00
        expected_value="4"
        hex_value="0x4"
        log.I("Param_00 parameter in the conf Conf_2_1= %s"%(expected_value))
        out, err = self.pfw.sendCmd("getParameter", self.param_name_02, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name_02, err))
        assert out == expected_value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name_02, expected_value, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_02)) == hex_value, log.F("FILESYSTEM : parameter %s update error"%self.param_name_02)
        #Param_12
        expected_value="4"
        hex_value="0x4"
        log.I("Param_12 parameter in the conf Conf_3_1= %s"%(expected_value))
        out, err = self.pfw.sendCmd("getParameter", self.param_name_03, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name_03, err))
        assert out == expected_value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name_03, expected_value, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_03)) == hex_value, log.F("FILESYSTEM : parameter %s update error"%self.param_name_03)
        log.I("Parameters checking : OK")

    def test_02_exportDomainsWithSettingsXML_Nominal_Case(self):
        """
        Testing exportDomainsWithSettingsXML nominal case
        -------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - export domains with settings in temp XML
                - import a reference XML
                - restore Configuration
                - import the temp XML
                - restore Configuration
                - check Domains
                - check Configuration
                - check Parameters
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [exportDomainsWithSettingsXML] function
                - [importDomainsWithSettingsXML] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [restoreConfiguration] function
                - [listDomains] function
                - [listConfiguration] function
                - [getRules] function
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - all operations succeed
        """
        log.D(self.test_02_exportDomainsWithSettingsXML_Nominal_Case.__doc__)

        ### INIT Domains Settings ####

        #Import a reference XML file

        log.I("Import Domains with initial settings from %s"%(self.reference_xml))
        out, err = self.pfw.sendCmd("importDomainsWithSettingsXML",self.reference_xml, "")
        assert err == None, log.E("Command [importDomainsWithSettingsXML %s] : %s"%(self.reference_xml,err))
        assert out == "Done", log.F("When using function importDomainsWithSettingsXML %s]"%(self.reference_xml))
        self.pfw.sendCmd("setTuningMode", "off","")
        init_value_01, err = self.pfw.sendCmd("getParameter", self.param_name_01, "")
        init_value_02, err = self.pfw.sendCmd("getParameter", self.param_name_02, "")
        init_value_03, err = self.pfw.sendCmd("getParameter", self.param_name_03, "")
        init_filesystem_01 = commands.getoutput("cat %s"%(self.filesystem_01))
        init_filesystem_02 = commands.getoutput("cat %s"%(self.filesystem_02))
        init_filesystem_03 = commands.getoutput("cat %s"%(self.filesystem_03))

        ### END OF INIT ###

        #Export in a temp XML file
        log.I("Export Domains with initial settings in %s"%(self.temp_xml))
        out, err = self.pfw.sendCmd("exportDomainsWithSettingsXML",self.temp_xml, "")
        assert err == None, log.E("Command [exportDomainsWithSettingsXML %s] : %s"%(self.temp_xml,err))
        assert out == "Done", log.F("When using function exportDomainsWithSettingsXML %s]"%(self.temp_xml))

        #Change the value of checked parameters
        self.pfw.sendCmd("setTuningMode", "on","")
        out, err = self.pfw.sendCmd("setParameter", self.param_name_01, str(int(init_value_01)+1))
        out, err = self.pfw.sendCmd("setParameter", self.param_name_02, str(int(init_value_02)+1))
        out, err = self.pfw.sendCmd("setParameter", self.param_name_03, str(int(init_value_03)+1))
        #save config
        domain_basename="Domain_"
        conf_basename="Conf_"
        for index_domain in range(3):
            for index_conf in range(2):
                domain_name=domain_basename+str(index_domain+1)
                conf_name=conf_basename+str(index_domain+1)+"_"+str(index_conf)
                log.I("Save config %s for domain %s"%(conf_name,domain_name))
                out, err = self.pfw.sendCmd("saveConfiguration",domain_name,conf_name)
                assert err == None, log.E("Command [saveConfiguration %s %s] : %s"%(domain_name,conf_name,err))
                assert out =="Done", log.F("When saving configuration %s for domain %s"%(conf_name,domain_name))
        log.I("Save configurations: OK")
        self.pfw.sendCmd("setTuningMode", "off","")

        #Check parameter values
        #UINT16
        expected_value=str(int(init_value_01)+1)
        log.I("UINT16 parameter = %s"%(expected_value))
        out, err = self.pfw.sendCmd("getParameter", self.param_name_01, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name_01, err))
        assert out == expected_value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name_01, expected_value, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_01)) != init_filesystem_01, log.F("FILESYSTEM : parameter %s update error"%self.param_name_01)
        #Param_00
        expected_value=str(int(init_value_02)+1)
        log.I("Param_00 parameter= %s"%(expected_value))
        out, err = self.pfw.sendCmd("getParameter", self.param_name_02, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name_02, err))
        assert out == expected_value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name_02, expected_value, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_02)) != init_filesystem_02, log.F("FILESYSTEM : parameter %s update error"%self.param_name_02)
        #Param_12
        expected_value=str(int(init_value_03)+1)
        log.I("Param_12 parameter= %s"%(expected_value))
        out, err = self.pfw.sendCmd("getParameter", self.param_name_03, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name_03, err))
        assert out == expected_value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name_03, expected_value, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_03)) != init_filesystem_03, log.F("FILESYSTEM : parameter %s update error"%self.param_name_03)

        #Import the temp XML file
        self.pfw.sendCmd("setTuningMode", "on","")
        log.I("Import Domains with settings from %s"%(self.temp_xml))
        out, err = self.pfw.sendCmd("importDomainsWithSettingsXML",self.temp_xml, "")
        assert err == None, log.E("Command [importDomainsWithSettingsXML %s] : %s"%(self.temp_xml,err))
        assert out == "Done", log.F("When using function importDomainsWithSettingsXML %s]"%(self.temp_xml))
        self.pfw.sendCmd("setTuningMode", "off","")

        #Check parameter values
        #UINT16
        expected_value=init_value_01
        log.I("UINT16 parameter = %s"%(expected_value))
        out, err = self.pfw.sendCmd("getParameter", self.param_name_01, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name_01, err))
        assert out == expected_value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name_01, expected_value, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_01)) == init_filesystem_01, log.F("FILESYSTEM : parameter %s update error"%self.param_name_01)
        #Param_00
        expected_value=init_value_02
        log.I("Param_00 parameter= %s"%(expected_value))
        out, err = self.pfw.sendCmd("getParameter", self.param_name_02, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name_02, err))
        assert out == expected_value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name_02, expected_value, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_02)) == init_filesystem_02, log.F("FILESYSTEM : parameter %s update error"%self.param_name_02)
        #Param_12
        expected_value=init_value_03
        log.I("Param_12 parameter= %s"%(expected_value))
        out, err = self.pfw.sendCmd("getParameter", self.param_name_03, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name_03, err))
        assert out == expected_value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name_03, expected_value, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_03)) == init_filesystem_03, log.F("FILESYSTEM : parameter %s update error"%self.param_name_03)

    def test_03_exportImportXML_withoutSettings_Nominal_Case(self):
        """
        Testing exportDomainsXML/importDomainsXML nominal case
        ------------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - export domains in temp XML
                - import a reference XML
                - restore Configuration
                - import the temp XML
                - restore Configuration
                - check Domains
                - check Configuration
                - check Parameters
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [exportDomainsXML] function
                - [importDomainsWithSettingsXML] function
                - [importDomainsXML] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [restoreConfiguration] function
                - [listDomains] function
                - [listConfiguration] function
                - [getRules] function
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - all operations succeed
                - parameter must not change
        """
        log.D(self.test_03_exportImportXML_withoutSettings_Nominal_Case.__doc__)

        ### INIT Domains Settings ####

        #Import a reference XML file

        log.I("Import Domains with initial settings from %s"%(self.reference_xml))
        out, err = self.pfw.sendCmd("importDomainsWithSettingsXML",self.reference_xml, "")
        assert err == None, log.E("Command [importDomainsWithSettingsXML %s] : %s"%(self.reference_xml,err))
        assert out == "Done", log.F("When using function importDomainsWithSettingsXML %s]"%(self.reference_xml))
        self.pfw.sendCmd("setTuningMode", "off","")
        init_value_01, err = self.pfw.sendCmd("getParameter", self.param_name_01, "")
        init_value_02, err = self.pfw.sendCmd("getParameter", self.param_name_02, "")
        init_value_03, err = self.pfw.sendCmd("getParameter", self.param_name_03, "")
        init_filesystem_01 = commands.getoutput("cat %s"%(self.filesystem_01))
        init_filesystem_02 = commands.getoutput("cat %s"%(self.filesystem_02))
        init_filesystem_03 = commands.getoutput("cat %s"%(self.filesystem_03))

        ### END OF INIT ###

        #Export domains without settings in a temp XML file
        log.I("Export Domains without initial settings in %s"%(self.temp_xml))
        out, err = self.pfw.sendCmd("exportDomainsXML",self.temp_xml, "")
        assert err == None, log.E("Command [exportDomainsXML %s] : %s"%(self.temp_xml,err))
        assert out == "Done", log.F("When using function exportDomainsXML %s]"%(self.temp_xml))

        #Change the value of checked parameters
        self.pfw.sendCmd("setTuningMode", "on","")
        out, err = self.pfw.sendCmd("setParameter", self.param_name_01, str(int(init_value_01)+1))
        out, err = self.pfw.sendCmd("setParameter", self.param_name_02, str(int(init_value_02)+1))
        out, err = self.pfw.sendCmd("setParameter", self.param_name_03, str(int(init_value_03)+1))
        #save config
        domain_basename="Domain_"
        conf_basename="Conf_"
        for index_domain in range(3):
            for index_conf in range(2):
                domain_name=domain_basename+str(index_domain+1)
                conf_name=conf_basename+str(index_domain+1)+"_"+str(index_conf)
                log.I("Save config %s for domain %s"%(conf_name,domain_name))
                out, err = self.pfw.sendCmd("saveConfiguration",domain_name,conf_name)
                assert err == None, log.E("Command [saveConfiguration %s %s] : %s"%(domain_name,conf_name,err))
                assert out =="Done", log.F("When saving configuration %s for domain %s"%(conf_name,domain_name))
        log.I("Save configurations: OK")
        self.pfw.sendCmd("setTuningMode", "off","")

        #Check parameter values
        #UINT16
        expected_value=str(int(init_value_01)+1)
        log.I("UINT16 parameter = %s"%(expected_value))
        out, err = self.pfw.sendCmd("getParameter", self.param_name_01, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name_01, err))
        assert out == expected_value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name_01, expected_value, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_01)) != init_filesystem_01, log.F("FILESYSTEM : parameter %s update error"%self.param_name_01)
        #Param_00
        expected_value=str(int(init_value_02)+1)
        log.I("Param_00 parameter= %s"%(expected_value))
        out, err = self.pfw.sendCmd("getParameter", self.param_name_02, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name_02, err))
        assert out == expected_value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name_02, expected_value, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_02)) != init_filesystem_02, log.F("FILESYSTEM : parameter %s update error"%self.param_name_02)
        #Param_12
        expected_value=str(int(init_value_03)+1)
        log.I("Param_12 parameter= %s"%(expected_value))
        out, err = self.pfw.sendCmd("getParameter", self.param_name_03, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name_03, err))
        assert out == expected_value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name_03, expected_value, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_03)) != init_filesystem_03, log.F("FILESYSTEM : parameter %s update error"%self.param_name_03)

        #Import the temp XML file without
        self.pfw.sendCmd("setTuningMode", "on","")
        log.I("Import Domains without settings from %s"%(self.temp_xml))
        out, err = self.pfw.sendCmd("importDomainsXML",self.temp_xml, "")
        assert err == None, log.E("Command [importDomainsXML %s] : %s"%(self.temp_xml,err))
        assert out == "Done", log.F("When using function importDomainsXML %s]"%(self.temp_xml))
        self.pfw.sendCmd("setTuningMode", "off","")

        #Check parameter values
        #UINT16
        unexpected_value=init_value_01
        log.I("UINT16 parameter = %s"%(unexpected_value))
        out, err = self.pfw.sendCmd("getParameter", self.param_name_01, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name_01, err))
        assert out != unexpected_value, log.F("BLACKBOARD : Unexpected value found for %s: %s" % (self.param_name_01, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_01)) != init_filesystem_01, log.F("FILESYSTEM : parameter %s update error"%self.param_name_01)
        #Param_00
        unexpected_value=init_value_02
        log.I("Param_00 parameter= %s"%(unexpected_value))
        out, err = self.pfw.sendCmd("getParameter", self.param_name_02, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name_02, err))
        assert out != unexpected_value, log.F("BLACKBOARD : Unexpected value found for %s: %s" % (self.param_name_02, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_02)) != init_filesystem_02, log.F("FILESYSTEM : parameter %s update error"%self.param_name_02)
        #Param_12
        unexpected_value=init_value_03
        log.I("Param_12 parameter= %s"%(unexpected_value))
        out, err = self.pfw.sendCmd("getParameter", self.param_name_03, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name_03, err))
        assert out != unexpected_value, log.F("BLACKBOARD : Unexpected value found for %s: %s"% (self.param_name_03, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_03)) != init_filesystem_03, log.F("FILESYSTEM : parameter %s update error"%self.param_name_03)

        #Import the reference_XML file without settings
        self.pfw.sendCmd("setTuningMode", "on","")
        log.I("Import Domains without settings from %s"%(self.reference_xml))
        out, err = self.pfw.sendCmd("importDomainsXML",self.reference_xml, "")
        assert err == None, log.E("Command [importDomainsXML %s] : %s"%(self.reference_xml,err))
        assert out == "Done", log.F("When using function importDomainsXML %s]"%(self.reference_xml))
        self.pfw.sendCmd("setTuningMode", "off","")

        #Check parameter values
        #UINT16
        unexpected_value=init_value_01
        log.I("UINT16 parameter = %s"%(unexpected_value))
        out, err = self.pfw.sendCmd("getParameter", self.param_name_01, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name_01, err))
        assert out != unexpected_value, log.F("BLACKBOARD : Unexpected value found for %s: %s" % (self.param_name_01, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_01)) != init_filesystem_01, log.F("FILESYSTEM : parameter %s update error"%self.param_name_01)
        #Param_00
        unexpected_value=init_value_02
        log.I("Param_00 parameter= %s"%(unexpected_value))
        out, err = self.pfw.sendCmd("getParameter", self.param_name_02, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name_02, err))
        assert out != unexpected_value, log.F("BLACKBOARD : Unexpected value found for %s: %s" % (self.param_name_02, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_02)) != init_filesystem_02, log.F("FILESYSTEM : parameter %s update error"%self.param_name_02)
        #Param_12
        unexpected_value=init_value_03
        log.I("Param_12 parameter= %s"%(unexpected_value))
        out, err = self.pfw.sendCmd("getParameter", self.param_name_03, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name_03, err))
        assert out != unexpected_value, log.F("BLACKBOARD : Unexpected value found for %s: %s" % (self.param_name_03, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_03)) != init_filesystem_03, log.F("FILESYSTEM : parameter %s update error"%self.param_name_03)



    def test_04_exportImportSettings_Binary_Nominal_Case(self):
        """
        Testing exportSettings/importSettings nominal case
        --------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - export settings in temp binary files
                - import a reference XML
                - restore Configuration
                - import the temp binary files
                - restore Configuration
                - check Domains
                - check Configuration
                - check Parameters
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [exportSettings] function
                - [importDomainsWithSettingsXML] function
                - [importSettings] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [restoreConfiguration] function
                - [listDomains] function
                - [listConfiguration] function
                - [getRules] function
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - all operations succeed
        """
        log.D(self.test_04_exportImportSettings_Binary_Nominal_Case.__doc__)
        ### INIT Domains Settings ####

        #Import a reference XML file

        log.I("Import Domains with initial settings from %s"
              %(self.reference_xml))
        out, err = self.pfw.sendCmd("importDomainsWithSettingsXML",self.reference_xml, "")
        assert err == None, log.E("Command [importDomainsWithSettingsXML %s] : %s"%(self.reference_xml,err))
        assert out == "Done", log.F("When using function importDomainsWithSettingsXML %s]"%(self.reference_xml))
        self.pfw.sendCmd("setTuningMode", "off","")
        init_value_01, err = self.pfw.sendCmd("getParameter", self.param_name_01, "")
        init_value_02, err = self.pfw.sendCmd("getParameter", self.param_name_02, "")
        init_value_03, err = self.pfw.sendCmd("getParameter", self.param_name_03, "")
        init_filesystem_01 = commands.getoutput("cat %s"%(self.filesystem_01))
        init_filesystem_02 = commands.getoutput("cat %s"%(self.filesystem_02))
        init_filesystem_03 = commands.getoutput("cat %s"%(self.filesystem_03))

        ### END OF INIT ###

        #Export domains without settings in a temp XML file
        log.I("Export Domains without initial settings in %s"%(self.temp_xml))
        out, err = self.pfw.sendCmd("exportDomainsXML",self.temp_xml, "")
        assert err == None, log.E("Command [exportDomainsXML %s] : %s"%(self.temp_xml,err))
        assert out == "Done", log.F("When using function exportDomainsXML %s]"%(self.temp_xml))
        #Export settings in a binary temp file
        log.I("Export settings in the binary files %s"%(self.temp_binary))
        out, err = self.pfw.sendCmd("exportSettings",self.temp_binary, "")
        assert err == None, log.E("Command [exportSettings %s] : %s"%(self.temp_binary,err))
        assert out == "Done", log.F("When using function exportSettings %s]"%(self.temp_binary))

        #Change the value of checked parameters
        self.pfw.sendCmd("setTuningMode", "on","")
        out, err = self.pfw.sendCmd("setParameter", self.param_name_01, str(int(init_value_01)+1))
        out, err = self.pfw.sendCmd("setParameter", self.param_name_02, str(int(init_value_02)+1))
        out, err = self.pfw.sendCmd("setParameter", self.param_name_03, str(int(init_value_03)+1))
        #save config
        domain_basename="Domain_"
        conf_basename="Conf_"
        for index_domain in range(3):
            for index_conf in range(2):
                domain_name=domain_basename+str(index_domain+1)
                conf_name=conf_basename+str(index_domain+1)+"_"+str(index_conf)
                log.I("Save config %s for domain %s"%(conf_name,domain_name))
                out, err = self.pfw.sendCmd("saveConfiguration",domain_name,conf_name)
                assert err == None, log.E("Command [saveConfiguration %s %s] : %s"%(domain_name,conf_name,err))
                assert out =="Done", log.F("When saving configuration %s for domain %s"%(conf_name,domain_name))
        log.I("Save configurations: OK")
        self.pfw.sendCmd("setTuningMode", "off","")

        #Check parameter values
        #UINT16
        expected_value=str(int(init_value_01)+1)
        log.I("UINT16 parameter = %s"%(expected_value))
        out, err = self.pfw.sendCmd("getParameter", self.param_name_01, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name_01, err))
        assert out == expected_value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name_01, expected_value, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_01)) != init_filesystem_01, log.F("FILESYSTEM : parameter %s update error"%self.param_name_01)
        #Param_00
        expected_value=str(int(init_value_02)+1)
        log.I("Param_00 parameter= %s"%(expected_value))
        out, err = self.pfw.sendCmd("getParameter", self.param_name_02, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name_02, err))
        assert out == expected_value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name_02, expected_value, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_02)) != init_filesystem_02, log.F("FILESYSTEM : parameter %s update error"%self.param_name_02)
        #Param_12
        expected_value=str(int(init_value_03)+1)
        log.I("Param_12 parameter= %s"%(expected_value))
        out, err = self.pfw.sendCmd("getParameter", self.param_name_03, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name_03, err))
        assert out == expected_value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name_03, expected_value, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_03)) != init_filesystem_03, log.F("FILESYSTEM : parameter %s update error"%self.param_name_03)

        #Import the temp XML file without
        self.pfw.sendCmd("setTuningMode", "on","")
        log.I("Import Domains without settings from %s"%(self.temp_xml))
        out, err = self.pfw.sendCmd("importDomainsXML",self.temp_xml, "")
        assert err == None, log.E("Command [importDomainsXML %s] : %s"%(self.temp_xml,err))
        assert out == "Done", log.F("When using function importDomainsXML %s]"%(self.temp_xml))
        self.pfw.sendCmd("setTuningMode", "off","")

        #Check parameter values
        #UINT16
        unexpected_value=init_value_01
        log.I("UINT16 parameter = %s"%(unexpected_value))
        out, err = self.pfw.sendCmd("getParameter", self.param_name_01, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name_01, err))
        assert out != unexpected_value, log.F("BLACKBOARD : Unexpected value found for %s: %s" % (self.param_name_01, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_01)) != init_filesystem_01, log.F("FILESYSTEM : parameter %s update error"%self.param_name_01)
        #Param_00
        unexpected_value=init_value_02
        log.I("Param_00 parameter= %s"%(unexpected_value))
        out, err = self.pfw.sendCmd("getParameter", self.param_name_02, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name_02, err))
        assert out != unexpected_value, log.F("BLACKBOARD : Unexpected value found for %s: %s" % (self.param_name_02, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_02)) != init_filesystem_02, log.F("FILESYSTEM : parameter %s update error"%self.param_name_02)
        #Param_12
        unexpected_value=init_value_03
        log.I("Param_12 parameter= %s"%(unexpected_value))
        out, err = self.pfw.sendCmd("getParameter", self.param_name_03, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name_03, err))
        assert out != unexpected_value, log.F("BLACKBOARD : Unexpected value found for %s: %s"% (self.param_name_03, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_03)) != init_filesystem_03, log.F("FILESYSTEM : parameter %s update error"%self.param_name_03)

        #Import settings from the binary files
        self.pfw.sendCmd("setTuningMode", "on","")
        log.I("Import settings from %s"%(self.temp_binary))
        out, err = self.pfw.sendCmd("importSettings",self.temp_binary, "")
        assert err == None, log.E("Command [importSettings %s] : %s"%(self.temp_binary,err))
        assert out == "Done", log.F("When using function importSettings %s]"%(self.temp_binary))
        self.pfw.sendCmd("setTuningMode", "off","")

        #Check parameter values
        #UINT16
        expected_value=init_value_01
        log.I("UINT16 parameter = %s"%(expected_value))
        out, err = self.pfw.sendCmd("getParameter", self.param_name_01, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name_01, err))
        assert out == expected_value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name_01, expected_value, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_01)) == init_filesystem_01, log.F("FILESYSTEM : parameter %s update error"%self.param_name_01)
        #Param_00
        expected_value=init_value_02
        log.I("Param_00 parameter= %s"%(expected_value))
        out, err = self.pfw.sendCmd("getParameter", self.param_name_02, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name_02, err))
        assert out == expected_value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name_02, expected_value, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_02)) == init_filesystem_02, log.F("FILESYSTEM : parameter %s update error"%self.param_name_02)
        #Param_12
        expected_value=init_value_03
        log.I("Param_12 parameter= %s"%(expected_value))
        out, err = self.pfw.sendCmd("getParameter", self.param_name_03, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name_03, err))
        assert out == expected_value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name_03, expected_value, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_03)) == init_filesystem_03, log.F("FILESYSTEM : parameter %s update error"%self.param_name_03)



    @unittest.expectedFailure
    def test_05_Import_XML_With_Settings_Error_Case(self):
        """
        Testing importDomainsWithSettingsXML error case
        -----------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - import with settings non-compliant XML
                - import with settings a non-existing  XML file
                - check Domains
                - check Configuration
                - check Parameters
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [importDomainsWithSettingsXML] function
                - [importDomainsXML] function
                - [importSettings] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [restoreConfiguration] function
                - [listDomains] function
                - [listConfiguration] function
                - [getRules] function
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - all errors are detected, initial domains keep settings
        """
        log.D(self.test_05_Import_XML_With_Settings_Error_Case.__doc__)

        ### INIT Domains Settings ####
        #Import a reference XML file

        log.I("Import Domains with initial settings from %s"%(self.reference_xml))
        out, err = self.pfw.sendCmd("importDomainsWithSettingsXML",self.reference_xml, "")
        assert err == None, log.E("Command [importDomainsWithSettingsXML %s] : %s"%(self.reference_xml,err))
        assert out == "Done", log.F("When using function importDomainsWithSettingsXML %s]"%(self.reference_xml))

        self.pfw.sendCmd("setTuningMode", "off","")
        init_value_01, err = self.pfw.sendCmd("getParameter", self.param_name_01, "")
        init_value_02, err = self.pfw.sendCmd("getParameter", self.param_name_02, "")
        init_value_03, err = self.pfw.sendCmd("getParameter", self.param_name_03, "")
        init_filesystem_01 = commands.getoutput("cat %s"%(self.filesystem_01))
        init_filesystem_02 = commands.getoutput("cat %s"%(self.filesystem_02))
        init_filesystem_03 = commands.getoutput("cat %s"%(self.filesystem_03))
        xml_path="$PFW_TEST_TOOLS/xml/XML_Test/"
        ### END OF INIT ###

        self.pfw.sendCmd("setTuningMode", "on","")
        #Import domains and settings from xml with outbound parameter value
        xml_name="Uncompliant_OutboundParameter.xml"
        log.I("Import %s with initial settings"%(xml_name))
        out, err = self.pfw.sendCmd("importDomainsWithSettingsXML",xml_path+xml_name, "")
        assert err == None, log.E("Command [importDomainsWithSettingsXML %s] : %s"%(xml_path+xml_name,err))
        assert out != "Done", log.F("Error not detected when imported %s]"%(xml_path+xml_name))
        log.I("Test OK : %s is not imported"%(xml_name))
        #Import domains and settings from xml using undeclared configurable element
        xml_name="Uncompliant_UndeclaredConfigurableElement.xml"
        log.I("Import %s with initial settings"%(xml_name))
        out, err = self.pfw.sendCmd("importDomainsWithSettingsXML",xml_path+xml_name, "")
        assert err == None, log.E("Command [importDomainsWithSettingsXML %s] : %s"%(xml_path+xml_name,err))
        assert out != "Done", log.F("Error not detected when imported %s]"%(xml_path+xml_name))
        log.I("Test OK : %s is not imported"%(xml_name))
        #Import domains and settings from xml using undeclared parameter
        xml_name="Uncompliant_UndeclaredParameter.xml"
        log.I("Import %s with initial settings"%(xml_name))
        out, err = self.pfw.sendCmd("importDomainsWithSettingsXML",xml_path+xml_name, "")
        assert err == None, log.E("Command [importDomainsWithSettingsXML %s] : %s"%(xml_path+xml_name,err))
        assert out != "Done", log.F("Error not detected when imported %s]"%(xml_path+xml_name))
        log.I("Test OK : %s is not imported"%(xml_name))
        #Import domains and settings from xml using wrong order of configurable element
        xml_name="Uncompliant_UnorderConfigurableElement.xml"
        log.I("Import %s with initial settings"%(xml_name))
        out, err = self.pfw.sendCmd("importDomainsWithSettingsXML",xml_path+xml_name, "")
        assert err == None, log.E("Command [importDomainsWithSettingsXML %s] : %s"%(xml_path+xml_name,err))
        assert out != "Done", log.F("Error not detected when imported %s]"%(xml_path+xml_name))
        log.I("Test OK : %s is not imported"%(xml_name))
        #Import domains and settings from unexistent xml
        xml_name="Unexistent.xml"
        log.I("Import %s with initial settings"%(xml_name))
        out, err = self.pfw.sendCmd("importDomainsWithSettingsXML",xml_path+xml_name, "")
        assert err == None, log.E("Command [importDomainsWithSettingsXML %s] : %s"%(xml_path+xml_name,err))
        assert out != "Done", log.F("Error not detected when imported %s]"%(xml_path+xml_name))
        log.I("Test OK : %s is not imported"%(xml_name))
        self.pfw.sendCmd("setTuningMode", "off","")

        #### check domains and settings ####

        #Check number of domain(3 domains are setup in the reference XML, initially only one domains is declared)
        # Domains listing using "listDomains" command
        log.I("Current domains listing")
        log.I("Command [listDomains]")
        out, err = self.pfw.sendCmd("listDomains","","")
        assert err == None, log.E("Command [listDomains] : %s"%(err))
        log.I("Command [listDomains] - correctly executed")
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
        log.I("Actual domains number : %s" % domains_nbr)
        assert domains_nbr==self.nb_domains_in_reference_xml, log.F("Number of listed domains is not compliant with the file %s - expected : %s - found : %s"%(self.reference_xml,self.nb_domains_in_reference_xml, domains_nbr))
        #Check number of config per domain(2 config per domains are setup in the reference XML)
        # Config listing
        domain_basename="Domain_"
        for index in range(self.nb_domains_in_reference_xml):
            domain_name=domain_basename+str(index+1)
            log.I("Listing config for domain %s"%(domain_name))
            out, err = self.pfw.sendCmd("listConfigurations",domain_name,"")
            assert err == None, log.E("Command [listConfigurations %s] : %s"%(domain_name,err))
            log.I("Command [listConfigurations %s] - correctly executed"%(domain_name))
            f_Config_Backup = open(self.temp_config, "w")
            f_Config_Backup.write(out)
            f_Config_Backup.close()
            f_Config_Backup = open(self.temp_config, "r")
            config_nbr = 0
            line=f_Config_Backup.readline()
            while line!="":
                line=f_Config_Backup.readline()
                config_nbr+=1
            f_Config_Backup.close()
            assert config_nbr==self.nb_conf_per_domains_in_reference_xml[index], log.F("Number of listed config for %s is not compliant with the file %s - expected : %s - found : %s"%(domain_name, self.reference_xml,self.nb_conf_per_domains_in_reference_xml[index], domains_nbr))
        log.I("Config checking : OK")
        #Check parameter values
        #UINT16
        expected_value=init_value_01
        hex_value=init_filesystem_01
        log.I("UINT16 parameter in the conf Conf_1_1= %s"%(expected_value))
        out, err = self.pfw.sendCmd("getParameter", self.param_name_01, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name_01, err))
        assert out == expected_value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name_01, expected_value, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_01)) == hex_value, log.F("FILESYSTEM : parameter %s update error"%self.param_name_01)
        #Param_00
        expected_value=init_value_02
        hex_value=init_filesystem_02
        log.I("Param_00 parameter in the conf Conf_2_1= %s"%(expected_value))
        out, err = self.pfw.sendCmd("getParameter", self.param_name_02, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name_02, err))
        assert out == expected_value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name_02, expected_value, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_02)) == hex_value, log.F("FILESYSTEM : parameter %s update error"%self.param_name_02)
        #Param_12
        expected_value=init_value_03
        hex_value=init_filesystem_03
        log.I("Param_12 parameter in the conf Conf_3_1= %s"%(expected_value))
        out, err = self.pfw.sendCmd("getParameter", self.param_name_03, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name_03, err))
        assert out == expected_value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name_03, expected_value, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_03)) == hex_value, log.F("FILESYSTEM : parameter %s update error"%self.param_name_03)
        log.I("Parameters checking : OK")

        #### END check domains and settings ####


    @unittest.expectedFailure
    def test_06_Import_XML_Without_Settings_Error_Case(self):
        """
        Testing import XML without settings error case
        ----------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - import non-compliant XML
                - import a non-existing XML files
                - import a reference XML
                - check Domains
                - check Configuration
                - check Parameters
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [importDomainsWithSettingsXML] function
                - [importDomainsXML] function
                - [importSettings] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [restoreConfiguration] function
                - [listDomains] function
                - [listConfiguration] function
                - [getRules] function
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - all errors are detected, initial domains keep settings
        """
        log.D(self.test_06_Import_XML_Without_Settings_Error_Case.__doc__)

        ### INIT Domains Settings ####
        #Import a reference XML file

        log.I("Import Domains with initial settings from %s"%(self.reference_xml))
        out, err = self.pfw.sendCmd("importDomainsWithSettingsXML",self.reference_xml, "")
        assert err == None, log.E("Command [importDomainsWithSettingsXML %s] : %s"%(self.reference_xml,err))
        assert out == "Done", log.F("When using function importDomainsWithSettingsXML %s]"%(self.reference_xml))

        self.pfw.sendCmd("setTuningMode", "off","")
        xml_path="$PFW_TEST_TOOLS/xml/XML_Test/"
        ### END OF INIT ###

        self.pfw.sendCmd("setTuningMode", "on","")
        #Import domains from xml with outbound parameter value
        xml_name="Uncompliant_OutboundParameter.xml"
        log.I("Import %s without settings"%(xml_name))
        out, err = self.pfw.sendCmd("importDomainsXML",xml_path+xml_name, "")
        assert err == None, log.E("Command [importDomainsXML %s] : %s"%(xml_path+xml_name,err))
        assert out != "Done", log.F("Error not detected when imported %s]"%(xml_path+xml_name))
        log.I("Test OK : %s is not imported"%(xml_name))
        #Import domains from xml using undeclared configurable element
        xml_name="Uncompliant_UndeclaredConfigurableElement.xml"
        log.I("Import %s without settings"%(xml_name))
        out, err = self.pfw.sendCmd("importDomainsXML",xml_path+xml_name, "")
        assert err == None, log.E("Command [importDomainsXML %s] : %s"%(xml_path+xml_name,err))
        assert out != "Done", log.F("Error not detected when imported %s]"%(xml_path+xml_name))
        log.I("Test OK : %s is not imported"%(xml_name))
        #Import domains from xml using undeclared parameter
        #xml_name="Uncompliant_UndeclaredParameter.xml"
        #log.I("Import %s without settings"%(xml_name))
        #out, err = self.pfw.sendCmd("importDomainsXML",xml_path+xml_name, "")
        #assert err == None, log.E("Command [importDomainsXML %s] : %s"%(xml_path+xml_name,err))
        #assert out != "Done", log.F("Error not detected when imported %s]"%(xml_path+xml_name))
        #log.I("Test OK : %s is not imported"%(xml_name))
        #Import domains from xml using wrong order of configurable element
        xml_name="Uncompliant_UnorderConfigurableElement.xml"
        log.I("Import %s without settings"%(xml_name))
        out, err = self.pfw.sendCmd("importDomainsXML",xml_path+xml_name, "")
        assert err == None, log.E("Command [importDomainsXML %s] : %s"%(xml_path+xml_name,err))
        assert out != "Done", log.F("Error not detected when imported %s]"%(xml_path+xml_name))
        log.I("Test OK : %s is not imported"%(xml_name))
        #Import domains from unexistent xml
        xml_name="Unexistent.xml"
        log.I("Import %s without settings"%(xml_name))
        out, err = self.pfw.sendCmd("importDomainsXML",xml_path+xml_name, "")
        assert err == None, log.E("Command [importDomainsXML %s] : %s"%(xml_path+xml_name,err))
        assert out != "Done", log.F("Error not detected when imported %s]"%(xml_path+xml_name))
        log.I("Test OK : %s is not imported"%(xml_name))
        self.pfw.sendCmd("setTuningMode", "off","")

        #### check domains and settings ####

        #Check number of domain(3 domains are setup in the reference XML, initially only one domains is declared)
        # Domains listing using "listDomains" command
        log.I("Current domains listing")
        log.I("Command [listDomains]")
        out, err = self.pfw.sendCmd("listDomains","","")
        assert err == None, log.E("Command [listDomains] : %s"%(err))
        log.I("Command [listDomains] - correctly executed")
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
        log.I("Actual domains number : %s" % domains_nbr)
        assert domains_nbr==self.nb_domains_in_reference_xml, log.F("Number of listed domains is not compliant with the file %s - expected : %s - found : %s"%(self.reference_xml,self.nb_domains_in_reference_xml, domains_nbr))
        #Check number of config per domain(2 config per domains are setup in the reference XML)
        # Config listing
        domain_basename="Domain_"
        for index in range(self.nb_domains_in_reference_xml):
            domain_name=domain_basename+str(index+1)
            log.I("Listing config for domain %s"%(domain_name))
            out, err = self.pfw.sendCmd("listConfigurations",domain_name,"")
            assert err == None, log.E("Command [listConfigurations %s] : %s"%(domain_name,err))
            log.I("Command [listConfigurations %s] - correctly executed"%(domain_name))
            f_Config_Backup = open(self.temp_config, "w")
            f_Config_Backup.write(out)
            f_Config_Backup.close()
            f_Config_Backup = open(self.temp_config, "r")
            config_nbr = 0
            line=f_Config_Backup.readline()
            while line!="":
                line=f_Config_Backup.readline()
                config_nbr+=1
            f_Config_Backup.close()
            assert config_nbr==self.nb_conf_per_domains_in_reference_xml[index], log.F("Number of listed config for %s is not compliant with the file %s - expected : %s - found : %s"%(domain_name, self.reference_xml,self.nb_conf_per_domains_in_reference_xml[index], domains_nbr))
        log.I("Config checking : OK")

        #### END check domains and settings ####


    @unittest.expectedFailure
    def test_07_Import_Settings_From_Binary_Error_Case(self):
        """
        Testing importDomainsWithSettingsXML error case
        ----------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - import settings from a non-compliant binary
                - import settings from a non-existing file
                - check Domains
                - check Configuration
                - check Parameters
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [importDomainsWithSettingsXML] function
                - [importDomainsXML] function
                - [importSettings] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [restoreConfiguration] function
                - [listDomains] function
                - [listConfiguration] function
                - [getRules] function
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - all errors are detected, initial domains keep settings
        """
        log.D(self.test_07_Import_Settings_From_Binary_Error_Case.__doc__)

        ### INIT Domains Settings ####
        #Import the initial XML file
        log.I("Import Domains with initial settings from %s"%(self.initial_xml))
        out, err = self.pfw.sendCmd("importDomainsWithSettingsXML",self.initial_xml, "")
        assert err == None, log.E("Command [importDomainsWithSettingsXML %s] : %s"%(self.initial_xml,err))
        assert out == "Done", log.F("When using function importDomainsWithSettingsXML %s]"%(self.initial_xml))
        #Export a binary files from the initial setting and config
        log.I("Export settings in the binary files %s"%(self.temp_binary))
        out, err = self.pfw.sendCmd("exportSettings",self.temp_binary, "")
        assert err == None, log.E("Command [exportSettings %s] : %s"%(self.temp_binary,err))
        assert out == "Done", log.F("When using function exportSettings %s]"%(self.temp_binary))
        #Import the reference XML file with another configuration
        log.I("Import Domains with initial settings from %s"%(self.reference_xml))
        out, err = self.pfw.sendCmd("importDomainsWithSettingsXML",self.reference_xml, "")
        assert err == None, log.E("Command [importDomainsWithSettingsXML %s] : %s"%(self.reference_xml,err))
        assert out == "Done", log.F("When using function importDomainsWithSettingsXML %s]"%(self.reference_xml))

        self.pfw.sendCmd("setTuningMode", "off","")
        init_value_01, err = self.pfw.sendCmd("getParameter", self.param_name_01, "")
        init_value_02, err = self.pfw.sendCmd("getParameter", self.param_name_02, "")
        init_value_03, err = self.pfw.sendCmd("getParameter", self.param_name_03, "")
        init_filesystem_01 = commands.getoutput("cat %s"%(self.filesystem_01))
        init_filesystem_02 = commands.getoutput("cat %s"%(self.filesystem_02))
        init_filesystem_03 = commands.getoutput("cat %s"%(self.filesystem_03))
        xml_path="$PFW_TEST_TOOLS/xml/XML_Test/"
        ### END OF INIT ###

        self.pfw.sendCmd("setTuningMode", "on","")
        #Import the temporary binary file, normaly uncompatible with this config
        self.pfw.sendCmd("setTuningMode", "on","")
        log.I("Import settings from %s"%(self.temp_binary))
        out, err = self.pfw.sendCmd("importSettings",self.temp_binary, "")
        assert err == None, log.E("Command [importSettings %s] : %s"%(self.temp_binary,err))
        assert out != "Done", log.F("Error not detected when imported %s]"%(self.temp_binary))
        log.I("Test OK : %s is not imported"%(self.temp_binary))
        #Import setings from a non-existing binary files
        name="Unexistent"
        log.I("Import settings from %s"%(name))
        out, err = self.pfw.sendCmd("importDomainsWithSettingsXML",xml_path+name, "")
        assert err == None, log.E("Command [importDomainsWithSettingsXML %s] : %s"%(xml_path+name,err))
        assert out != "Done", log.F("Error not detected when imported %s]"%(xml_path+name))
        log.I("Test OK : %s is not imported"%(name))
        self.pfw.sendCmd("setTuningMode", "off","")

        #### check domains and settings ####

        #Check number of domain(3 domains are setup in the reference XML, initially only one domains is declared)
        # Domains listing using "listDomains" command
        log.I("Current domains listing")
        log.I("Command [listDomains]")
        out, err = self.pfw.sendCmd("listDomains","","")
        assert err == None, log.E("Command [listDomains] : %s"%(err))
        log.I("Command [listDomains] - correctly executed")
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
        log.I("Actual domains number : %s" % domains_nbr)
        assert domains_nbr==self.nb_domains_in_reference_xml, log.F("Number of listed domains is not compliant with the file %s - expected : %s - found : %s"%(self.reference_xml,self.nb_domains_in_reference_xml, domains_nbr))
        #Check number of config per domain(2 config per domains are setup in the reference XML)
        # Config listing
        domain_basename="Domain_"
        for index in range(self.nb_domains_in_reference_xml):
            domain_name=domain_basename+str(index+1)
            log.I("Listing config for domain %s"%(domain_name))
            out, err = self.pfw.sendCmd("listConfigurations",domain_name,"")
            assert err == None, log.E("Command [listConfigurations %s] : %s"%(domain_name,err))
            log.I("Command [listConfigurations %s] - correctly executed"%(domain_name))
            f_Config_Backup = open(self.temp_config, "w")
            f_Config_Backup.write(out)
            f_Config_Backup.close()
            f_Config_Backup = open(self.temp_config, "r")
            config_nbr = 0
            line=f_Config_Backup.readline()
            while line!="":
                line=f_Config_Backup.readline()
                config_nbr+=1
            f_Config_Backup.close()
            assert config_nbr==self.nb_conf_per_domains_in_reference_xml[index], log.F("Number of listed config for %s is not compliant with the file %s - expected : %s - found : %s"%(domain_name, self.reference_xml,self.nb_conf_per_domains_in_reference_xml[index], domains_nbr))
        log.I("Config checking : OK")
        #Check parameter values
        #UINT16
        expected_value=init_value_01
        hex_value=init_filesystem_01
        log.I("UINT16 parameter in the conf Conf_1_1= %s"%(expected_value))
        out, err = self.pfw.sendCmd("getParameter", self.param_name_01, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name_01, err))
        assert out == expected_value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name_01, expected_value, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_01)) == hex_value, log.F("FILESYSTEM : parameter %s update error"%self.param_name_01)
        #Param_00
        expected_value=init_value_02
        hex_value=init_filesystem_02
        log.I("Param_00 parameter in the conf Conf_2_1= %s"%(expected_value))
        out, err = self.pfw.sendCmd("getParameter", self.param_name_02, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name_02, err))
        assert out == expected_value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name_02, expected_value, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_02)) == hex_value, log.F("FILESYSTEM : parameter %s update error"%self.param_name_02)
        #Param_12
        expected_value=init_value_03
        hex_value=init_filesystem_03
        log.I("Param_12 parameter in the conf Conf_3_1= %s"%(expected_value))
        out, err = self.pfw.sendCmd("getParameter", self.param_name_03, "")
        assert err == None, log.E("When setting parameter %s : %s" % (self.param_name_03, err))
        assert out == expected_value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s" % (self.param_name_03, expected_value, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_03)) == hex_value, log.F("FILESYSTEM : parameter %s update error"%self.param_name_03)
        log.I("Parameters checking : OK")
