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
from testGenerator.SubprocessLogger import SubprocessLoggerThread
from testGenerator.SubprocessLogger import ScriptLoggerThread
import logging
import json
import time
import os

class TestLauncher:
    """ Class which interacts with the system to launch tests """

    def __init__(self,
                 criterionClasses,
                 configParser,
                 consoleLogger):
        """
            Here we create commands to launch thanks to the config Parser

            :param criterionClasses: runtime generated criterion classes
            :type criterionClasses: list of types
            :param configParser: object which allows to get config parameters
            :type configParser: ConfigParser
            :param consoleLogger: console log handler
            :type consoleLogger: Handler
        """
        self.__criterionClasses = criterionClasses
        self.__configParser = configParser

        # Prepare basic commands
        HALCommand=[configParser["RemoteProcessCommand"],
                            configParser["TestPlatformHost"]]
        SetCriteriaCommand=HALCommand+["setCriterionState"]
        TestPlatformHostCommand=[configParser["RemoteProcessCommand"],
                                         configParser["TestPlatformHost"]]

        self.__logFileName = configParser["LogFile"]

        # Commands
        self.__startTestPlatformCmd = [configParser["PrefixCommand"],
                                       configParser["TestPlatformCommand"],
                                       configParser["PFWConfFile"]]

        self.__createCriterionCmd = [configParser["PrefixCommand"]]
        self.__createCriterionCmd.extend(TestPlatformHostCommand)

        self.__startPseudoHALCmd = [configParser["PrefixCommand"]]
        self.__startPseudoHALCmd.extend(TestPlatformHostCommand)
        self.__startPseudoHALCmd.append("start")

        self.__setCriterionCmd = [configParser["PrefixCommand"]]
        self.__setCriterionCmd.extend(SetCriteriaCommand)

        self.__applyConfigurationsCmd = [configParser["PrefixCommand"]]
        self.__applyConfigurationsCmd.extend(HALCommand)
        self.__applyConfigurationsCmd.append("applyConfigurations")

        self.__setupScript = [configParser["SetupScript"]]

        # Command used to generate coverage
        self.__coverageCmd = [
                    "eval",
                    configParser["CoverageDir"]+"/aplog2coverage.sh",
                    "-d",
                    configParser["PFWDomainConfFile"],
                    "-e.",
                    self.__logFileName,
                    "-f",
                    "-o",
                    configParser["CoverageFile"]
                ]

        # Prepare script Commands
        # Loading possible scripts
        with open(configParser["ScriptsFile"],'r') as scriptFile:
            self.__rawScripts = json.load(scriptFile)

        self.__availableLaunchType = ["asynchronous", "synchronous"]

        self.__consoleLogger = consoleLogger
        self.__logger = logging.getLogger(__name__)
        self.__logger.addHandler(consoleLogger)

    @property
    def scripts(self):
        return self.__rawScripts.keys()

    def init(self, criterionClasses, isVerbose):
        """ Initialise the Pseudo HAL """

        # Use user script to setup environment as requested before to do anything
        self.__logger.info("Launching Setup script")
        self.__call_process(self.__setupScript)

        self.__logger.info("Pseudo Hal Initialisation")
        # Test platform is launched asynchronously and not as script
        self.__call_process(self.__startTestPlatformCmd, True)
        # wait Initialisation
        time.sleep(1)

        for criterionClass in criterionClasses:
            if ExclusiveCriterion in criterionClass.__bases__:
                createSlctCriterionCmd="createExclusiveSelectionCriterionFromStateList"
            else:
                createSlctCriterionCmd="createInclusiveSelectionCriterionFromStateList"

            createCriterionArgs = [createSlctCriterionCmd,
                           criterionClass.__name__]+criterionClass.allowedValues()

            self.__call_process(self.__createCriterionCmd+createCriterionArgs)

        self.__call_process(self.__startPseudoHALCmd)

    def executeTestVector(self, criterions):
        """ Launch the Test """
        for criterion in criterions:
            if ExclusiveCriterion in criterion.__class__.__bases__:
                criterionValue = [criterion.currentValue]
            else:
                criterionValue = criterion.currentValue
            setCriterionArgs = [criterion.__class__.__name__]+list(criterionValue)
            self.__call_process(self.__setCriterionCmd+setCriterionArgs)

        # Applying conf
        self.__call_process(self.__applyConfigurationsCmd)

    def executeScript(self, scriptName):
        """ Launching desired test scripts """

        (script, launchType) = self.__rawScripts[scriptName]

        if not launchType in self.__availableLaunchType:
            errorMessage = "Launch type ({}) for script {} isn't recognized. ".format(
                    launchType,
                    scriptName)
            errorMessage += "Default value ({}) has been applied.".format(
                    self.__availableLaunchType[0])

            self.__logger.error(errorMessage)
            launchType = self.__availableLaunchType[0]

        # Create and launch the command to use the desired script
        self.__call_process(
                ["eval","{}/{}".format(
                    os.path.split(self.__configParser["ScriptsFile"])[0],
                    script)],
                launchType == self.__availableLaunchType[0],
                True)

    def generateCoverage(self):
        """ Launch Coverage Tool on generated Log and save results in dedicated file  """
        self.__logger.debug("Generating coverage file")
        self.__call_process(self.__coverageCmd)

    def __call_process(self, cmd, isAsynchronous=False, isScriptThread=False):
        """ Private function which call a shell command """

        if isScriptThread:
            self.__logger.info("Launching script : {}".format(' '.join(cmd)))
            launcher = ScriptLoggerThread(cmd, self.__consoleLogger)
        else:
            self.__logger.debug("Launching command : {}".format(' '.join(cmd)))
            launcher = SubprocessLoggerThread(cmd, self.__consoleLogger)

        launcher.start()

        if not isAsynchronous:
            # if the process is synchronous, we wait him before continuing
            launcher.join()

