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
Adding and Removing elements from domain testcases

List of tested functions :
--------------------------
    - [listDomainElements]  function
    - [addElement] function
    - [removeElement] function

Test cases :
------------
    - Testing nominal case
    - Testing addElement errors
    - Testing removeElement errors
"""
import os
from Util.PfwUnitTestLib import PfwTestCase
from Util import ACTLogging
log=ACTLogging.Logger()

class TestCases(PfwTestCase):
    def setUp(self):
        self.pfw.sendCmd("setTuningMode", "on")
        self.domain_name = "Domain_0"
        self.elem_0_path = "/Test/Test/TEST_DIR"
        self.elem_1_path = "/Test/Test/TEST_DOMAIN_0"
        self.elem_2_path = "/Test/Test/TEST_DOMAIN_1"

    def tearDown(self):
        self.pfw.sendCmd("setTuningMode", "off")

    def test_Nominal_Case(self):
        """
        Testing nominal case
        --------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - list and backup initial domain elements
                - add a domain element
                - remove a domain element
                - list and check domains elements
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [listDomainElements] function
                - [addElement] function
                - [removeElement] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - all operations succeed
        """
        log.D(self.test_Nominal_Case.__doc__)

        # List and backup initial domain elements
        log.I("Listing initial domain %s elements" % (self.domain_name))
        out, err = self.pfw.sendCmd("listDomainElements",str(self.domain_name))
        assert err == None, "ERROR : command [listDomainElements] - Error while listing domain elements"
        f_DomainElements_Backup = open("f_DomainElements_Backup", "w")
        f_DomainElements_Backup.write(out)
        f_DomainElements_Backup.close()
        log.I("command [listDomainElements] correctly executed")
        f_DomainElements_Backup = open("f_DomainElements_Backup", "r")
        element_nbr_init = 0
        line=f_DomainElements_Backup.readline()
        while line!="":
            line=f_DomainElements_Backup.readline()
            element_nbr_init+=1
        f_DomainElements_Backup.close()
        log.I("Actual domain %s elements number is %s" % (self.domain_name,element_nbr_init))

        # Adding a new domain element
        log.I("Adding a new domain element to domain %s" % (self.domain_name))
        out, err = self.pfw.sendCmd("addElement", str(self.domain_name), str(self.elem_1_path))
        assert err == None, "ERROR : command [addElement] - Error while adding new domain element %s" % (self.elem_1_path)
        assert out == "Done", "ERROR : command [addElement] - Error while adding new domain element %s" % (self.elem_1_path)
        log.I("Adding a new domain element to domain %s" % (self.domain_name))
        out, err = self.pfw.sendCmd("addElement", str(self.domain_name), str(self.elem_2_path))
        assert err == None, "ERROR : command [addElement] - Error while adding new domain element %s" % (self.elem_2_path)
        assert out == "Done", "ERROR : command [addElement] - Error while adding new domain element %s" % (self.elem_2_path)
        log.I("New domain elements %s and %s added to domain %s" % (self.elem_1_path, self.elem_2_path, self.domain_name))

        # Removing a domain element
        log.I("Removing domain element %s from domain %s" % (self.elem_1_path,self.domain_name))
        out, err = self.pfw.sendCmd("removeElement", str(self.domain_name), str(self.elem_1_path))
        assert err == None, "ERROR : command [removeElement] - Error while removing domain element %s" % (self.elem_1_path)
        assert out == "Done", "ERROR : command [removeElement] - Error while removing domain element %s" % (self.elem_1_path)

        # Checking final domain elements
        log.I("Listing final domain %s elements" % (self.domain_name))
        out, err = self.pfw.sendCmd("listDomainElements",str(self.domain_name))
        assert err == None, "ERROR : command [listDomainElements] - Error while listing domain elements"
        f_DomainElements = open("f_DomainElements", "w")
        f_DomainElements.write(out)
        f_DomainElements.close()
        log.I("command [listDomainElements] correctly executed")
        f_DomainElements = open("f_DomainElements", "r")
        element_nbr = 0
        line=f_DomainElements.readline()
        while line!="":
            line=f_DomainElements.readline()
            element_nbr+=1
        f_DomainElements.close()
        log.I("Actual domain %s elements number is %s" % (self.domain_name,element_nbr))
        log.I("Checking domain %s elements names conformity" % (self.domain_name))
        f_DomainElements = open("f_DomainElements", "r")
        f_DomainElements_Backup = open("f_DomainElements_Backup", "r")
        for line in range(element_nbr):
            # initial domain elements shall not have been impacted by current test
            if (line < element_nbr_init):
                element_name = f_DomainElements.readline().strip('\n')
                element_name_backup = f_DomainElements_Backup.readline().strip('\n')
                assert element_name==element_name_backup, "ERROR : Error while modifying domain elements on domain %s" % (self.domain_name)
            # last listed element shall be equal to the only one element added previously
            else:
                element_name = f_DomainElements.readline().strip('\n')
                assert element_name==str(self.elem_2_path), "ERROR : Error while modifying domain elements on domain %s" % (self.domain_name)
        log.I("Actual domain %s elements names conform to expected values" % (self.domain_name))
        # Temporary files deletion
        f_DomainElements.close()
        f_DomainElements_Backup.close()
        os.remove("f_DomainElements_Backup")
        os.remove("f_DomainElements")
        # Removing created domain element
        out, err = self.pfw.sendCmd("removeElement", str(self.domain_name), str(self.elem_2_path))
        assert err == None, "ERROR : command [removeElement] - Error while removing domain element %s" % (self.elem_2_path)
        assert out == "Done", "ERROR : command [removeElement] - Error while removing domain element %s" % (self.elem_2_path)

    def test_addElement_Error(self):
        """
        Testing addElement error
        ------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - add an already existing domain element
                - add a non defined domain element
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [addElement] function
                - [listDomainElements] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - Errors correctly detected
                - No side effect
        """
        log.D(self.test_addElement_Error.__doc__)

        # List and backup initial domain elements
        log.I("Listing initial domain %s elements" % (self.domain_name))
        out, err = self.pfw.sendCmd("listDomainElements",str(self.domain_name))
        assert err == None, "ERROR : command [listDomainElements] - Error while listing domain elements"
        f_DomainElements_Backup = open("f_DomainElements_Backup", "w")
        f_DomainElements_Backup.write(out)
        f_DomainElements_Backup.close()
        log.I("command [listDomainElements] correctly executed")
        f_DomainElements_Backup = open("f_DomainElements_Backup", "r")
        element_nbr_init = 0
        line=f_DomainElements_Backup.readline()
        while line!="":
            line=f_DomainElements_Backup.readline()
            element_nbr_init+=1
        f_DomainElements_Backup.close()
        log.I("Actual domain %s elements number is %s" % (self.domain_name,element_nbr_init))

        # Adding a new domain element errors
        log.I("Adding an already existing domain element to domain %s" % (self.domain_name))
        out, err = self.pfw.sendCmd("addElement", str(self.domain_name), str(self.elem_0_path))
        assert err == None, "ERROR : command [addElement] - Error while adding new domain element %s" % (self.elem_0_path)
        assert out != "Done", "ERROR : command [addElement] - Error not detected while adding an already existing domain element to domain %s" % (self.domain_name)
        log.I("Adding a non defined domain element to domain %s" % (self.domain_name))
        out, err = self.pfw.sendCmd("addElement", str(self.domain_name), "Non_Defined_Element")
        assert err == None, "ERROR : command [addElement] - Error while adding new domain element %s" % (self.elem_2_path)
        assert out != "Done", "ERROR : command [addElement] - Error not detected while adding a non defined domain element to domain %s" % (self.domain_name)
        log.I("Error when adding elements correctly detected")

        # Checking final domain elements
        log.I("Listing final domain %s elements" % (self.domain_name))
        out, err = self.pfw.sendCmd("listDomainElements",str(self.domain_name))
        assert err == None, "ERROR : command [listDomainElements] - Error while listing domain elements"
        f_DomainElements = open("f_DomainElements", "w")
        f_DomainElements.write(out)
        f_DomainElements.close()
        log.I("command [listDomainElements] correctly executed")
        f_DomainElements = open("f_DomainElements", "r")
        element_nbr = 0
        line=f_DomainElements.readline()
        while line!="":
            line=f_DomainElements.readline()
            element_nbr+=1
        f_DomainElements.close()
        log.I("Actual domain %s elements number is %s" % (self.domain_name,element_nbr))
        log.I("Checking domain %s elements names conformity" % (self.domain_name))
        f_DomainElements = open("f_DomainElements", "r")
        f_DomainElements_Backup = open("f_DomainElements_Backup", "r")
        for line in range(element_nbr):
            # initial domain elements shall not have been impacted by current test
            element_name = f_DomainElements.readline().strip('\n')
            element_name_backup = f_DomainElements_Backup.readline().strip('\n')
            assert element_name==element_name_backup, "ERROR : domain %s elements affected by addElement errors" % (self.domain_name)
        log.I("Actual domain %s elements names conform to expected values" % (self.domain_name))
        # Temporary files deletion
        f_DomainElements.close()
        f_DomainElements_Backup.close()
        os.remove("f_DomainElements_Backup")
        os.remove("f_DomainElements")

    def test_removeElement_Error(self):
        """
        Testing removeElement error
        ---------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - remove a non defined domain element
                - remove a domain element on a wrong domain name
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [removeElement] function
                - [listDomainElements] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - Errors correctly detected
                - No side effect
        """
        log.D(self.test_removeElement_Error.__doc__)

        # List and backup initial domain elements
        log.I("Listing initial domain %s elements" % (self.domain_name))
        out, err = self.pfw.sendCmd("listDomainElements",str(self.domain_name))
        assert err == None, "ERROR : command [listDomainElements] - Error while listing domain elements"
        f_DomainElements_Backup = open("f_DomainElements_Backup", "w")
        f_DomainElements_Backup.write(out)
        f_DomainElements_Backup.close()
        log.I("command [listDomainElements] correctly executed")
        f_DomainElements_Backup = open("f_DomainElements_Backup", "r")
        element_nbr_init = 0
        line=f_DomainElements_Backup.readline()
        while line!="":
            line=f_DomainElements_Backup.readline()
            element_nbr_init+=1
        f_DomainElements_Backup.close()
        log.I("Actual domain %s elements number is %s" % (self.domain_name,element_nbr_init))

        # Error when removing domain elements
        log.I("Removing a domain element from a non defined domain")
        out, err = self.pfw.sendCmd("removeElement", "Wrong_Domain_Name", str(self.elem_0_path))
        assert err == None, "ERROR : command [removeElement] - Error when removing domain element %s" % (self.elem_0_path)
        assert out != "Done", "ERROR : command [removeElement] - Error not detected when removing domain element %s from an undefined domain"% (self.elem_0_path)
        log.I("Removing a non existent domain element from domain %s" % (self.domain_name))
        out, err = self.pfw.sendCmd("removeElement", str(self.domain_name), "Wrong_Element_Name")
        assert err == None, "ERROR : command [removeElement] - Error when removing domain element %s" % (self.elem_0_path)
        assert out != "Done", "ERROR : command [removeElement] - Error not detected when removing a non existent domain element from domain %s" % (self.domain_name)
        log.I("Error when removing elements correctly detected")

        # Checking final domain elements
        log.I("Listing final domain %s elements" % (self.domain_name))
        out, err = self.pfw.sendCmd("listDomainElements",str(self.domain_name))
        assert err == None, "ERROR : command [listDomainElements] - Error while listing domain elements"
        f_DomainElements = open("f_DomainElements", "w")
        f_DomainElements.write(out)
        f_DomainElements.close()
        log.I("command [listDomainElements] correctly executed")
        f_DomainElements = open("f_DomainElements", "r")
        element_nbr = 0
        line=f_DomainElements.readline()
        while line!="":
            line=f_DomainElements.readline()
            element_nbr+=1
        f_DomainElements.close()
        log.I("Actual domain %s elements number is %s" % (self.domain_name,element_nbr))
        log.I("Checking domain %s elements names conformity" % (self.domain_name))
        f_DomainElements = open("f_DomainElements", "r")
        f_DomainElements_Backup = open("f_DomainElements_Backup", "r")
        for line in range(element_nbr):
            # initial domain elements shall not have been impacted by current test
            element_name = f_DomainElements.readline().strip('\n')
            element_name_backup = f_DomainElements_Backup.readline().strip('\n')
            assert element_name==element_name_backup, "ERROR : domain %s elements affected by addElement errors" % (self.domain_name)
        log.I("Actual domain %s elements names conform to expected values" % (self.domain_name))
        # Temporary files deletion
        f_DomainElements.close()
        f_DomainElements_Backup.close()
        os.remove("f_DomainElements_Backup")
        os.remove("f_DomainElements")
