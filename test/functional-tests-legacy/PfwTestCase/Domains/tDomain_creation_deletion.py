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
    - [createDomain]  function
    - [deleteDomain] function

Test cases :
------------
    - Testing nominal cases
    - Testing domain creation error
    - Testing domain deletion error
"""
import os
from Util.PfwUnitTestLib import PfwTestCase
from Util import ACTLogging
log=ACTLogging.Logger()

# Test of Domains - Basic operations (creations/deletions)
class TestCases(PfwTestCase):
    def setUp(self):
        self.pfw.sendCmd("setTuningMode", "on")
        self.new_domains_number = 4
        self.new_domain_name = "Domain"

    def tearDown(self):
        self.pfw.sendCmd("setTuningMode", "off")

    def test_Domain_Creation_Error(self):
        """
        Testing domain creation error
        -----------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - Create an already existent domain
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [createDomain] function
                - [listDomains] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - Error detected when creating an already existent domain
                - No domains list update
        """
        log.D(self.test_Domain_Creation_Error.__doc__)
        # New domain creation
        log.I("New domain creation")
        log.I("command [createDomain]")
        domain_name = 'Test_Domain'
        out, err = self.pfw.sendCmd("createDomain",domain_name, "")
        assert out == "Done", out
        assert err == None, "ERROR : command [createDomain] - Error while creating domain %s" % (domain_name)
        log.I("command [createDomain] correctly executed")

        # Domains listing using "listDomains" command
        log.I("Current domains listing")
        log.I("command [listDomains]")
        out, err = self.pfw.sendCmd("listDomains","","")
        assert err == None, "ERROR : command [listDomains] - Error while listing domains"
        log.I("command [listDomains] - correctly executed")

        # Domains listing backup
        f_Domains_Backup = open("f_Domains_Backup", "w")
        f_Domains_Backup.write(out)
        f_Domains_Backup.close()
        f_Domains_Backup = open("f_Domains_Backup", "r")
        domains_nbr_init = 0
        line=f_Domains_Backup.readline()
        while line!="":
            line=f_Domains_Backup.readline()
            domains_nbr_init+=1
        f_Domains_Backup.close()
        log.I("Actual domains number : %s" % domains_nbr_init)

        # Trying to add an existent domain name
        log.I("Adding an already existent domain name")
        log.I("command [createDomain]")
        domain_name = 'Test_Domain'
        out, err = self.pfw.sendCmd("createDomain",domain_name, "")
        assert out != "Done", "ERROR : command [createDomain] - Error not detected when creating an already existent domain"
        assert err == None, err
        log.I("command [createDomain] - error correctly detected")

        # Checking domains list integrity
        log.I("Checking domains listing integrity after domain creation error")
        ## Domains listing using "listDomains" command
        out, err = self.pfw.sendCmd("listDomains","","")
        assert err == None, "ERROR : command [listDomains] - Error while listing domains"
        f_Domains = open("f_Domains", "w")
        f_Domains.write(out)
        f_Domains.close()
        ## Domains listing integrity check
        f_Domains = open("f_Domains", "r")
        domains_nbr = 0
        line=f_Domains.readline()
        while line!="":
            line=f_Domains.readline()
            domains_nbr+=1
        f_Domains.close()
        assert domains_nbr == domains_nbr_init, "ERROR : Domains number error, expected %s, found %s" % (domains_nbr_init,domains_nbr)
        log.I("Test OK - Domains number not updated")
        f_Domains = open("f_Domains", "r")
        f_Domains_Backup = open("f_Domains_Backup", "r")
        for line in range(domains_nbr):
            domain_backup_name = f_Domains_Backup.readline().strip('\n'),
            domain_name = f_Domains.readline().strip('\n'),
            assert domain_backup_name==domain_name, "ERROR : Error while reading domain %s" % (domain_backup_name)
        log.I("Test OK - Domains listing not affected by domain creation error")

        # Closing and deleting temp files
        f_Domains_Backup.close()
        f_Domains.close()
        os.remove("f_Domains_Backup")
        os.remove("f_Domains")

    def test_Domain_Deletion_Error(self):
        """
        Testing domain deletion error
        -----------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - Delete a non existent domain
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [deleteDomain] function
                - [listDomains] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - Error detected when deleting a non-existent domain
                - No domains list update
        """
        log.D(self.test_Domain_Deletion_Error.__doc__)
        # Domains listing using "listDomains" command
        log.I("Current domains listing")
        log.I("command [listDomains]")
        out, err = self.pfw.sendCmd("listDomains","","")
        assert err == None, "ERROR : command [listDomains] - Error while listing domains"
        log.I("command [listDomains] correctly executed")

        # Domains listing backup
        f_Domains_Backup = open("f_Domains_Backup", "w")
        f_Domains_Backup.write(out)
        f_Domains_Backup.close()
        f_Domains_Backup = open("f_Domains_Backup", "r")
        domains_nbr_init = 0
        line=f_Domains_Backup.readline()
        while line!="":
            line=f_Domains_Backup.readline()
            domains_nbr_init+=1
        f_Domains_Backup.close()
        log.I("Actual domains number : %s" % domains_nbr_init)

        # Trying to delete a non-existent domain name
        log.I("Deleting a non-existent domain name")
        log.I("command [deleteDomain]")
        domain_name = 'Wrong_Domain_Name'
        out, err = self.pfw.sendCmd("deleteDomain",domain_name, "")
        assert out != "Done", "ERROR : command [deleteDomain] - Error not detected when deleting a non-existent domain"
        assert err == None, err
        log.I("command [deleteDomain] - error correctly detected")

        # Checking domains list integrity
        log.I("Checking domains listing integrity after domain deletion error")
        ## Domains listing using "listDomains" command
        out, err = self.pfw.sendCmd("listDomains","","")
        assert err == None, "ERROR : command [listDomains] - Error while listing domains"
        f_Domains = open("f_Domains", "w")
        f_Domains.write(out)
        f_Domains.close()
        ## Domains listing integrity check
        f_Domains = open("f_Domains", "r")
        domains_nbr = 0
        line=f_Domains.readline()
        while line!="":
            line=f_Domains.readline()
            domains_nbr+=1
        f_Domains.close()
        assert domains_nbr == domains_nbr_init, "ERROR : Domains number error, expected %s, found %s" % (domains_nbr_init,domains_nbr)
        log.I("Test OK - Domains number not updated")
        f_Domains = open("f_Domains", "r")
        f_Domains_Backup = open("f_Domains_Backup", "r")
        for line in range(domains_nbr):
            domain_backup_name = f_Domains_Backup.readline().strip('\n'),
            domain_name = f_Domains.readline().strip('\n'),
            assert domain_backup_name==domain_name, "Error while reading domain %s" % (domain_backup_name)
        log.I("Test OK - Domains listing not affected by domain deletion error")

        # Closing and deleting temp files
        f_Domains_Backup.close()
        f_Domains.close()
        os.remove("f_Domains_Backup")
        os.remove("f_Domains")

    def test_Nominal_Case(self):
        """
        Testing nominal cases
        ---------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - Create X new domains
                - Delete X domains
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [createDomain] function
                - [deleteDomain] function
                - [listDomains] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - X new domains created
                - X domains deleted
        """
        log.D(self.test_Nominal_Case.__doc__)
        # Initial domains listing using "listDomains" command
        log.I("Initial domains listing")
        log.I("command [listDomains]")
        out, err = self.pfw.sendCmd("listDomains","","")
        assert err == None, "ERROR : command [listDomains] - Error while listing domains"
        log.I("command [listDomains] correctly executed")

        # Initial domains number count
        f_init_domains = open("f_init_domains", "w")
        f_init_domains.write(out)
        f_init_domains.close()
        init_domains_nbr = 0
        f_init_domains = open("f_init_domains", "r")
        line=f_init_domains.readline()
        while line!="":
            line=f_init_domains.readline()
            init_domains_nbr+=1
        f_init_domains.close()
        log.I("Initial domains number : %s" % (init_domains_nbr))

        # New domains creation
        log.I("New domains creation")
        log.I("PFW command : [createDomain]")
        for index in range (self.new_domains_number):
            domain_name = "".join([self.new_domain_name, "_", str(index+init_domains_nbr)])
            out, err = self.pfw.sendCmd("createDomain",domain_name, "")
            assert out == "Done", "ERROR : %s" % (out)
            assert err == None, "ERROR : command [createDomain] - Error while creating domain %s" % (domain_name)
        log.I("command [createDomain] correctly executed")

        # New domain creation check
        log.I("New domains creation check :")
        log.I("command [listDomains]")
        out, err = self.pfw.sendCmd("listDomains","","")
        assert err == None, "ERROR : command [listDomains] - Error while listing new domains"
        log.I("command [listDomains] correctly executed")

        # Working on a temporary files to record domains listing
        tempfile = open("tempfile", "w")
        tempfile.write(out)
        tempfile.close()

        # Checking last added entries in the listing
        tempfile = open("tempfile", "r")
        domains_nbr = 0
        line=tempfile.readline()
        while line!="":
            line=tempfile.readline()
            domains_nbr+=1
        tempfile.close()
        log.I("New domains conformity check")
        tempfile = open("tempfile", "r")
        for line in range(domains_nbr):
            if (line >= (domains_nbr - self.new_domains_number)):
                domain_name = "".join([self.new_domain_name,"_",str(line)]),
                domain_created = tempfile.readline().strip('\n'),
                assert domain_name==domain_created, "ERROR : Error while creating domain %s %s" % (domain_created, domain_name)
            else:
                domain_created = tempfile.readline()
        log.I("New domains conform to expected values")
        created_domains_number = domains_nbr - init_domains_nbr
        log.I("%s new domains created" % created_domains_number)
        tempfile.close()
        os.remove("tempfile")

        # New domains deletion
        log.I("New domains deletion")
        log.I("command [deleteDomain]")
        for index in range (self.new_domains_number):
            domain_name = "".join([self.new_domain_name, "_", str(index+init_domains_nbr)])
            out, err = self.pfw.sendCmd("deleteDomain",domain_name, "")
            assert out == "Done", "ERROR : %s" % (out)
            assert err == None, "ERROR : command [deleteDomain] - Error while deleting domain %s" % (domain_name)
        log.I("command [deleteDomain] correctly executed")

        # New domains deletion check
        f_init_domains = open("f_init_domains", "r")
        tempfile = open("tempfile", "w")
        log.I("New domains deletion check :")
        log.I("command [listDomains]")
        out, err = self.pfw.sendCmd("listDomains","","")
        assert err == None, "ERROR : command [listDomains] - Error while listing domains"
        log.I("command [listDomains] correctly executed")
        tempfile.write(out)
        tempfile.close()
        tempfile = open("tempfile", "r")
        line=tempfile.readline()
        line_init=f_init_domains.readline()
        while line!="":
            line=tempfile.readline()
            line_init=f_init_domains.readline()
            assert line == line_init, "ERROR : Domain deletion error"
        if line=="":
            assert line_init == "", "ERROR : Wrong domains deletion number"
            log.I("Deletion completed - %s domains deleted" % created_domains_number)

        # Temporary files deletion
        tempfile.close()
        f_init_domains.close()
        os.remove("tempfile")
        os.remove("f_init_domains")
