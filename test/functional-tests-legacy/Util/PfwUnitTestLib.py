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

import subprocess
import unittest
import time

class RemoteCli(object):
    def sendCmd(self, cmd, *args):
        shell_cmd = " ".join([self.platform_command, cmd])
        if args is not None:
            shell_cmd += " " + " ".join(args)
        print "CMD  :",
        print "[" + shell_cmd + "]"
        try:
            p = subprocess.Popen(shell_cmd, shell=True, stdout=subprocess.PIPE)
        except Exception as (errno, strerror):
            return None, strerror
        out, err = p.communicate()
        if out is not None:
            out = out.strip()
        return out, err

class Pfw(RemoteCli):
    def __init__(self):
        self.platform_command = "remote-process localhost 5000 "

class Hal(RemoteCli):
    def __init__(self):
        self.platform_command = "remote-process localhost 5001 "

    # Starts the HAL exe
    def startHal(self):
        cmd= "test-platform $PFW_TEST_CONFIGURATION"
        subprocess.Popen(cmd, shell=True)
        pass

    # Send command "stop" to the HAL
    def stopHal(self):
        subprocess.call("remote-process localhost 5001 exit", shell=True)

    def createInclusiveCriterion(self, name, nb):
        self.sendCmd("createInclusiveSelectionCriterion", name, nb)

    def createExclusiveCriterion(self, name, nb):
        self.sendCmd("createExclusiveSelectionCriterion", name, nb)

    # Starts the Pfw
    def start(self):
        self.sendCmd("start")

# A PfwTestCase gather tests performed on one instance of the PFW.
class PfwTestCase(unittest.TestCase):

    hal = Hal()

    def __init__(self, argv):
        super(PfwTestCase, self).__init__(argv)
        self.pfw = Pfw()

    @classmethod
    def setUpClass(cls):
        cls.startHal()

    @classmethod
    def tearDownClass(cls):
        cls.stopHal()

    @classmethod
    def startHal(cls):
        # set up the Hal & pfw
        cls.hal.startHal()
        time.sleep(0.1)
        # create criterions
        cls.hal.createInclusiveCriterion("Crit_0", "2")
        cls.hal.createExclusiveCriterion("Crit_1", "2")
        # start the Pfw
        cls.hal.start()

    @classmethod
    def stopHal(cls):
        cls.hal.stopHal()
        time.sleep(0.1)
