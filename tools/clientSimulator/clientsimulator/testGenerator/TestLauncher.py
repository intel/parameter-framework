# Copyright (c) 2014-2015, Intel Corporation
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

from clientsimulator.criterion.ExclusiveCriterion import ExclusiveCriterion
from clientsimulator.configuration.ConfigParser import ConfigParser
from clientsimulator.testGenerator.SubprocessLogger import SubprocessLoggerThread
from clientsimulator.testGenerator.SubprocessLogger import ScriptLoggerThread
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
        halCommand = ["remote-process",
                      "localhost",
                      configParser["TestPlatformPort"]]
        setCriteriaCommand = halCommand + ["setCriterionState"]
        testPlatformHostCommand = ["remote-process",
                                   "localhost",
                                   configParser["TestPlatformPort"]]

        self.__logFileName = configParser["LogFile"]

        # Commands
        self.__startTestPlatformCmd = [configParser["PrefixCommand"],
                                       "test-platform",
                                       configParser["PfwConfFile"],
                                       configParser["TestPlatformPort"]]

        self.__createCriterionCmd = [configParser["PrefixCommand"]]
        self.__createCriterionCmd.extend(testPlatformHostCommand)

        self.__startPseudoHALCmd = [configParser["PrefixCommand"]]
        self.__startPseudoHALCmd.extend(testPlatformHostCommand)
        self.__startPseudoHALCmd.append("start")

        self.__setCriterionCmd = [configParser["PrefixCommand"]]
        self.__setCriterionCmd.extend(setCriteriaCommand)

        self.__applyConfigurationsCmd = [configParser["PrefixCommand"]]
        self.__applyConfigurationsCmd.extend(halCommand)
        self.__applyConfigurationsCmd.append("applyConfigurations")

        # Command used to generate coverage
        self.__coverageCmd = [
            "eval",
            os.path.join(configParser["CoverageDir"], "aplog2coverage.sh"),
            "-d",
            configParser["PfwDomainConfFile"],
            "-e.",
            self.__logFileName,
            "-f",
            "-o",
            configParser["CoverageFile"]
        ]

        # Prepare script Commands
        # Loading possible scripts
        self.__rawScripts = {}
        if configParser["ScriptsFile"]:
            with open(configParser["ScriptsFile"], 'r') as scriptFile:
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

        self.__logger.info("Pseudo Hal Initialisation")
        # Test platform is launched asynchronously and not as script
        self.__call_process(self.__startTestPlatformCmd, True)
        # wait Initialisation
        time.sleep(1)

        for criterionClass in criterionClasses:
            if ExclusiveCriterion in criterionClass.__bases__:
                createSlctCriterionCmd = "createExclusiveSelectionCriterionFromStateList"
            else:
                createSlctCriterionCmd = "createInclusiveSelectionCriterionFromStateList"

            createCriterionArgs = [
                createSlctCriterionCmd,
                criterionClass.__name__] + criterionClass.allowedValues()

            self.__call_process(
                self.__createCriterionCmd + createCriterionArgs)

        self.__call_process(self.__startPseudoHALCmd)

    def executeTestVector(self, criterions):
        """ Launch the Test """
        for criterion in criterions:
            if ExclusiveCriterion in criterion.__class__.__bases__:
                criterionValue = [criterion.currentValue]
            else:
                criterionValue = criterion.currentValue
            # If no value given, we add "" to the command to set the default state
            criterionValueArg = list(criterionValue) if list(criterionValue) else ["\"\""]
            setCriterionArgs = [criterion.__class__.__name__] + criterionValueArg
            self.__call_process(self.__setCriterionCmd + setCriterionArgs)

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
        # A script's path is absolute or relative to the "ScriptsFile" file.
        self.__call_process(
            ["eval", os.path.join(
                os.path.dirname(self.__configParser["ScriptsFile"]),
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
