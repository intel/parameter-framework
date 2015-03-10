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
synchronization functions testcases

List of tested functions :
--------------------------
    - [getAutoSync]  function
    - [setAutoSync]  function
    - [sync]  function

Test cases :
------------
    - Testing getAutoSync nominal case
    - Testing setAutoSync nominal case
    - Testing sync nominal case
    - Testing errors
"""
import commands, os
import unittest
from Util.PfwUnitTestLib import PfwTestCase
from Util import ACTLogging
log=ACTLogging.Logger()

class TestCases(PfwTestCase):

    def setUp(self):

        pfw_filesystem=os.getenv("PFW_RESULT")

        self.pfw.sendCmd("setTuningMode", "on")
        self.param_name_01 = "/Test/Test/TEST_DIR/BOOL"
        self.filesystem_01 = pfw_filesystem+"/BOOL"
        self.param_name_02 = "/Test/Test/TEST_DIR/INT16"
        self.filesystem_02 = pfw_filesystem+"/INT16"
        self.param_name_03 = "/Test/Test/TEST_DIR/UINT32"
        self.filesystem_03 = pfw_filesystem+"/UINT32"

    def tearDown(self):
        self.pfw.sendCmd("setTuningMode", "off")

    def test_01_getAutoSync_Case(self):
        """
        Testing getAutoSync nominal case
        ----------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - enable autosync
                - get autosync state
                - disable autosync
                - get autosync state
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setAutoSync] function
                - [getAutoSync] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - getAutoSync return expected state
        """
        log.D(self.test_01_getAutoSync_Case.__doc__)
        value = "on"
        log.I("Enable autosync")
        out,err = self.pfw.sendCmd("setAutoSync", value)
        assert err == None, log.E("When enabling autosync : %s" % (err))
        assert out == "Done", log.F("setAutoSync - expected : Done , found : %s" % (out))
        log.I("Check autosync state")
        out, err = self.pfw.sendCmd("getAutoSync","")
        assert err == None, log.E("When getting autosync state : %s" % (err))
        assert out == value, log.F("setAutoSync - expected : %s , found : %s" % (value,out))
        value = "off"
        log.I("Disable autosync")
        out,err = self.pfw.sendCmd("setAutoSync", value)
        assert err == None, log.E("When enabling autosync : %s" % (err))
        assert out == "Done", log.F("setAutoSync - expected : Done , found : %s" % (out))
        log.I("Check autosync state")
        out, err = self.pfw.sendCmd("getAutoSync","")
        assert err == None, log.E("When getting autosync state : %s" % (err))
        assert out == value, log.F("setAutoSync - expected : %s , found : %s" % (value,out))

    def test_02_setAutoSync_Case(self):
        """
        Testing getAutoSync nominal case
        -------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - enable autosync
                - set differents parameters
                - check the value on the filesystem
                - disable autosync
                - set differents parameters
                - check the value on the filesystem
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setAutoSync] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getAutoSync] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - When autosync is enabled, the filesystem is automatically
                synchronized with the blackboard.
        """
        log.D(self.test_02_setAutoSync_Case.__doc__)
        #Check the initial parameter value
        init_value_01, err = self.pfw.sendCmd("getParameter", self.param_name_01, "")
        init_value_02, err = self.pfw.sendCmd("getParameter", self.param_name_02, "")
        init_value_03, err = self.pfw.sendCmd("getParameter", self.param_name_03, "")
        init_filesystem_01 = commands.getoutput("cat %s"%(self.filesystem_01))
        init_filesystem_02 = commands.getoutput("cat %s"%(self.filesystem_02))
        init_filesystem_03 = commands.getoutput("cat %s"%(self.filesystem_03))
        #Implement a new value
        if int(init_value_01)==0 :
            new_value_01 = "1"
        else :
            new_value_01 = "0"
        new_value_02 = str(int(init_value_02)+1)
        new_value_03 = str(int(init_value_03)+1)
        #Enable the autosync
        value = "on"
        log.I("Enable autosync")
        out,err = self.pfw.sendCmd("setAutoSync", value)
        assert err == None, log.E("When enabling autosync : %s" % (err))
        assert out == "Done", log.F("setAutoSync - expected : Done , found : %s" % (out))
        #Set the new parameter value
        self.pfw.sendCmd("setParameter", self.param_name_01, new_value_01)
        self.pfw.sendCmd("setParameter", self.param_name_02, new_value_02)
        self.pfw.sendCmd("setParameter", self.param_name_03, new_value_03)
        #Check the filesystem values
        #BOOL
        assert commands.getoutput("cat %s"%(self.filesystem_01)) != init_filesystem_01, log.F("FILESYSTEM : parameter %s update error"%self.param_name_01)
        #INT16
        assert commands.getoutput("cat %s"%(self.filesystem_02)) != init_filesystem_02, log.F("FILESYSTEM : parameter %s update error"%self.param_name_02)
        #UINT32
        assert commands.getoutput("cat %s"%(self.filesystem_03)) != init_filesystem_03, log.F("FILESYSTEM : parameter %s update error"%self.param_name_03)
        log.I("test setAutoSync %s : OK"%(value))
        #Enable the autosync
        value = "off"
        log.I("Disable autosync")
        out,err = self.pfw.sendCmd("setAutoSync", value)
        assert err == None, log.E("When enabling autosync : %s" % (err))
        assert out == "Done", log.F("setAutoSync - expected : Done , found : %s" % (out))
        #Set the new parameter value
        self.pfw.sendCmd("setParameter", self.param_name_01, init_value_01)
        self.pfw.sendCmd("setParameter", self.param_name_02, init_value_02)
        self.pfw.sendCmd("setParameter", self.param_name_03, init_value_03)
        #Check the filesystem values
        #BOOL
        assert commands.getoutput("cat %s"%(self.filesystem_01)) != init_filesystem_01, log.F("FILESYSTEM : parameter %s  is updated, autosync is still enabled"%self.param_name_01)
        #INT16
        assert commands.getoutput("cat %s"%(self.filesystem_02)) != init_filesystem_02, log.F("FILESYSTEM : parameter %s  is updated, autosync is still enabled"%self.param_name_02)
        #UINT32
        assert commands.getoutput("cat %s"%(self.filesystem_03)) != init_filesystem_03, log.F("FILESYSTEM : parameter %s  is updated, autosync is still enabled"%self.param_name_03)
        log.I("test setAutoSync %s : OK"%(value))


    @unittest.expectedFailure
    def test_03_Manual_Sync_Case(self):
        """
        Testing getAutoSync nominal case
        -------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - disable autosync
                - set differents parameters
                - check the value on the filesystem
                - sync
                - check the value on the filesystem
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [sync] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [setAutoSync] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - sync should synchronized filessystem with blackboard
        """
        log.D(self.test_03_Manual_Sync_Case.__doc__)
        #Check the initial parameter value
        init_value_01, err = self.pfw.sendCmd("getParameter", self.param_name_01, "")
        init_value_02, err = self.pfw.sendCmd("getParameter", self.param_name_02, "")
        init_value_03, err = self.pfw.sendCmd("getParameter", self.param_name_03, "")
        init_filesystem_01 = commands.getoutput("cat %s"%(self.filesystem_01))
        init_filesystem_02 = commands.getoutput("cat %s"%(self.filesystem_02))
        init_filesystem_03 = commands.getoutput("cat %s"%(self.filesystem_03))
        #Implement a new value
        if int(init_value_01)==0 :
            new_value_01 = "1"
        else :
            new_value_01 = "0"
        new_value_02 = str(int(init_value_02)+1)
        new_value_03 = str(int(init_value_03)+1)
        #Enable the autosync
        value = "off"
        log.I("Disable autosync")
        out,err = self.pfw.sendCmd("setAutoSync", value)
        assert err == None, log.E("When enabling autosync : %s" % (err))
        assert out == "Done", log.F("setAutoSync - expected : Done , found : %s" % (out))
        #Set the new parameter value
        self.pfw.sendCmd("setParameter", self.param_name_01, new_value_01)
        self.pfw.sendCmd("setParameter", self.param_name_02, new_value_02)
        self.pfw.sendCmd("setParameter", self.param_name_03, new_value_03)
        #Check the filesystem values, must not changed
        #BOOL
        assert commands.getoutput("cat %s"%(self.filesystem_01)) == init_filesystem_01, log.F("FILESYSTEM : parameter %s update error"%self.param_name_01)
        #INT16
        assert commands.getoutput("cat %s"%(self.filesystem_02)) == init_filesystem_02, log.F("FILESYSTEM : parameter %s update error"%self.param_name_02)
        #UINT32
        assert commands.getoutput("cat %s"%(self.filesystem_03)) == init_filesystem_03, log.F("FILESYSTEM : parameter %s update error"%self.param_name_03)
        log.I("test setAutoSync %s : OK"%(value))
        log.I("Sync")
        out,err = self.pfw.sendCmd("sync", "")
        assert err == None, log.E("When syncing : %s" % (err))
        assert out == "Done", log.F("Sync - expected : Done , found : %s" % (out))
        #Check the filesystem values
        #BOOL
        assert commands.getoutput("cat %s"%(self.filesystem_01)) != init_filesystem_01, log.F("FILESYSTEM : parameter %s  is updated, autosync is still enabled"%self.param_name_01)
        #INT16
        assert commands.getoutput("cat %s"%(self.filesystem_02)) != init_filesystem_02, log.F("FILESYSTEM : parameter %s  is updated, autosync is still enabled"%self.param_name_02)
        #UINT32
        assert commands.getoutput("cat %s"%(self.filesystem_03)) != init_filesystem_03, log.F("FILESYSTEM : parameter %s  is updated, autosync is still enabled"%self.param_name_03)
        log.I("test setAutoSync %s : OK"%(value))
