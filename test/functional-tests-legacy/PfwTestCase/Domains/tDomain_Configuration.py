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
Creation, renaming and deletion configuration testcases

List of tested functions :
--------------------------
    - [listConfigurations]  function
    - [createConfiguration] function
    - [deleteConfiguration] function
    - [renameConfiguration] function

Test cases :
------------
    - Testing configuration creation error
    - Testing configuration renaming error
    - Testing configuration deletion error
    - Testing nominal case
"""
import os
from Util.PfwUnitTestLib import PfwTestCase
from Util import ACTLogging
log=ACTLogging.Logger()


# Test of Domains - Rename
class TestCases(PfwTestCase):
    def setUp(self):
        self.pfw.sendCmd("setTuningMode", "on")
        self.domain_name = "domain_test"
        self.conf_test = "conf_white"
        self.conf_test_renamed = "conf_black"
        self.new_conf_number = 5

    def tearDown(self):
        self.pfw.sendCmd("setTuningMode", "off")

    def test_Conf_Creation_Error(self):
        """
        Testing configuration creation error
        ------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - Create an already existent configuration
                - Create a configuration with no name specified
                - Create a configuration on a wrong domain name
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [createConfiguration] function
                - [createDomain] function
                - [listConfigurations] function
                - [deleteConfiguration] function
                - [deleteDomain] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - no configuration created
                - existent configurations not affected by error
        """
        log.D(self.test_Conf_Creation_Error.__doc__)
        # New domain creation for testing purpose
        log.I("New domain creation for testing purpose : %s" % (self.domain_name))
        log.I("command [createDomain]")
        out, err = self.pfw.sendCmd("createDomain",self.domain_name, "")
        assert out == "Done", out
        assert err == None, "ERROR : command [createDomain] - Error while creating domain %s" % (self.domain_name)
        log.I("command [createDomain] correctly executed")
        log.I("Domain %s created" % (self.domain_name))

        # New configurations creation for testing purpose
        for iteration in range (self.new_conf_number):
            new_conf_name = "".join([self.conf_test, "_", str(iteration)])
            log.I("New configuration %s creation for domain %s" % (new_conf_name,self.domain_name))
            log.I("command [createConfiguration]")
            out, err = self.pfw.sendCmd("createConfiguration",self.domain_name,new_conf_name)
            assert out == "Done", out
            assert err == None, "ERROR : command [createConfiguration] - Error while creating configuration %s" % (new_conf_name)
            log.I("command [createConfiguration] correctly executed")
            log.I("Configuration %s created for domain %s" % (new_conf_name,self.domain_name))

        # Domain configurations listing backup
        log.I("Configurations listing for domain %s" % (self.domain_name))
        log.I("command [listConfigurations]")
        out, err = self.pfw.sendCmd("listConfigurations",self.domain_name, "")
        assert err == None, "ERROR : command [listConfigurations] - Error while listing configurations for domain %s" % (self.domain_name)
        log.I("command [listConfigurations] correctly executed")
        # Saving configurations names
        f_configurations_backup = open("f_configurations_backup", "w")
        f_configurations_backup.write(out)
        f_configurations_backup.close()

        # New configurations creation error
        log.I("Creating an already existent configurations names")
        for iteration in range (self.new_conf_number):
            new_conf_name = "".join([self.conf_test, "_", str(iteration)])
            log.I("Trying to create already existent %s configuration for domain %s" % (new_conf_name,self.domain_name))
            log.I("command [createConfiguration]")
            out, err = self.pfw.sendCmd("createConfiguration",self.domain_name,new_conf_name)
            assert out != "Done", "ERROR : command [createConfiguration] - Error not detected while creating already existent configuration %s" % (new_conf_name)
            assert err == None, "ERROR : command [createConfiguration] - Error while creating configuration %s" % (new_conf_name)
            log.I("command [createConfiguration] correctly executed")
            log.I("error correctly detected, no configuration created")
        log.I("Creating a configuration without specifying a name")
        out, err = self.pfw.sendCmd("createConfiguration",self.domain_name)
        assert out != "Done", "ERROR : command [createConfiguration] - Error not detected while creating a configuration without specifying a name"
        assert err == None, "ERROR : command [createConfiguration] - Error while creating configuration"
        log.I("error correctly detected")
        log.I("Creating a configuration on a wrong domain name")
        new_conf_name = "new_conf"
        out, err = self.pfw.sendCmd("createConfiguration","wrong_domain_name",new_conf_name)
        assert out != "Done", "ERROR : command [createConfiguration] - Error not detected while creating a configuration on a wrong domain name"
        assert err == None, "ERROR : command [createConfiguration] - Error while creating configuration"
        log.I("error correctly detected")

        # New domain configurations listing
        log.I("Configurations listing for domain %s" % (self.domain_name))
        log.I("command [listConfigurations]" )
        out, err = self.pfw.sendCmd("listConfigurations",self.domain_name, "")
        assert err == None, "ERROR : command [listConfigurations] - Error while listing configurations for domain %s" % (self.domain_name)
        log.I("command [listConfigurations] correctly executed")
        # Saving configurations names
        f_configurations = open("f_configurations", "w")
        f_configurations.write(out)
        f_configurations.close()

        # Checking configurations names integrity
        log.I("Configurations listing conformity check")
        f_configurations = open("f_configurations", "r")
        f_configurations_backup = open("f_configurations_backup", "r")
        for iteration in range(self.new_conf_number):
            listed_conf_backup = f_configurations_backup.readline().strip('\n')
            listed_conf = f_configurations.readline().strip('\n')
            assert listed_conf==listed_conf_backup, "ERROR : Error while listing configuration %s (found %s)" % (listed_conf_backup, listed_conf)
        log.I("No change detected, listed configurations names conform to expected values")

        # New domain deletion
        log.I("End of test, new domain deletion")
        log.I("command [deleteDomain]")
        out, err = self.pfw.sendCmd("deleteDomain",self.domain_name, "")
        assert out == "Done", "ERROR : %s" % (out)
        assert err == None, "ERROR : command [deleteDomain] - Error while deleting domain %s" % (self.domain_name)
        log.I("command [deleteDomain] correctly executed")

        # Closing and deleting temp files
        f_configurations_backup.close()
        os.remove("f_configurations_backup")
        f_configurations.close()
        os.remove("f_configurations")

    def test_Conf_Renaming_Error(self):
        """
        Testing configuration renaming error
        ------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - Rename a configuration with an already used name
                - Rename a configuration with no name specified
                - Rename a configuration on a wrong domain name
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [renameConfiguration] function
                - [createDomain] function
                - [listConfigurations] function
                - [createConfiguration] function
                - [deleteConfiguration] function
                - [deleteDomain] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - error detected
                - no configuration created
                - existent configurations not affected by error
        """
        log.D(self.test_Conf_Renaming_Error.__doc__)
        # New domain creation for testing purpose
        log.I("New domain creation for testing purpose : %s" % (self.domain_name))
        log.I("command [createDomain]")
        out, err = self.pfw.sendCmd("createDomain",self.domain_name, "")
        assert out == "Done", out
        assert err == None, "ERROR : command [createDomain] - Error while creating domain %s" % (self.domain_name)
        log.I("command [createDomain] correctly executed")
        log.I("Domain %s created" % (self.domain_name))

        # New configurations creation for testing purpose
        for iteration in range (self.new_conf_number):
            new_conf_name = "".join([self.conf_test, "_", str(iteration)])
            log.I("New configuration %s creation for domain %s" % (new_conf_name,self.domain_name))
            log.I("command [createConfiguration]")
            out, err = self.pfw.sendCmd("createConfiguration",self.domain_name,new_conf_name)
            assert out == "Done", out
            assert err == None, "ERROR : command [createConfiguration] - Error while creating configuration %s" % (new_conf_name)
            log.I("command [createConfiguration] correctly executed")
            log.I("Configuration %s created for domain %s" % (new_conf_name,self.domain_name))

        # Domain configurations listing backup
        log.I("Configurations listing for domain %s" % (self.domain_name))
        log.I("command [listConfigurations]")
        out, err = self.pfw.sendCmd("listConfigurations",self.domain_name, "")
        assert err == None, "ERROR : command [listConfigurations] - Error while listing configurations for domain %s" % (self.domain_name)
        log.I("command [listConfigurations] correctly executed")
        # Saving configurations names
        f_configurations_backup = open("f_configurations_backup", "w")
        f_configurations_backup.write(out)
        f_configurations_backup.close()

        # New configurations renaming error
        log.I("renaming a configuration with an already used name")
        for iteration in range (self.new_conf_number-1):
            conf_name = "".join([self.conf_test, "_", str(iteration)])
            new_conf_name = "".join([self.conf_test, "_", str(iteration+1)])
            log.I("Trying to rename %s on domain %s with an already used name : %s" % (conf_name,self.domain_name,new_conf_name))
            log.I("command [renameConfiguration]" )
            out, err = self.pfw.sendCmd("renameConfiguration",self.domain_name,conf_name,new_conf_name)
            assert out != "Done", "ERROR : command [renameConfiguration] - Error not detected while renaming configuration %s with an already used name" % (new_conf_name)
            assert err == None, "ERROR : command [renameConfiguration] - Error while renaming configuration %s" % (new_conf_name)
            log.I("command [renameConfiguration] correctly executed")
            log.I("error correctly detected, no configuration renamed")
        log.I("renaming a configuration without specifying a new name")
        out, err = self.pfw.sendCmd("renameConfiguration",self.domain_name,new_conf_name)
        assert out != "Done", "ERROR : command [renameConfiguration] - Error not detected while renaming a configuration without specifying a new name"
        assert err == None, "ERROR : command [renameConfiguration] - Error while renaming configuration"
        log.I("error correctly detected, no configuration renamed")
        log.I("renaming a configuration on a wrong domain name")
        new_conf_name = "new_conf"
        out, err = self.pfw.sendCmd("renameConfiguration","wrong_domain_name",new_conf_name,"Configuration")
        assert out != "Done", "ERROR : command [renameConfiguration] - Error not detected while renaming a configuration on a wrong domain name"
        assert err == None, "ERROR : command [renameConfiguration] - Error while renaming configuration"
        log.I("error correctly detected, no configuration renamed")

        # New domain configurations listing
        log.I("Configurations listing for domain %s" % (self.domain_name))
        log.I("command [listConfigurations]")
        out, err = self.pfw.sendCmd("listConfigurations",self.domain_name, "")
        assert err == None, "ERROR : command [listConfigurations] - Error while listing configurations for domain %s" % (self.domain_name)
        log.I("command [listConfigurations] correctly executed")
        # Saving configurations names
        f_configurations = open("f_configurations", "w")
        f_configurations.write(out)
        f_configurations.close()

        # Checking configurations names integrity
        log.I("Configurations listing conformity check")
        f_configurations = open("f_configurations", "r")
        f_configurations_backup = open("f_configurations_backup", "r")
        for iteration in range(self.new_conf_number):
            listed_conf_backup = f_configurations_backup.readline().strip('\n')
            listed_conf = f_configurations.readline().strip('\n')
            assert listed_conf==listed_conf_backup, "ERROR : Error while listing configuration %s (found %s)" % (listed_conf_backup, listed_conf)
        log.I("No change detected, listed configurations names conform to expected values")

        # Testing domain deletion
        log.I("End of test, new domain deletion")
        log.I("command [deleteDomain]")
        out, err = self.pfw.sendCmd("deleteDomain",self.domain_name, "")
        assert out == "Done", "ERROR : %s" % (out)
        assert err == None, "ERROR : command [deleteDomain] - Error while deleting domain %s" % (self.domain_name)
        log.I("command [deleteDomain] correctly executed")

        # Closing and deleting temp files
        f_configurations_backup.close()
        os.remove("f_configurations_backup")
        f_configurations.close()
        os.remove("f_configurations")

    def test_Conf_Deletion_Error(self):
        """
        Testing configuration deletion error
        ------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - Delete a configuration with a non existent name
                - Delete a configuration with no name specified
                - Delete a configuration on a wrong domain name
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [deleteConfiguration] function
                - [createDomain] function
                - [listConfigurations] function
                - [createConfiguration] function
                - [deleteDomain] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - error detected
                - no configuration created
                - existent configurations not affected by error
        """
        print self.test_Conf_Renaming_Error.__doc__
        # New domain creation for testing purpose
        log.I("New domain creation for testing purpose : %s" % (self.domain_name))
        log.I("command [createDomain]")
        out, err = self.pfw.sendCmd("createDomain",self.domain_name, "")
        assert out == "Done", out
        assert err == None, "ERROR : command [createDomain] - Error while creating domain %s" % (self.domain_name)
        log.I("command [createDomain] correctly executed")
        log.I("Domain %s created" % (self.domain_name))

        # New configurations creation for testing purpose
        for iteration in range (self.new_conf_number):
            new_conf_name = "".join([self.conf_test, "_", str(iteration)])
            log.I("New configuration %s creation for domain %s" % (new_conf_name,self.domain_name))
            log.I("command [createConfiguration]")
            out, err = self.pfw.sendCmd("createConfiguration",self.domain_name,new_conf_name)
            assert out == "Done", out
            assert err == None, "ERROR : command [createConfiguration] - Error while creating configuration %s" % (new_conf_name)
            log.I("command [createConfiguration] correctly executed")
            log.I("Configuration %s created for domain %s" % (new_conf_name,self.domain_name))

        # Domain configurations listing backup
        log.I("Configurations listing for domain %s" % (self.domain_name))
        log.I("command [listConfigurations]")
        out, err = self.pfw.sendCmd("listConfigurations",self.domain_name, "")
        assert err == None, "ERROR : command [listConfigurations] - Error while listing configurations for domain %s" % (self.domain_name)
        log.I("command [listConfigurations] correctly executed")
        # Saving configurations names
        f_configurations_backup = open("f_configurations_backup", "w")
        f_configurations_backup.write(out)
        f_configurations_backup.close()

        # Configurations deletion errors
        log.I("Trying various deletions error test cases")
        log.I("Trying to delete a wrong configuration name on domain %s" % (self.domain_name))
        log.I("command [deleteConfiguration]")
        out, err = self.pfw.sendCmd("deleteConfiguration",self.domain_name,"wrong_configuration_name")
        assert out != "Done", "ERROR : command [deleteConfiguration] - Error not detected while deleting non existent configuration name"
        assert err == None, "ERROR : command [deleteConfiguration] - Error while deleting configuration"
        log.I("command [deleteConfiguration] correctly executed")
        log.I("error correctly detected, no configuration deleted")
        log.I("deleting a configuration with no name specified")
        out, err = self.pfw.sendCmd("deleteConfiguration",self.domain_name)
        assert out != "Done", "ERROR : command [deleteConfiguration] - Error not detected while deleting a configuration without specifying a name"
        assert err == None, "ERROR : command [deleteConfiguration] - Error while deleting configuration"
        log.I("error correctly detected, no configuration deleted")
        log.I("deleting a configuration on a wrong domain name")
        out, err = self.pfw.sendCmd("deleteConfiguration","wrong_domain_name",new_conf_name)
        assert out != "Done", "ERROR : command [deleteConfiguration] - Error not detected while deleting a configuration on a wrong domain name"
        assert err == None, "ERROR : command [deleteConfiguration] - Error while deleting configuration"
        log.I("error correctly detected, no configuration deleted")

        # New domain configurations listing
        log.I("Configurations listing for domain %s" % (self.domain_name))
        log.I("command [listConfigurations]")
        out, err = self.pfw.sendCmd("listConfigurations",self.domain_name, "")
        assert err == None, "ERROR : command [listConfigurations] - Error while listing configurations for domain %s" % (self.domain_name)
        log.I("command [listConfigurations] correctly executed")
        # Saving configurations names
        f_configurations = open("f_configurations", "w")
        f_configurations.write(out)
        f_configurations.close()

        # Checking configurations names integrity
        log.I("Configurations listing conformity check")
        f_configurations = open("f_configurations", "r")
        f_configurations_backup = open("f_configurations_backup", "r")
        for iteration in range(self.new_conf_number):
            listed_conf_backup = f_configurations_backup.readline().strip('\n')
            listed_conf = f_configurations.readline().strip('\n')
            assert listed_conf==listed_conf_backup, "ERROR : Error while listing configuration %s (found %s)" % (listed_conf_backup, listed_conf)
        log.I("No change detected, listed configurations names conform to expected values")

        # Testing domain deletion
        log.I("End of test, new domain deletion")
        log.I("command [deleteDomain]")
        out, err = self.pfw.sendCmd("deleteDomain",self.domain_name, "")
        assert out == "Done", "ERROR : %s" % (out)
        assert err == None, "ERROR : command [deleteDomain] - Error while deleting domain %s" % (self.domain_name)
        log.I("command [deleteDomain] correctly executed")

        # Closing and deleting temp files
        f_configurations_backup.close()
        os.remove("f_configurations_backup")
        f_configurations.close()
        os.remove("f_configurations")

    def test_Nominal_Case(self):
        """
        Testing nominal cases
        ---------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - Create new configurations
                - List domain configurations
                - Rename configurations
                - Delete configurations
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [listConfigurations] function
                - [createConfiguration] function
                - [renameConfiguration] function
                - [deleteConfiguration] function
                - [createDomain] function
                - [deleteDomain] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - all operations succeed
        """
        log.D(self.test_Nominal_Case.__doc__)
        # New domain creation
        log.I("New domain creation for testing purpose : %s" % (self.domain_name))
        log.I("command [createDomain]")
        out, err = self.pfw.sendCmd("createDomain",self.domain_name, "")
        assert out == "Done", out
        assert err == None, "ERROR : command [createDomain] - Error while creating domain %s" % (self.domain_name)
        log.I("command [createDomain] correctly executed")
        log.I("Domain %s created" % (self.domain_name))

        # New configurations creation
        for iteration in range (self.new_conf_number):
            new_conf_name = "".join([self.conf_test, "_", str(iteration)])
            log.I("New configuration %s creation for domain %s" % (new_conf_name,self.domain_name))
            log.I("command [createConfiguration]" )
            out, err = self.pfw.sendCmd("createConfiguration",self.domain_name,new_conf_name)
            assert out == "Done", out
            assert err == None, "ERROR : command [createConfiguration] - Error while creating configuration %s" % (new_conf_name)
            log.I("command [createConfiguration] correctly executed")
            log.I("Configuration %s created for domain %s" % (new_conf_name,self.domain_name))

        # Listing domain configurations
        log.I("Configurations listing for domain %s" % (self.domain_name))
        log.I("command [listConfigurations]")
        out, err = self.pfw.sendCmd("listConfigurations",self.domain_name, "")
        assert err == None, "ERROR : command [listConfigurations] - Error while listing configurations for domain %s" % (self.domain_name)
        log.I("command [listConfigurations] correctly executed")
        # Saving configurations names
        f_configurations = open("f_configurations", "w")
        f_configurations.write(out)
        f_configurations.close()
        # Checking configurations names integrity
        log.I("Configurations listing conformity check")
        f_configurations = open("f_configurations", "r")
        for iteration in range(self.new_conf_number):
            new_conf_name = "".join([self.conf_test, "_", str(iteration)])
            listed_conf = f_configurations.readline().strip('\n')
            assert listed_conf==new_conf_name, "ERROR : Error while listing configuration %s (found %s)" % (listed_conf, new_conf_name)
        log.I("Listed configurations names conform to expected values")

        # Configuration renaming
        log.I("Configurations renaming")
        for iteration in range (self.new_conf_number):
            conf_name = "".join([self.conf_test, "_", str(iteration)])
            new_conf_name = "".join([self.conf_test_renamed, "_", str(iteration)])
            log.I("Configuration %s renamed to %s in domain %s" % (conf_name,new_conf_name,self.domain_name))
            log.I("command [renameConfiguration]")
            out, err = self.pfw.sendCmd("renameConfiguration",self.domain_name,conf_name,new_conf_name)
            assert out == "Done", out
            assert err == None, "ERROR : command [renameConfiguration] - Error while renaming configuration %s to %s" % (conf_name,new_conf_name)
            log.I("command [renameConfiguration] correctly executed")
            log.I("Configuration %s renamed to %s for domain %s" % (conf_name,new_conf_name,self.domain_name))
        # Listing domain configurations
        log.I("Configurations listing to check configurations renaming")
        log.I("command [listConfigurations]")
        out, err = self.pfw.sendCmd("listConfigurations",self.domain_name, "")
        assert err == None, "ERROR : command [listConfigurations] - Error while listing configurations for domain %s" % (self.domain_name)
        log.I("command [listConfigurations] correctly executed")
        # Saving configurations names
        f_configurations_renamed = open("f_configurations_renamed", "w")
        f_configurations_renamed.write(out)
        f_configurations_renamed.close()
        # Checking configurations names integrity
        log.I("Configurations listing conformity check")
        f_configurations_renamed = open("f_configurations_renamed", "r")
        for iteration in range(self.new_conf_number):
            new_conf_name = "".join([self.conf_test_renamed, "_", str(iteration)])
            listed_conf = f_configurations_renamed.readline().strip('\n')
            assert listed_conf==new_conf_name, "ERROR : Error while renaming configuration %s (found %s)" % (new_conf_name,listed_conf)
        log.I("Listed configurations names conform to expected values, renaming successfull")

        # New domain deletion
        log.I("End of test, new domain deletion")
        log.I("command [deleteDomain]")
        out, err = self.pfw.sendCmd("deleteDomain",self.domain_name, "")
        assert out == "Done", "ERROR : %s" % (out)
        assert err == None, "ERROR : command [deleteDomain] - Error while deleting domain %s" % (self.domain_name)
        log.I("command [deleteDomain] correctly executed")

        # Closing and deleting temp file
        f_configurations.close()
        os.remove("f_configurations")
        f_configurations_renamed.close()
        os.remove("f_configurations_renamed")
