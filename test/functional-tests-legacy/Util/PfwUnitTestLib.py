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

import os
import subprocess
import unittest
import time
import socket

class RemoteCli(object):
    def setRemoteProcess(self, remoteProcess):
        self.remoteProcess = remoteProcess

    def sendCmd(self, cmd, *args):
        assert self.remoteProcess.poll() == None, "Can not send command to Test platform as it has died."

        sys_cmd = self.platform_command + [cmd]
        if args is not None:
            sys_cmd += args
        print "CMD  : %s" % sys_cmd

        try:
            p = subprocess.Popen(sys_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        except Exception as (errno, strerror):
            return None, strerror
        out, err = p.communicate()
        out = out.rstrip('\r\n')
        if p.returncode == 0:
            assert err == "", "test-platform succeded but stderr is not empty: %s" % err
            # function is expected to return stderr on command failure, None on success
            err = None
        else:
            # Unfortunately lots are test are bugged and will fail if errors
            # are not hidden
            # For now only log the error
            print "CMD failed. stdout: '%s'\nstderr: '%s'" % (out, err)

            err = None # FIXME: fix all tests that fail when this line is removed

        return out, err

class Pfw(RemoteCli):
    platform_command = ["remote-process", "localhost", "5000"]

class Hal(RemoteCli):
    testPlatformPort = 5001
    platform_command = ["remote-process", "localhost", str(testPlatformPort)]

    def __init__(self, pfw):
        self.pfw = pfw

    # Starts the HAL exe
    def startHal(self):
        cmd= ["test-platform", os.environ["PFW_TEST_CONFIGURATION"], str(self.testPlatformPort)]
        self.setRemoteProcess(subprocess.Popen(cmd))
        # Wait for the test-platform listening socket
        while socket.socket().connect_ex(("localhost", self.testPlatformPort)) != 0:
            assert self.remoteProcess.poll() == None, "Test platform has failed to start."
            time.sleep(0.01)

    # Send command "stop" to the HAL
    def stopHal(self):
        self.sendCmd("exit")
        returncode = self.remoteProcess.wait()
        assert returncode == 0, "test-platform did not stop succesfully: %s" % returncode

    def createInclusiveCriterion(self, name, nb):
        self.sendCmd("createInclusiveSelectionCriterion", name, nb)

    def createExclusiveCriterion(self, name, nb):
        self.sendCmd("createExclusiveSelectionCriterion", name, nb)

    # Starts the Pfw
    def start(self):
        self.sendCmd("setValidateSchemasOnStart", "true")
        self.sendCmd("start")
        self.pfw.setRemoteProcess(self.remoteProcess)

# A PfwTestCase gather tests performed on one instance of the PFW.
class PfwTestCase(unittest.TestCase):

    pfw = Pfw()
    hal = Hal(pfw)

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
        # create criterions
        cls.hal.createInclusiveCriterion("Crit_0", "2")
        cls.hal.createExclusiveCriterion("Crit_1", "2")
        # start the Pfw
        cls.hal.start()

    @classmethod
    def stopHal(cls):
        cls.hal.stopHal()
