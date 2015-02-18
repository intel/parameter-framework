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
Renaming domains testcases

List of tested functions :
--------------------------
    - [renameDomain]  function

Test cases :
------------
    - Nominal cases
    - Renaming errors
    - Special cases
"""
import os
from Util.PfwUnitTestLib import PfwTestCase
from Util import ACTLogging
log=ACTLogging.Logger()

# Test of Domains - Rename
class TestCases(PfwTestCase):
    def setUp(self):
        self.pfw.sendCmd("setTuningMode", "on")
        self.domain_name = "domain_white"
        self.new_domain_name = "domain_black"
        self.renaming_iterations = 5

    def tearDown(self):
        self.pfw.sendCmd("setTuningMode", "off")

    def test_Nominal_Case(self):
        """
        Nominal case
        ------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - Renaming a domain
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [renameDomain] function
                - [createDomain] function
                - [listDomains] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - domains correctly renamed
        """
        log.D(self.test_Nominal_Case.__doc__)
        # New domain creation
        log.I("New domain creation : %s" % (self.domain_name))
        log.I("command [createDomain]" )
        out, err = self.pfw.sendCmd("createDomain",self.domain_name, "")
        assert out == "Done", out
        assert err == None, "ERROR : command [createDomain] - ERROR while creating domain %s" % (self.domain_name)
        log.I("command [createDomain] correctly executed")
        log.I("Domain %s created" % (self.domain_name))

        # Initial domains listing using "listDomains" command
        log.I("Creating a domains listing backup")
        log.I("command [listDomains]")
        out, err = self.pfw.sendCmd("listDomains","","")
        assert err == None, "INFO : command [listDomains] - ERROR while listing domains"
        log.I("command [listDomains] correctly executed")
        # Saving initial domains names
        f_init_domains = open("f_init_domains", "w")
        f_init_domains.write(out)
        f_init_domains.close()
        log.I("Domains listing backup created")

        # Checking domains number
        f_init_domains = open("f_init_domains", "r")
        domains_nbr = 0
        line=f_init_domains.readline()
        while line!="":
            line=f_init_domains.readline()
            domains_nbr+=1
        f_init_domains.close()
        os.remove("f_init_domains")
        log.I("%s domains names saved" % domains_nbr)

        # Domain renaming iterations
        log.I("Checking domain renaming - %s iterations" % self.renaming_iterations)
        old_name = self.domain_name
        new_name = self.new_domain_name
        for iteration in range (self.renaming_iterations):
            log.I("Iteration %s" % (iteration))
            log.I("Renaming domain %s to %s" % (old_name,new_name))
            log.I("command [renameDomain]")
            out, err = self.pfw.sendCmd("renameDomain",old_name,new_name)
            assert out == "Done", out
            assert err == None, "ERROR : command [renameDomain] - ERROR while renaming domain %s" % (old_name)
            # Domains listing using "listDomains" command
            log.I("Creating a domains listing")
            log.I("command [listDomains]")
            out, err = self.pfw.sendCmd("listDomains","","")
            assert err == None, "ERROR : command [listDomains] - ERROR while listing domains"
            log.I("command [listDomains] correctly executed")
            # Saving domains names
            f_domains = open("f_domains", "w")
            f_domains.write(out)
            f_domains.close()
            log.I("Domains listing created")
            # Checking renaming
            log.I("Checking that renaming is correct in domains listing")
            f_domains = open("f_domains", "r")
            for line in range(domains_nbr):
                if (line >= (domains_nbr - 1)):
                    domain_renamed = f_domains.readline().strip('\n')
                    assert domain_renamed==new_name, "ERROR : Error while renaming domain %s" % (old_name)
                else:
                    f_domains.readline()
            f_domains.close()
            log.I("New domain name %s conform to expected value" % (new_name))
            temp = old_name
            old_name = new_name
            new_name = temp
            os.remove("f_domains")

    def test_Renaming_Error(self):
        """
        Renaming errors
        ---------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - renaming a non existent domain
                - renaming a domain with an already existent domain name
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [renameDomain] function
                - [createDomain] function
                - [renameDomain] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - error detected
                - domains names remain unchanged
        """
        log.D(self.test_Renaming_Error.__doc__)
        # New domains creation
        log.I("New domain creation : %s" % (self.domain_name))
        log.I("command [createDomain]")
        out, err = self.pfw.sendCmd("createDomain",self.domain_name, "")
        assert out == "Done", out
        assert err == None, "ERROR : command [createDomain] - Error while creating domain %s" % (self.domain_name)
        log.I("command [createDomain] - correctly executed")
        log.I("command Domain %s created" % (self.domain_name))

        # Initial domains listing using "listDomains" command
        log.I("Creating a domains listing backup")
        log.I("command [listDomains]")
        out, err = self.pfw.sendCmd("listDomains","","")
        assert err == None, "INFO : command [listDomains] - Error while listing domains"
        log.I("command [listDomains] correctly executed")
        # Saving initial domains names
        f_init_domains = open("f_init_domains", "w")
        f_init_domains.write(out)
        f_init_domains.close()
        log.I("Domains listing backup created")

        # Checking domains number
        f_init_domains = open("f_init_domains", "r")
        domains_nbr = 0
        line=f_init_domains.readline()
        while line!="":
            line=f_init_domains.readline()
            domains_nbr+=1
        f_init_domains.close()
        log.I("%s domains names saved" % domains_nbr)

        # Domain renaming error : renamed domain does not exist
        log.I("Renaming a non existent domain")
        log.I("Renaming domain FAKE to NEW_NAME")
        log.I("command [renameDomain]")
        out, err = self.pfw.sendCmd("renameDomain",'FAKE','NEW_NAME')
        assert out != "Done", out
        assert err == None, "ERROR : command [renameDomain] - Error while renaming domain"
        log.I("command [renameDomain] - renaming error correctly detected")
        # Domains listing using "listDomains" command
        log.I("Creating a domains listing")
        log.I("command [listDomains]")
        out, err = self.pfw.sendCmd("listDomains","","")
        assert err == None, "ERROR : command [listDomains] - Error while listing domains"
        log.I("command [listDomains] correctly executed")
        # Saving domains names
        f_domains = open("f_domains", "w")
        f_domains.write(out)
        f_domains.close()
        log.I("Domains listing created")
        # Checking domains names integrity
        log.I("Checking domains names integrity")
        f_domains = open("f_domains", "r")
        f_init_domains = open("f_init_domains", "r")
        for line in range(domains_nbr):
            domain_name = f_domains.readline().strip('\n')
            domain_backup_name = f_init_domains.readline().strip('\n')
            assert domain_name==domain_backup_name, "ERROR : Domain name %s affected by the renaming error" % (domain_backup_name)
        f_domains.close()
        f_init_domains.close()
        log.I("Domains names not affected by the renaming error")
        os.remove("f_domains")

        # Domain renaming error : renaming a domain with an already existent domain name
        log.I("renaming a domain with an already existent domain name")
        log.I("Renaming domain %s to %s" % (self.domain_name,self.new_domain_name) )
        log.I("command [renameDomain]")
        out, err = self.pfw.sendCmd("renameDomain",self.domain_name,self.new_domain_name)
        assert out != "Done", out
        assert err == None, "INFO : command [renameDomain] - Error while renaming domain"
        log.I("command [renameDomain] - renaming error correctly detected")
        # Domains listing using "listDomains" command
        log.I("Creating a domains listing")
        log.I("command [listDomains]")
        out, err = self.pfw.sendCmd("listDomains","","")
        assert err == None, "ERROR : command [listDomains] - Error while listing domains"
        log.I("command [listDomains] correctly executed")
        # Saving domains names
        f_domains = open("f_domains", "w")
        f_domains.write(out)
        f_domains.close()
        log.I("Domains listing created")
        # Checking domains names integrity
        log.I("Checking domains names integrity")
        f_domains = open("f_domains", "r")
        f_init_domains = open("f_init_domains", "r")
        for line in range(domains_nbr):
            domain_name = f_domains.readline().strip('\n')
            domain_backup_name = f_init_domains.readline().strip('\n')
            assert domain_name==domain_backup_name, "ERROR : domain name %s affected by the renaming error" % (domain_backup_name)
        f_domains.close()
        f_init_domains.close()
        log.I("Domains names not affected by the renaming error")
        os.remove("f_domains")
        os.remove("f_init_domains")

    def test_Special_Cases(self):
        """
        Special cases
        -------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - renaming a domain with its own name
            Tested commands :
           ~~~~~~~~~~~~~~~~~
                - [renameDomain] function
                - [createDomain] function
                - [listDomains] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - no error
                - domains names remain unchanged
        """
        log.D(self.test_Special_Cases.__doc__)
        # New domain creation
        # Already created in previous test

        # Initial domains listing using "listDomains" command
        log.I("Creating a domains listing backup")
        log.I("command [listDomains]")
        out, err = self.pfw.sendCmd("listDomains","","")
        assert err == None, "ERROR : command [listDomains] - Error while listing domains"
        log.I("command [listDomains] correctly executed")
        # Saving initial domains names
        f_init_domains = open("f_init_domains", "w")
        f_init_domains.write(out)
        f_init_domains.close()
        log.I("Domains listing backup created")

        # Checking domains number
        f_init_domains = open("f_init_domains", "r")
        domains_nbr = 0
        line=f_init_domains.readline()
        while line!="":
            line=f_init_domains.readline()
            domains_nbr+=1
        f_init_domains.close()
        log.I("%s domains names saved" % domains_nbr)

        # Domain renaming error : renaming a domain with its own name
        log.I("renaming a domain with its own name")
        log.I("Renaming domain %s to %s" % (self.domain_name,self.domain_name))
        log.I("command [renameDomain]")
        out, err = self.pfw.sendCmd("renameDomain",self.domain_name,self.domain_name)
        assert out == "Done", out
        assert err == None, "ERROR : command [renameDomain] - Error while renaming domain"
        log.I("command [renameDomain] correctly executed")
        # Domains listing using "listDomains" command
        log.I("Creating a domains listing")
        log.I("command [listDomains]")
        out, err = self.pfw.sendCmd("listDomains","","")
        assert err == None, "ERROR : command [listDomains] - Error while listing domains"
        log.I("command [listDomains] correctly executed")
        # Saving domains names
        f_domains = open("f_domains", "w")
        f_domains.write(out)
        f_domains.close()
        log.I("Domains listing created")
        # Checking domains names integrity
        log.I("Checking domains names integrity")
        f_domains = open("f_domains", "r")
        f_init_domains = open("f_init_domains", "r")
        for line in range(domains_nbr):
            domain_name = f_domains.readline().strip('\n')
            domain_backup_name = f_init_domains.readline().strip('\n')
            assert domain_name==domain_backup_name, "ERROR : domain name %s affected by the renaming" % (domain_backup_name)
        f_domains.close()
        f_init_domains.close()
        log.I("Domains names not affected by the renaming")

        os.remove("f_domains")
        os.remove("f_init_domains")
