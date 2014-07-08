#
# INTEL CONFIDENTIAL
# Copyright 2014 Intel
# Corporation All Rights Reserved.
#
# The source code contained or described herein and all documents related to
# the source code ("Material") are owned by Intel Corporation or its suppliers
# or licensors. Title to the Material remains with Intel Corporation or its
# suppliers and licensors. The Material contains trade secrets and proprietary
# and confidential information of Intel or its suppliers and licensors. The
# Material is protected by worldwide copyright and trade secret laws and
# treaty provisions. No part of the Material may be used, copied, reproduced,
# modified, published, uploaded, posted, transmitted, distributed, or
# disclosed in any way without Intels prior express written permission.
#
# No license under any patent, copyright, trade secret or other intellectual
# property right is granted to or conferred upon you by disclosure or delivery
# of the Materials, either expressly, by implication, inducement, estoppel or
# otherwise. Any license under such intellectual property rights must be
# express and approved by Intel in writing.
#

from criterion.ExclusiveCriterion import ExclusiveCriterion
from configuration.ConfigParser import ConfigParser
import logging
import threading
import subprocess
import time
import os
import sys

class TestLauncher:
      """ Class which interacts with the system to launch tests"""

      def __init__(self, criterionClasses, configParser, consoleLogger):
            """ Here we create commands to launch thanks to the config Parser"""
            self.__criterionClasses = criterionClasses

            #Combination from previous
            PFWtest_HALCommand=[configParser["PFWtest_RemoteProcessCommand"],
                                configParser["PFWtest_TestPlatformHost"]]
            PFWtest_SetCriteriaCommand=PFWtest_HALCommand+["setCriterionState"]
            PFWtest_TestPlatformHostCommand=[configParser["PFWtest_RemoteProcessCommand"],
                                             configParser["PFWtest_TestPlatformHost"]]

            killAllCmd = "killall"
            backgroundProcess = "&"

            self.__logFileName = configParser["PFWtest_LogFile"]

            #Commands
            self.__killTestPlatformCmd = [configParser["PFWtest_PrefixCommand"],
                                          killAllCmd,configParser["PFWtest_TestPlatformCommand"]]

            self.__startTestPlatformCmd = [configParser["PFWtest_PrefixCommand"],
                                           configParser["PFWtest_TestPlatformCommand"],
                                           configParser["PFWtest_PFWConfFile"], backgroundProcess]

            self.__createCriterionCmd = [configParser["PFWtest_PrefixCommand"]]
            self.__createCriterionCmd.extend(PFWtest_TestPlatformHostCommand)

            self.__startPseudoHALCmd = [configParser["PFWtest_PrefixCommand"]]
            self.__startPseudoHALCmd.extend(PFWtest_TestPlatformHostCommand)
            self.__startPseudoHALCmd.append("start")

            self.__setCriterionCmd = [configParser["PFWtest_PrefixCommand"]]
            self.__setCriterionCmd.extend(PFWtest_SetCriteriaCommand)

            self.__applyConfigurationsCmd = [configParser["PFWtest_PrefixCommand"]]
            self.__applyConfigurationsCmd.extend(PFWtest_HALCommand)
            self.__applyConfigurationsCmd.append("applyConfigurations")

            self.__statusCmd = [configParser["PFWtest_RemoteProcessCommand"],
                                configParser["PFWtest_ParameterFramworkHost"],"status"]

            self.__playMediaCmd = [configParser["PFWtest_PrefixCommand"],
                                   configParser["PFWtest_PlayCmd"],
                                   configParser["PFWtest_PlayCmdArgs"],
                                   configParser["PFWtest_PlayedMedia"],
                                   backgroundProcess]

            self.__captureMediaCmd = [configParser["PFWtest_PrefixCommand"],
                                      configParser["PFWtest_CaptureCmd"],
                                      configParser["PFWtest_CaptureCmdArgs"],
                                      configParser["PFWtest_CapturedMedia"],
                                      backgroundProcess]

            self.__loopbackMediaCmd = [configParser["PFWtest_PrefixCommand"], "eval", "\"",
                                       configParser["PFWtest_CaptureCmd"],
                                       configParser["PFWtest_LoopbackCmdArgs"], " | ",
                                       configParser["PFWtest_PlayCmd"],
                                       configParser["PFWtest_LoopbackCmdArgs"],
                                       "\"",backgroundProcess]

            self.__killPlayMediaCmd = [configParser["PFWtest_PrefixCommand"], killAllCmd,
                                       configParser["PFWtest_CaptureCmd"]]
            self.__killCaptureMediaCmd = [configParser["PFWtest_PrefixCommand"], killAllCmd,
                                          configParser["PFWtest_PlayCmd"]]

            #AT Commands

            self.__openGsmTTYCmd = [configParser["PFWtest_PrefixCommand"],
                                    "cat",
                                    configParser["PFWtest_GsmTty"],
                                    backgroundProcess]
            self.__registerLineCmd = [configParser["PFWtest_PrefixCommand"],
                                      " ".join(["\"","echo","-e -n",
                                      configParser["PFWtest_ATCmd_Register"],
                                      ">",
                                      configParser["PFWtest_GsmTty"],"\""])]
            self.__launchCallCmd = [configParser["PFWtest_PrefixCommand"],
                                    " ".join(["\"","echo","-e -n",
                                    configParser["PFWtest_ATCmd_Call"],
                                    ">",
                                    configParser["PFWtest_GsmTty"],"\""])]

            #Media Commands Dict
            self.__media_commands = {
                  "play": lambda: self.__call_process(self.__playMediaCmd,True),
                  "capture": lambda: self.__call_process(self.__captureMediaCmd,True),
                  "kill": self.kill_media,
                  "loopback": lambda: self.__call_process(self.__loopbackMediaCmd,True),
                  "call": self.__csv_Call
                  }


            #Routing Criterion
            self.__routageStateCriterion = self.__criterionClasses[
                  configParser["PFWtest_RouteStateCriterionName"]]()

            self.__logger = logging.getLogger(__name__)
            self.__logger.addHandler(consoleLogger)

      def init(self, testVectorDefault, isVerbose):
            """ Initialise the Pseudo HAL """
            self.__logger.info("Pseudo Hal Initialisation")
            self.kill_TestPlatform()
            self.__call_process(self.__startTestPlatformCmd,True)
            # wait Initialisation
            time.sleep(1)

            for criterion in testVectorDefault.criterions+[self.__routageStateCriterion]:
                  if ExclusiveCriterion in criterion.__class__.__bases__:
                        createSlctCriterionCmd="createExclusiveSelectionCriterionFromStateList"
                  else:
                        createSlctCriterionCmd="createInclusiveSelectionCriterionFromStateList"

                  createCriterionArgs = [createSlctCriterionCmd,
                                         criterion.__class__.__name__]+criterion.allowedValues

                  self.__call_process(self.__createCriterionCmd+createCriterionArgs)

            self.__call_process(self.__startPseudoHALCmd)


      def __applyConfigurations(self):
            """ Interact with the PFW instance to set criterions and apply configurations  """
            self.__logger.info("Applying Configurations")
            for routageState in self.__routageStateCriterion.allowedValues:
                  setCriterionArgs = [self.__routageStateCriterion.__class__.__name__,
                                      routageState]
                  self.__call_process(self.__setCriterionCmd+setCriterionArgs)
                  self.__call_process(self.__applyConfigurationsCmd)


      def execute(self, testVector):
            """ Launch the Test """
            self.__logger.info("Launching Test")

            #Launching Media command corresponding to the testType
            try:
                  self.__media_commands[testVector.testType]()
            except KeyError as e:
                  self.__logger.error("Wrong Type of Test, Killing alsa instances")
                  self.kill_media()

            for criterion in testVector.criterions:
                  if ExclusiveCriterion in criterion.__class__.__bases__:
                        criterionValue = [criterion.currentValue]
                  else:
                        criterionValue = criterion.currentValue
                  setCriterionArgs = [criterion.__class__.__name__]+list(criterionValue)
                  self.__call_process(self.__setCriterionCmd+setCriterionArgs)
            self.__applyConfigurations()

      def kill_media(self):
            """ Kill audio related process """
            self.__call_process(self.__killPlayMediaCmd)
            self.__call_process(self.__killCaptureMediaCmd)

      def kill_TestPlatform(self):
            """ Kill an instance of the TestPlatform """
            self.__call_process(self.__killTestPlatformCmd)

      def __call_process(self,cmd,isWaited=False):
            """ Private function which call a shell command """
            self.__logger.debug("Launching command : {}".format(' '.join(cmd)))

            if not isWaited:
                # for getting special adb env script
                subProc = subprocess.Popen([os.getenv("SHELL"), "-c", ' '.join(cmd)],
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE,
                            shell=False)

                stdout,stderr = subProc.communicate()

                if stdout:
                    self.__logger.debug(stdout.decode('utf-8').rstrip())
                if stderr:
                    self.__logger.error(stderr.decode('utf-8').rstrip())
                subProc.wait()

            else:
                launcher = TestLoggerThread(cmd)
                launcher.start()

      def __csv_Call(self):
            """ Launch a call via AT commands """
            self.__logger.info("Calling")
            #self.__call_process(self.__openGsmTTYCmd)
            self.__call_process(self.__registerLineCmd)
            self.__logger.info("Waiting Operator Registration")
            time.sleep(6)
            #wait the registration
            self.__call_process(self.__launchCallCmd)



class TestLoggerThread(threading.Thread):
    """ This class is here to log long process stdout and stderr """

    def __init__(self,cmd):
        super().__init__()
        #This thread is a daemon because we must be able to quit without killing all childs
        self.daemon = True
        self.__cmd = cmd
        self.__logger = logging.getLogger(__name__)

    def run(self):
        #logging stdout and stderr through pipes
        subProc = subprocess.Popen([os.getenv("SHELL"), "-c", ' '.join(self.__cmd)],
                                    stdout=subprocess.PIPE,
                                    stderr=subprocess.PIPE,
                                    shell=False)

        for line in iter(subProc.stdout.readline,''):
            formatted=line.decode('utf-8').rstrip()
            if formatted:  self.__logger.debug(formatted)

        for line in iter(subProc.stderr.readline,''):
            formatted=line.decode('utf-8').rstrip()
            if formatted: self.__logger.error(formatted)

        subProc.wait()


