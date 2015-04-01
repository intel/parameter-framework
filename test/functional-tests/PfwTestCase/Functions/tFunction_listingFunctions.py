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
All listing and dumping function testcases.

List of tested functions :
--------------------------
    - [dumpDomains]  function
    - [dumpElement] function

Test cases :
------------
    - Testing dumpDomains function on nominal case
    - Testing dumpElements function on nominal case
"""
import commands, os
import unittest
from Util.PfwUnitTestLib import PfwTestCase
from Util import ACTLogging
log=ACTLogging.Logger()

class TestCases(PfwTestCase):

    def setUp(self):

        self.pfw.sendCmd("setTuningMode", "on")

        pfw_test_tools=os.getenv("PFW_TEST_TOOLS")
        self.reference_dumpDomains_xml = pfw_test_tools+"/xml/XML_Test/Reference_dumpDomains.xml"
        self.reference_dumpDomains_file = pfw_test_tools+"/xml/XML_Test/Reference_dumpDomains"
        self.initial_xml = pfw_test_tools+"/xml/TestConfigurableDomains.xml"

        self.list_domains=[]
        self.list_criteria=["Crit_0", "Crit_1"]
        self.list_parameters=[]
        self.temp_file="tempfile"

        self.domain_name = "Domain_0"
        self.config_name = "Conf_0"

    def tearDown(self):
        self.pfw.sendCmd("setTuningMode", "off")
        if os.path.exists(self.temp_file):
            os.remove(self.temp_file)

    def test_01_dumpDomains_Case(self):
        """
        Testing dumpDomains function
        ----------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - import a reference XML : Reference_DumpDomains.xml
                - dumpDomains
                - compare out to a reference file : Reference_DumpDomains
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [dumpDomains] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [importDomainsWithSettingsXML] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - string stdout due to dumpDomains is the same than string in
                the reference file
        """
        log.D(self.test_01_dumpDomains_Case.__doc__)

        #Import a reference XML file
        log.I("Import Domains with settings from %s"%(self.reference_dumpDomains_xml))
        out, err = self.pfw.sendCmd("importDomainsWithSettingsXML",self.reference_dumpDomains_xml, "")
        assert err == None, log.E("Command [importDomainsWithSettingsXML %s] : %s"%(self.reference_dumpDomains_xml,err))
        assert out == "Done", log.F("When using function importDomainsWithSettingsXML %s]"%(self.reference_dumpDomains_xml))

        log.I("Command [dumpDomains]")
        out, err = self.pfw.sendCmd("dumpDomains","","")
        assert err == None, log.E("Command [dumpDomains] : %s"%(err))
        assert out == commands.getoutput("cat %s"%(self.reference_dumpDomains_file)), log.F("A diff is found between dumpDomains output and %s"%(self.reference_dumpDomains_file))
        log.I("Command [dumpDomains] - correctly executed")

    def test_03_help_Case(self):
        """
        Testing help function
        ---------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - help
                - check results
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [help] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - string stdout due to help is not empty
        """
        log.D(self.test_03_help_Case.__doc__)
        log.I("Command [help]")
        out, err = self.pfw.sendCmd("help","")
        assert err == None, log.E("Command [help] : %s"%(err))
        assert out != ""
        log.I("Command [help] - correctly executed")

    def test_04_status_Case(self):
        """
        Testing status function
        -----------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - status
                - check results
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [status] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - string stdout due to status is not empty
        """
        log.D(self.test_04_status_Case.__doc__)
        log.I("Command [status]")
        out, err = self.pfw.sendCmd("status","")
        assert err == None, log.E("Command [help] : %s"%(err))
        assert out != ""
        log.I("Command [status] - correctly executed")

    def test_05_listCriteria_Case(self):
        """
        Testing listCriteria function
        -----------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - listCriteria
                - check results
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [listCriteria] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - string stdout due to listCriteria is not empty
        """
        log.D(self.test_05_listCriteria_Case.__doc__)
        log.I("Command [listCriteria]")
        out, err = self.pfw.sendCmd("listCriteria","")
        assert err == None, log.E("Command [listCriteria] : %s"%(err))
        assert out != ""
        log.I("Command [listCriteria] - correctly executed")

    def test_06_listDomains_Case(self):
        """
        Testing listDomains function
        ----------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - listDomains
                - check results
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [listDomains] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - string stdout due to listDomains is not empty
        """
        log.D(self.test_06_listDomains_Case.__doc__)
        log.I("Command [listDomains]")
        out, err = self.pfw.sendCmd("listDomains")
        assert err == None, log.E("Command [listDomains] : %s"%(err))
        assert out != ""
        log.I("Command [listDomains] - correctly executed")

    @unittest.expectedFailure
    def test_06_listDomainElements_Case(self):
        """
        Testing listDomains function
        ----------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - listDomainElements
                - check results
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [listDomainElements] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - string stdout due to listDomains is not empty
        """
        log.D(self.test_06_listDomainElements_Case.__doc__)
        log.I("Command [listDomainElements]")
        out, err = self.pfw.sendCmd("listDomainElements",self.domain_name)
        assert err == None, log.E("Command [listDomainElements] : %s"%(err))
        assert out != "", log.F("Fail when listDomainElements %s: stdout is empty"%(self.domain_name))
        log.I("Command [listDomainElements] - correctly executed")

    @unittest.expectedFailure
    def test_07_listConfigurations_Case(self):
        """
        Testing listConfigurations function
        -----------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - listConfigurations
                - check results
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [listConfigurations] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - string stdout due to listConfigurations is not empty
        """
        log.D(self.test_07_listConfigurations_Case.__doc__)
        log.I("Command [listConfigurations]")
        out, err = self.pfw.sendCmd("listConfigurations",self.domain_name)
        assert err == None, log.E("Command [listConfigurations] : %s"%(err))
        assert out != "", log.F("Fail when listConfigurations %s: stdout is empty"%(self.domain_name))
        log.I("Command [listConfigurations] - correctly executed")

    def test_08_listElements_Case(self):
        """
        Testing listElements function
        -----------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - listElements
                - check results
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [listElements] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - string stdout due to listElements is not empty
        """
        log.D(self.test_08_listElements_Case.__doc__)
        log.I("Command [listElements]")
        out, err = self.pfw.sendCmd("listElements")
        assert err == None, log.E("Command [listElements] : %s"%(err))
        out, err = self.pfw.sendCmd("listElements","/Test/")
        assert err == None, log.E("Command [listElements /Test/] : %s"%(err))
        assert out != ""
        log.I("Command [listElements] - correctly executed")

    def test_09_listParameters_Case(self):
        """
        Testing listParameters function
        -------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - listParameters
                - check results
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [listParameters] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - string stdout due to listParameters is not empty
        """
        log.D(self.test_09_listParameters_Case.__doc__)
        log.I("Command [listParameters]")
        out, err = self.pfw.sendCmd("listParameters")
        assert err == None, log.E("Command [listParameters] : %s"%(err))
        out, err = self.pfw.sendCmd("listParameters","/Test/")
        assert err == None, log.E("Command [listParameters /Test/] : %s"%(err))
        assert out != ""
        log.I("Command [listParameters] - correctly executed")

    def test_10_getElementSize_Case(self):
        """
        Testing getElementSize function
        -------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - listParameters
                - getElementSize for all parameters
                - check results
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [getElementSize] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [listParameters] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - string stdout due to getElementSize is not empty
        """
        log.D(self.test_10_getElementSize_Case.__doc__)
        log.I("Command [listParameters]")
        out, err = self.pfw.sendCmd("listParameters","/Test/")
        assert err == None, log.E("Command [listParameters /Test/] : %s"%(err))
        assert out != ""
        log.I("Command [listParameters] - correctly executed")
        # Write out in temp file
        f_temp_file = open(self.temp_file, "w")
        f_temp_file.write(out)
        f_temp_file.close()

        # Extract parameter from the temp file
        f_temp_file = open(self.temp_file, "r")
        lines = f_temp_file.readlines()
        f_temp_file.close()

        for line in lines :
            if not line.find("/") == -1 :
                final_position_in_line= line.find("[")-1
                self.list_parameters.append(line[0:final_position_in_line])

        for parameter in self.list_parameters :
            out, err = self.pfw.sendCmd("getElementSize",parameter)
            assert err == None, log.E("Command [getElementSize %s] : %s"%(parameter,err))
            assert out != ""

        out, err = self.pfw.sendCmd("getElementSize","/Test/")
        assert err == None, log.E("Command [getElementSize /Test/] : %s"%(err))
        assert out != ""

        out, err = self.pfw.sendCmd("getElementSize")
        assert err == None, log.E("Command [getElementSize /Test/] : %s"%(err))

    def test_11_showProperties_Case(self):
        """
        Testing showProperties function
        -------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - listParameters
                - showProperties for all parameters
                - check results
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [showProperties] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [listParameters] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - string stdout due to getElementSize is not empty
        """
        log.D(self.test_11_showProperties_Case.__doc__)
        log.I("Command [listParameters]")
        out, err = self.pfw.sendCmd("listParameters","/Test/")
        assert err == None, log.E("Command [listParameters /Test/] : %s"%(err))
        assert out != ""
        log.I("Command [listParameters] - correctly executed")
        # Write out in temp file
        f_temp_file = open(self.temp_file, "w")
        f_temp_file.write(out)
        f_temp_file.close()

        # Extract parameter from the temp file
        f_temp_file = open(self.temp_file, "r")
        lines = f_temp_file.readlines()
        f_temp_file.close()

        for line in lines :
            if not line.find("/") == -1 :
                final_position_in_line= line.find("[")-1
                self.list_parameters.append(line[0:final_position_in_line])

        for parameter in self.list_parameters :
            out, err = self.pfw.sendCmd("showProperties",parameter)
            assert err == None, log.E("Command [showProperties %s] : %s"%(parameter,err))
            assert out != ""

        out, err = self.pfw.sendCmd("showProperties","/Test/")
        assert err == None, log.E("Command [showProperties /Test/] : %s"%(err))
        assert out != ""

        out, err = self.pfw.sendCmd("showProperties")
        assert err == None, log.E("Command [showProperties] : %s"%(err))

    def test_12_listBelongingDomains_Case(self):
        """
        Testing listBelongingDomains function
        -------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - listParameters
                - listBelongingDomains for all parameters
                - check results
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [listBelongingDomains] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [listParameters] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - string stdout due to listBelongingDomains is not empty
        """
        log.D(self.test_12_listBelongingDomains_Case.__doc__)
        log.I("Command [listParameters]")
        out, err = self.pfw.sendCmd("listParameters","/Test/")
        assert err == None, log.E("Command [listParameters /Test/] : %s"%(err))
        assert out != ""
        log.I("Command [listParameters] - correctly executed")
        # Write out in temp file
        f_temp_file = open(self.temp_file, "w")
        f_temp_file.write(out)
        f_temp_file.close()

        # Extract parameter from the temp file
        f_temp_file = open(self.temp_file, "r")
        lines = f_temp_file.readlines()
        f_temp_file.close()

        for line in lines :
            if not line.find("/") == -1 :
                final_position_in_line= line.find("[")-1
                self.list_parameters.append(line[0:final_position_in_line])

        for parameter in self.list_parameters :
            out, err = self.pfw.sendCmd("listBelongingDomains",parameter)
            assert err == None, log.E("Command [listBelongingDomains %s] : %s"%(parameter,err))

        out, err = self.pfw.sendCmd("listBelongingDomains","/Test/")
        assert err == None, log.E("Command [listBelongingDomains /Test/] : %s"%(err))

        out, err = self.pfw.sendCmd("listBelongingDomains")
        assert err == None, log.E("Command [listBelongingDomains] : %s"%(err))

    def test_13_listAssociatedDomains_Case(self):
        """
        Testing listAssociatedDomains function
        -------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - listParameters
                - listAssociatedDomains for all parameters
                - check results
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [listAssociatedDomains] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [listParameters] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - string stdout due to listBelongingDomains is not empty
        """
        log.D(self.test_13_listAssociatedDomains_Case.__doc__)
        log.I("Command [listParameters]")
        out, err = self.pfw.sendCmd("listParameters","/Test/")
        assert err == None, log.E("Command [listParameters /Test/] : %s"%(err))
        log.I("Command [listParameters] - correctly executed")
        # Write out in temp file
        f_temp_file = open(self.temp_file, "w")
        f_temp_file.write(out)
        f_temp_file.close()

        # Extract parameter from the temp file
        f_temp_file = open(self.temp_file, "r")
        lines = f_temp_file.readlines()
        f_temp_file.close()

        for line in lines :
            if not line.find("/") == -1 :
                final_position_in_line= line.find("[")-1
                self.list_parameters.append(line[0:final_position_in_line])

        for parameter in self.list_parameters :
            out, err = self.pfw.sendCmd("listAssociatedDomains",parameter)
            assert err == None, log.E("Command [listAssociatedDomains %s] : %s"%(parameter,err))

        out, err = self.pfw.sendCmd("listAssociatedDomains","/Test/")
        assert err == None, log.E("Command [listAssociatedDomains /Test/] : %s"%(err))

    def test_14_listAssociatedElements_Case(self):
        """
        Testing listAssociatedElements function
        -------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - listAssociatedElements
                - check results
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [listAssociatedElements] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - string stdout due to listAssociatedElements is not empty
        """
        log.D(self.test_14_listAssociatedElements_Case.__doc__)
        log.I("Command [listAssociatedElements]")
        out, err = self.pfw.sendCmd("listAssociatedElements")
        assert err == None, log.E("Command [listAssociatedElements] : %s"%(err))
        log.I("Command [listAssociatedElements] - correctly executed")

    def test_15_listConflictingElements_Case(self):
        """
        Testing listConflictingElements function
        -------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - listConflictingElements
                - check results
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [listConflictingElements] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - string stdout due to listConflictingElements is not empty
        """
        log.D(self.test_15_listConflictingElements_Case.__doc__)
        log.I("Command [listConflictingElements]")
        out, err = self.pfw.sendCmd("listConflictingElements")
        assert err == None, log.E("Command [listConflictingElements] : %s"%(err))
        log.I("Command [listConflictingElements] - correctly executed")

    def test_16_listRogueElements_Case(self):
        """
        Testing listRogueElements function
        -------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - listRogueElements
                - check results
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [listRogueElements] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - string stdout due to listRogueElements is not empty
        """
        log.D(self.test_16_listRogueElements_Case.__doc__)
        log.I("Command [listRogueElements]")
        out, err = self.pfw.sendCmd("listRogueElements")
        assert err == None, log.E("Command [listRogueElements] : %s"%(err))
        log.I("Command [listRogueElements] - correctly executed")
