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
Split elements from domains testcases

List of tested functions :
--------------------------
    - [splitDomain]  function
    - [listBelongingDomains] function
    - [listAssociatedDomains] function
    - [listAssociatedElements] function
    - [listConflictingElements] function
    - [listRogueElements] function
Test cases :
------------
    - Testing nominal case
"""
import os.path
from Util.PfwUnitTestLib import PfwTestCase
from Util import ACTLogging
log=ACTLogging.Logger()

class TestCases(PfwTestCase):

    def setUp(self):
        self.pfw.sendCmd("setTuningMode", "on")
        self.reference_xml = "$PFW_TEST_TOOLS/xml/XML_Test/Reference_Split_Domain.xml"

        self.temp_domain="f_Domains_Backup"
        self.temp_status="f_Config_Status"

        self.path_main = "/Test/Test/TEST_MAIN/"
        self.path_dir_0 = "/Test/Test/TEST_MAIN/TEST_DIR_0"
        self.path_dir_1 = "/Test/Test/TEST_MAIN/TEST_DIR_1"
        self.path_dir_2 = "/Test/Test/TEST_MAIN/TEST_DIR_2"
        self.dir_nbr = 3
        self.element_name = "TEST_DIR"

        self.domain_1 = "Domain_1"
        self.domain_2 = "Domain_2"
        self.domain_3 = "Domain_3"

        self.temp_file="f_temp_file"

    def tearDown(self):
        self.pfw.sendCmd("setTuningMode", "off")
        if os.path.exists(self.temp_file):
            os.remove(self.temp_file)
    def test_Combinatorial_Criteria(self):
        """
        Testing combinatorial criteria
        ------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - Split a configuration element associated to a domain
                - Check that the configuration element children are associated to the domain
                - Pass a configuration element to rogue element
                - Add a configuration element to another domain and heck that this element is
                conflicting while not removed from original domain.

            Tested commands :
           ~~~~~~~~~~~~~~~~~
                - [splitDomain]  function
                - [listBelongingDomains] function
                - [listAssociatedDomains] function
                - [listAssociatedElements] function
                - [listConflictingElements] function
                - [listRogueElements] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - Conform to expected behavior
        """
        log.D(self.test_Combinatorial_Criteria.__doc__)

        # Import a reference XML file
        log.I("Import Domains with settings from %s"%(self.reference_xml))
        out, err = self.pfw.sendCmd("importDomainsWithSettingsXML",self.reference_xml, "")
        assert err == None, log.E("Command [importDomainsWithSettingsXML %s] : %s"%(self.reference_xml,err))
        assert out == "Done", log.F("When using function importDomainsWithSettingsXML %s]"%(self.reference_xml))

        # Checking initial state
        # Checking domain integrity
        log.I("Checking initial conditions :")
        log.I("Checking that %s configurable element is associated to %s :" % (self.path_main,self.domain_1))
        out, err = self.pfw.sendCmd("listAssociatedDomains", self.path_main)
        assert err == None, log.E("Command [listAssociatedDomains] : error when listing domain name")
        f_temp_file = open(self.temp_file, "w")
        f_temp_file.write(out)
        f_temp_file.close()
        element_name = self.domain_1
        element_found = 0
        for line in open(self.temp_file, "r"):
            if element_name in line:
                element_found = 1
        assert element_found==1, log.F("configurable element %s not correctly associated to domain %s" % (self.path_main, self.domain_1))
        log.I("configurable element %s correctly associated to domain %s" % (self.path_main, self.domain_1))
        # Deleting temp file
        os.remove(self.temp_file)

        # Checking children integrity
        log.I("Checking that %s children configurable elements are correctly set for the test" % (self.path_main))
        out, err = self.pfw.sendCmd("listElements", self.path_main)
        assert err == None, log.E("Command [listElements] : listing error")
        f_temp_file = open(self.temp_file, "w")
        f_temp_file.write(out)
        f_temp_file.close()
        for index in range (self.dir_nbr):
            element_name = "".join([self.element_name, "_", str(index)])
            element_found = 0
            for line in open(self.temp_file, "r"):
                if element_name in line:
                    element_found = 1
            assert element_found==1, log.F("Element %s not found in %s" % (element_name, self.path_main))
            log.I("Element %s found in %s" % (element_name, self.path_main))
            # Checking that child element belong to domain
            element_path = "".join([self.path_main, element_name])
            out, err = self.pfw.sendCmd("listBelongingDomains", element_path)
            assert err == None, log.E("Command [listBelongingDomains] : listing error")
            assert out == self.domain_1, log.F("Wrong behavior : %s not belonging to %s " % (element_name, self.domain_1))
            # Checking that child element is not associated to domain, and only belong to it
            out, err = self.pfw.sendCmd("listAssociatedDomains", element_path)
            assert err == None, log.E("Command [listAssociatedDomains] : listing error")
            assert out == '', log.F("Wrong behavior : configurable element %s not associated to %s" % (element_name, self.domain_1))
            log.I("configurable element %s is belonging to %s" % (element_name, self.domain_1))
        log.I("Configurable elements : check OK")
        # Deleting temp file
        os.remove(self.temp_file)

        # Split domain_0
        log.I("Splitting configurable element %s from %s" % (self.path_main, self.domain_1))
        out, err = self.pfw.sendCmd("splitDomain", self.domain_1, self.path_main)
        assert err == None, log.E("Command [splitDomain] : %s" % (err))
        assert out == 'Done', log.F("Wrong behavior : configurable element %s not splitted correctly" % (self.path_main))
        log.I("Splitting done")

        # check that the configurable element splitted is neither associated nor belonging to the domain
        log.I("Checking that %s is neither associated nor belonging to %s" % (self.path_main, self.domain_1))
        out, err = self.pfw.sendCmd("listBelongingDomains", self.path_main)
        assert err == None, log.E("Command [listBelongingDomains] : listing error")
        assert out != self.domain_1, log.F("Wrong behavior : %s still belonging to %s" % (self.path_main, self.domain_1))
        out, err = self.pfw.sendCmd("listAssociatedDomains", self.path_main)
        assert err == None, log.E("Command [listAssociatedDomains] : listing error")
        assert out == '', log.F("Wrong behavior : configurable element %s still associated to %s" % (self.path_main, self.domain_1))
        log.I("Configurable element %s is no longer associated to %s" % (self.path_main, self.domain_1))

        # Checking that children configurable elements are now associated to domain
        log.I("Checking that %s children configurable elements are now associated to %s" % (self.path_main, self.domain_1))
        out, err = self.pfw.sendCmd("listElements", self.path_main)
        assert err == None, log.E("Command [listElements] : listing error")
        f_temp_file = open(self.temp_file, "w")
        f_temp_file.write(out)
        f_temp_file.close()
        for index in range (self.dir_nbr):
            element_name = "".join([self.element_name, "_", str(index)])
            element_found = 0
            for line in open(self.temp_file, "r"):
                if element_name in line:
                    element_found = 1
            assert element_found==1, log.F("Element %s not found in %s" % (element_name, self.path_main))
            log.I("Element %s found in %s" % (element_name, self.path_main))
            # Checking that child element is associated to domain
            element_path = "".join([self.path_main, element_name])
            out, err = self.pfw.sendCmd("listAssociatedDomains", element_path)
            assert err == None, log.E("Command [listAssociatedDomains] : listing error")
            assert out == self.domain_1, log.F("Wrong behavior : configurable element %s not associated to %s" % (element_name, self.domain_1))
            log.I("configurable element %s is associated to %s" % (element_name, self.domain_1))
        log.I("Configurable elements : check OK")
        # Deleting temp file
        os.remove(self.temp_file)

        # Removing one element from domain and checking that it becomes a rogue element
        log.I("Removing domain element %s from domain %s" % (self.path_dir_0, self.domain_1))
        out, err = self.pfw.sendCmd("removeElement", str(self.domain_1), str(self.path_dir_0))
        assert err == None, log.E("ERROR : command [removeElement] - Error while removing domain element %s" % (self.path_dir_0))
        assert out == "Done", log.F("Domain element %s not correctly removed" % (self.path_dir_0))
        log.I("Domain element %s correctly removed from domain %s" % (self.path_dir_0, self.domain_1))
        log.I("Checking that %s is a rogue element" % (self.path_dir_0))
        out, err = self.pfw.sendCmd("listRogueElements")
        assert err == None, log.E("command [listRogueElements] - Error while listing rogue elements")
        f_temp_file = open(self.temp_file, "w")
        f_temp_file.write(out)
        f_temp_file.close()
        element_found = 0
        for line in open(self.temp_file, "r"):
            if self.path_dir_0 in line:
                element_found = 1
        assert element_found==1, log.F("Configurable element %s not found in rogue elements" % (self.path_dir_0))
        log.I("Element %s found in rogue elements" % (self.path_dir_0))

        # Moving one configuration element to another domain
        log.I("Moving configurable element %s from domain %s to domain %s" % (self.path_dir_1, self.domain_1, self.domain_2))
        log.I("Adding %s to domain %s" % (self.path_dir_1, self.domain_2))
        out, err = self.pfw.sendCmd("addElement", self.domain_2, self.path_dir_1)
        assert err == None, log.E("ERROR : command [addElement] - Error while adding element %s to domain %s" % (self.path_dir_1, self.domain_2))
        out, err = self.pfw.sendCmd("listConflictingElements")
        assert err == None, log.E("command [listConflictingElements] - Error while listing conflicting elements")
        f_temp_file = open(self.temp_file, "w")
        f_temp_file.write(out)
        f_temp_file.close()
        element_found = 0
        for line in open(self.temp_file, "r"):
            if self.path_dir_1 in line:
                element_found = 1
        assert element_found==1, log.F("Configurable element %s not found in conflicting elements" % (self.path_dir_1))
        log.I("Element %s found in conflicting elements" % (self.path_dir_1))
        log.I("Removing %s from domain %s" % (self.path_dir_1, self.domain_1))
        out, err = self.pfw.sendCmd("removeElement", self.domain_1, self.path_dir_1)
        assert err == None, log.E("ERROR : command [removeElement] - Error while removing element %s from domain %s" % (self.path_dir_1, self.domain_2))
        out, err = self.pfw.sendCmd("listConflictingElements")
        assert err == None, log.E("command [listConflictingElements] - Error while listing conflicting elements")
        f_temp_file = open(self.temp_file, "w")
        f_temp_file.write(out)
        f_temp_file.close()
        element_found = 0
        for line in open(self.temp_file, "r"):
            if self.path_dir_1 in line:
                element_found = 1
        assert element_found!=1, log.F("Configurable element %s still found in conflicting elements" % (self.path_dir_1))
        log.I("Element %s no longer found in conflicting elements" % (self.path_dir_1))
