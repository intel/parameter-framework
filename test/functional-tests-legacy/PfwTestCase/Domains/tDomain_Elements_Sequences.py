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
Element Sequence testcases

List of tested functions :
--------------------------
    - [setElementSequence]  function
    - [getElementSequence] function

Test cases :
------------
    - Testing setElementSequence errors
    - Testing getElementSequence errors
    - Testing nominal case
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
        self.configuration = "Conf_0"

    def tearDown(self):
        self.pfw.sendCmd("setTuningMode", "off")

    def test_Nominal_Case(self):
        """
        Testing nominal case
        --------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set a new sequences order for a selected configuration
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setElementSequence] function
                - [getElementSequence] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - all operations succeed
                - new sequences order conform to expected order
        """
        log.D(self.test_Nominal_Case.__doc__)

        # Adding new domain elements
        log.I("Working on domain %s" % (self.domain_name))
        log.I("Adding a new domain element to domain %s" % (self.domain_name))
        out, err = self.pfw.sendCmd("addElement", str(self.domain_name), str(self.elem_1_path))
        assert err == None, "ERROR : command [addElement] - Error while adding new domain element %s" % (self.elem_1_path)
        assert out == "Done", "ERROR : command [addElement] - Error while adding new domain element %s" % (self.elem_1_path)
        log.I("Adding a new domain element to domain %s" % (self.domain_name))
        out, err = self.pfw.sendCmd("addElement", str(self.domain_name), str(self.elem_2_path))
        assert err == None, "ERROR : command [addElement] - Error while adding new domain element %s" % (self.elem_2_path)
        assert out == "Done", "ERROR : command [addElement] - Error while adding new domain element %s" % (self.elem_2_path)
        log.I("New domain elements %s and %s added to domain %s" % (self.elem_1_path, self.elem_2_path, self.domain_name))

        # Getting elements sequence from selected configuration
        log.I("Getting elements sequence from configuration %s" % (self.configuration))
        out, err = self.pfw.sendCmd("getElementSequence", self.domain_name, self.configuration)
        assert err == None, "ERROR : command [getElementSequence] - Error while listing elements sequence for configuration %s" % (self.configuration)
        log.I("Listing elements sequence for configuration %s correctly executed :\n%s" % (self.configuration, out))

        # Setting new elements sequence order for selected configuration
        log.I("Setting new elements sequence order for configuration %s" % (self.configuration))
        out, err = self.pfw.sendCmd("setElementSequence", self.domain_name, self.configuration, self.elem_2_path, self.elem_0_path, self.elem_1_path)
        assert err == None, "ERROR : command [setElementSequence] - Error while setting new elements sequence for configuration %s" % (self.configuration)
        assert out == "Done", "ERROR : command [setElementSequence] - Error while setting new elements sequence for configuration %s" % (self.configuration)
        log.I("Setting new elements sequence for configuration %s correctly executed")
        out, err = self.pfw.sendCmd("getElementSequence", self.domain_name, self.configuration)
        assert err == None, "ERROR : command [getElementSequence] - Error while listing elements sequence for configuration %s" % (self.configuration)
        log.I("New elements sequence for configuration %s :\n%s" % (self.configuration, out))

        # Checking new elements sequence order conformity for selected configuration
        log.I("Checking new elements sequence order for configuration")
        f_ConfigElementsOrder = open("f_ConfigElementsOrder", "w")
        f_ConfigElementsOrder.write(out)
        f_ConfigElementsOrder.close()
        f_ConfigElementsOrder = open("f_ConfigElementsOrder", "r")
        element_name = f_ConfigElementsOrder.readline().strip('\n')
        assert element_name==self.elem_2_path, "ERROR : Error while modifying configuration %s elements order on domain %s" % (self.configuration)
        element_name = f_ConfigElementsOrder.readline().strip('\n')
        assert element_name==self.elem_0_path, "ERROR : Error while modifying configuration %s elements order on domain %s" % (self.configuration)
        element_name = f_ConfigElementsOrder.readline().strip('\n')
        assert element_name==self.elem_1_path, "ERROR : Error while modifying configuration %s elements order on domain %s" % (self.configuration)
        log.I("New elements sequence order conform to expected order for configuration %s" % (self.configuration))
        # Closing and removing temp file
        f_ConfigElementsOrder.close()
        os.remove("f_ConfigElementsOrder")
        # Removing created domain element
        out, err = self.pfw.sendCmd("removeElement", str(self.domain_name), str(self.elem_1_path))
        assert err == None, "ERROR : command [removeElement] - Error while removing domain element %s" % (self.elem_1_path)
        assert out == "Done", "ERROR : command [removeElement] - Error while removing domain element %s" % (self.elem_1_path)
        out, err = self.pfw.sendCmd("removeElement", str(self.domain_name), str(self.elem_2_path))
        assert err == None, "ERROR : command [removeElement] - Error while removing domain element %s" % (self.elem_2_path)
        assert out == "Done", "ERROR : command [removeElement] - Error while removing domain element %s" % (self.elem_2_path)

    def test_setElementSequence_errors(self):
        """
        Testing setElementSequence_errors
        ---------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - Setting an element not belonging to configuration
                - Setting undefined element in sequence order
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setElementSequence] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - all errors correctly detected
                - no impact on initial sequences order
        """
        log.D(self.test_setElementSequence_errors.__doc__)

        # Adding a new domain element
        log.I("Working on domain %s" % (self.domain_name))
        log.I("Adding a new domain element to domain %s" % (self.domain_name))
        out, err = self.pfw.sendCmd("addElement", str(self.domain_name), str(self.elem_1_path))
        assert err == None, "ERROR : command [addElement] - Error while adding new domain element %s" % (self.elem_1_path)
        assert out == "Done", "ERROR : command [addElement] - Error while adding new domain element %s" % (self.elem_1_path)
        log.I("New domain element %s added to domain %s" % (self.elem_1_path, self.domain_name))

        # Getting elements sequence from selected configuration
        log.I("Getting elements sequence from configuration %s" % (self.configuration))
        out, err = self.pfw.sendCmd("getElementSequence", self.domain_name, self.configuration)
        assert err == None, "ERROR : command [getElementSequence] - Error while listing elements sequence for configuration %s" % (self.configuration)
        log.I("Listing elements sequence for configuration %s correctly executed :\n%s" % (self.configuration, out))

        # Elements sequence backup
        f_ConfigElementsOrder_Backup = open("f_ConfigElementsOrder_Backup", "w")
        f_ConfigElementsOrder_Backup.write(out)
        f_ConfigElementsOrder_Backup.close()

        # Setting an element not belonging to configuration in sequence order
        log.I("Setting an element not belonging to configuration %s in sequence order" % (self.configuration))
        out, err = self.pfw.sendCmd("setElementSequence", self.domain_name, self.configuration, self.elem_2_path, self.elem_0_path, self.elem_1_path)
        assert err == None, "ERROR : command [setElementSequence] - Error while setting elements sequence for configuration %s" % (self.configuration)
        assert out != "Done", "ERROR : command [setElementSequence] - Error not detected when setting an element not belonging to configuration"

        # Setting undefined element in sequence order for selected configuration
        log.I("Setting undefined element in sequence order for configuration %s" % (self.configuration))
        out, err = self.pfw.sendCmd("setElementSequence", self.domain_name, self.configuration, "Wrong_Element_Name", self.elem_0_path, self.elem_1_path)
        assert err == None, "ERROR : command [setElementSequence] - Error while setting elements sequence for configuration %s" % (self.configuration)
        assert out != "Done", "ERROR : command [getElementSequence] - Error not detected when setting an undefined element to configuration"

        # Getting elements sequence from selected configuration for checking purpose
        out, err = self.pfw.sendCmd("getElementSequence", self.domain_name, self.configuration)
        assert err == None, "ERROR : command [getElementSequence] - Error while listing elements sequence for configuration %s" % (self.configuration)
        # Elements sequence backup
        f_ConfigElementsOrder = open("f_ConfigElementsOrder", "w")
        f_ConfigElementsOrder.write(out)
        f_ConfigElementsOrder.close()

        # Checking new elements sequence order conformity for selected configuration
        log.I("Checking new elements sequence order for configuration")
        f_ConfigElementsOrder = open("f_ConfigElementsOrder", "r")
        f_ConfigElementsOrder_Backup = open("f_ConfigElementsOrder_Backup", "r")
        new_element_name = f_ConfigElementsOrder.readline().strip('\n')
        element_name = f_ConfigElementsOrder_Backup.readline().strip('\n')
        assert element_name==new_element_name, "ERROR : setElementSequence errors have affected elements order on domain %s" % (self.configuration)
        new_element_name = f_ConfigElementsOrder.readline().strip('\n')
        element_name = f_ConfigElementsOrder_Backup.readline().strip('\n')
        assert element_name==new_element_name, "ERROR : setElementSequence errors have affected elements order on domain %s" % (self.configuration)
        log.I("Elements sequence order not affected by setElementSequence errors")

        # Closing and removing temp file
        f_ConfigElementsOrder.close()
        f_ConfigElementsOrder_Backup.close()
        os.remove("f_ConfigElementsOrder")
        os.remove("f_ConfigElementsOrder_Backup")
        # Removing created domain element
        out, err = self.pfw.sendCmd("removeElement", str(self.domain_name), str(self.elem_1_path))
        assert err == None, "ERROR : command [removeElement] - Error while removing domain element %s" % (self.elem_1_path)
        assert out == "Done", "ERROR : command [removeElement] - Error while removing domain element %s" % (self.elem_1_path)

    def test_getElementSequence_errors(self):
        """
        Testing getElementSequence_errors
        ---------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - Getting an element sequence on a wrong domain name
                - Getting an element sequence on a wrong configuration name
            Tested commands :
           ~~~~~~~~~~~~~~~~~
                - [getElementSequence] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - all errors correctly detected
                - no impact on initial sequences order
        """
        log.D(self.test_getElementSequence_errors.__doc__)

        # Adding new domain elements
        log.I("Adding a new domain element to domain %s" % (self.domain_name))
        out, err = self.pfw.sendCmd("addElement", str(self.domain_name), str(self.elem_1_path))
        assert err == None, "ERROR : command [addElement] - Error while adding new domain element %s" % (self.elem_1_path)
        assert out == "Done", "ERROR : command [addElement] - Error while adding new domain element %s" % (self.elem_1_path)
        log.I("Adding a new domain element to domain %s" % (self.domain_name))
        out, err = self.pfw.sendCmd("addElement", str(self.domain_name), str(self.elem_2_path))
        assert err == None, "ERROR : command [addElement] - Error while adding new domain element %s" % (self.elem_2_path)
        assert out == "Done", "ERROR : command [addElement] - Error while adding new domain element %s" % (self.elem_2_path)
        log.I("New domain elements %s and %s added to domain %s" % (self.elem_1_path, self.elem_2_path, self.domain_name))

        # Getting elements sequence from selected configuration
        log.I("Getting elements sequence from configuration %s" % (self.configuration))
        out, err = self.pfw.sendCmd("getElementSequence", self.domain_name, self.configuration)
        assert err == None, "ERROR : command [getElementSequence] - Error while listing elements sequence for configuration %s" % (self.configuration)
        log.I("Listing elements sequence for configuration %s correctly executed :\n%s" % (self.configuration, out))

        # Elements sequence backup
        f_ConfigElementsOrder_Backup = open("f_ConfigElementsOrder_Backup", "w")
        f_ConfigElementsOrder_Backup.write(out)
        f_ConfigElementsOrder_Backup.close()

        # Getting an element sequence on a wrong domain name
        log.I("Getting an element sequence on a wrong domain name")
        out, err = self.pfw.sendCmd("getElementSequence", "Wrong_Domain_Name", self.configuration)
        assert err == None, "ERROR : command [getElementSequence] - Error when getting elements sequence for configuration %s" % (self.configuration)
        assert out != "Done", "ERROR : command [getElementSequence] - Error not detected when getting elements sequence for a wrong domain name"

        # Getting an element sequence on a wrong configuration name
        log.I("Getting an element sequence on a wrong configuration name")
        out, err = self.pfw.sendCmd("getElementSequence", self.domain_name, "Wrong_Configuration_Name")
        assert err == None, "ERROR : command [getElementSequence] - Error when getting elements sequence on a wrong configuration name"
        assert out != "Done", "ERROR : command [getElementSequence] - Error not detected when getting elements sequence on a wrong configuration name"

        # Getting elements sequence from selected configuration for checking purpose
        out, err = self.pfw.sendCmd("getElementSequence", self.domain_name, self.configuration)
        assert err == None, "ERROR : command [getElementSequence] - Error while listing elements sequence for configuration %s" % (self.configuration)
        # Elements sequence backup
        f_ConfigElementsOrder = open("f_ConfigElementsOrder", "w")
        f_ConfigElementsOrder.write(out)
        f_ConfigElementsOrder.close()

        # Checking new elements sequence order conformity for selected configuration
        log.I("Checking new elements sequence order for configuration")
        f_ConfigElementsOrder = open("f_ConfigElementsOrder", "r")
        f_ConfigElementsOrder_Backup = open("f_ConfigElementsOrder_Backup", "r")
        new_element_name = f_ConfigElementsOrder.readline().strip('\n')
        element_name = f_ConfigElementsOrder_Backup.readline().strip('\n')
        assert element_name==new_element_name, "ERROR : getElementSequence errors have affected elements order on domain %s" % (self.configuration)
        new_element_name = f_ConfigElementsOrder.readline().strip('\n')
        element_name = f_ConfigElementsOrder_Backup.readline().strip('\n')
        assert element_name==new_element_name, "ERROR : getElementSequence errors have affected elements order on domain %s" % (self.configuration)
        log.I("Elements sequence order not affected by getElementSequence errors")

        # Closing and removing temp file
        f_ConfigElementsOrder.close()
        f_ConfigElementsOrder_Backup.close()
        os.remove("f_ConfigElementsOrder")
        os.remove("f_ConfigElementsOrder_Backup")
        # Removing created domain element
        out, err = self.pfw.sendCmd("removeElement", str(self.domain_name), str(self.elem_1_path))
        assert err == None, "ERROR : command [removeElement] - Error while removing domain element %s" % (self.elem_1_path)
        assert out == "Done", "ERROR : command [removeElement] - Error while removing domain element %s" % (self.elem_1_path)
        out, err = self.pfw.sendCmd("removeElement", str(self.domain_name), str(self.elem_2_path))
        assert err == None, "ERROR : command [removeElement] - Error while removing domain element %s" % (self.elem_2_path)
        assert out == "Done", "ERROR : command [removeElement] - Error while removing domain element %s" % (self.elem_2_path)
