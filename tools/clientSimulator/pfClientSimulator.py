#!/usr/bin/python3
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

from clientsimulator.criterion.CriterionClassFactory import CriterionClassFactory
from clientsimulator.testGenerator.TestVectorFactory import TestVectorFactory
from clientsimulator.testGenerator.TestLauncher import TestLauncher
from clientsimulator.testGenerator.SubprocessLogger import SubprocessLoggerThread
from clientsimulator.testGenerator.SubprocessLogger import ScriptLoggerThread
from clientsimulator.configuration.ConfigParser import ConfigParser
from clientsimulator.scenario.Scenario import Scenario
from clientsimulator.userInteraction.UserInteractor import UserInteractor
from clientsimulator.userInteraction.DynamicCallHelper import DynamicCallHelper
import argparse
import time
import logging
import os


def close(logger, testLauncher, coverage):
    """ SIGINT Handler which clean up processes  """

    # Check if some scripts are running, if this is the case
    # we warn the user.
    if ScriptLoggerThread.getRunningInstances():
        try:
            logger.info("{} \n {}".format(
                "Some subprocesses are still running. The program will wait them before exiting.",
                "If you really want to exit, please confirm by typing Ctrl+C again"))

            # Wait for thread to terminate
            while ScriptLoggerThread.getRunningInstances():
                time.sleep(1)
        except KeyboardInterrupt as e:
            pass

    # Kill subprocess (at least test-platform one)
    SubprocessLoggerThread.closeAll()

    if coverage:
        testLauncher.generateCoverage()

    logger.info("Closing")

    exit(0)


def launchScenario(
        logger,
        consoleLogger,
        actionGathererFileName,
        scenarioFileName,
        testFactory,
        testLauncher):

    logger.info("Launching {}".format(scenarioFileName))

    Scenario(consoleLogger,
             scenarioFileName,
             actionGathererFileName,
             testFactory,
             testLauncher).play()

    logger.info("Scenario execution complete.")


def main():

    # Handle Arguments

    parser = argparse.ArgumentParser()

    parser.add_argument("test_directory", type=str, default=None,
                        help="precise a test directory (required).")

    parser.add_argument("-s", "--scenario", type=int, default=None, nargs='+',
                        help="precise one or more scenarios to launch.")

    interactiveness = parser.add_mutually_exclusive_group()
    interactiveness.add_argument("--no-exit", action='store_true',
                                 help="lets you interactively select more scenarios (This is"
                                      " implicit if neither '--scenario' nor '--interactive' are "
                                      " passed).")

    interactiveness.add_argument("--interactive", action='store_true',
                                 help="run in interactive mode (lets you select actions and scripts"
                                 " to run).")

    parser.add_argument(
        "-v",
        "--verbose",
        action='store_true',
        help="display test-platform's and scripts' log on stdout.")

    parser.add_argument("-c", "--coverage", action='store_true',
                        help="generate coverage file at end of script")

    args = parser.parse_args()

    # Logging Configuration
    logger = logging.getLogger(__name__)

    # Decide what to write in console depending on verbose argument
    consoleLogger = logging.StreamHandler()
    if args.verbose:
        consoleLogger.setLevel(logging.DEBUG)
    else:
        consoleLogger.setLevel(logging.INFO)
    logger.addHandler(consoleLogger)

    # The given directory should have a conf.json file
    if not os.path.isfile(os.path.join(args.test_directory, "conf.json")):
        # This error will only be logged in the terminal
        logger.error(
            "Cannot find configuration file : conf.json in {} directory.".format(
                args.test_directory))
        exit(1)

    try:
        configParser = ConfigParser(
            os.path.join(
                args.test_directory,
                "conf.json"),
            args.test_directory,
            consoleLogger)
    except KeyError as e:
        logger.error(
            "Missing mandatory configuration item {} in the"
            " conf.json file".format(e))
        exit(1)

    # Always write all log in the file
    logging.basicConfig(level=logging.DEBUG,
                        format='%(name)-12s %(levelname)-8s %(message)s',
                        filename=configParser["LogFile"],
                        filemode='w')

    # Parsing criterion file and classes generation
    logger.info("Criterion analysis")
    classFactory = CriterionClassFactory(configParser["CriterionFile"])
    criterionClasses = classFactory.generateCriterionClasses()

    # Tests Handlers Generation
    testFactory = TestVectorFactory(
        criterionClasses,
        consoleLogger)

    testLauncher = TestLauncher(
        criterionClasses,
        configParser,
        consoleLogger)

    # Initialisation
    testLauncher.init(criterionClasses, args.verbose)

    # Launching
    try:
        if args.interactive:
            # Launch Interactive Mode with default criterions values
            UserInteractor(
                testLauncher,
                testFactory.generateTestVector()).launchInteractiveMode()
        else:
            scenarioOptions = [
                    (scenarioFileName,
                     DynamicCallHelper(
                         launchScenario,
                         logger,
                         consoleLogger,
                         configParser["ActionGathererFile"],
                         os.path.join(
                             configParser["ScenariosDirectory"], scenarioFileName),
                         testFactory,
                         testLauncher
                     ))
                for scenarioFileName in sorted(os.listdir(configParser["ScenariosDirectory"]))
            ]
            if args.scenario is not None:
                for elem in args.scenario:
                    scenarioOptions[elem][1]()
            if (args.scenario is None) or args.no_exit:
                # Let the user choose more scenarios after the ones chosen by command line
                # or if none was given on the command line.
                UserInteractor.getMenu(scenarioOptions, "Quit")
    except KeyboardInterrupt as e:
        close(logger, testLauncher, args.coverage)
    else:
        close(logger, testLauncher, args.coverage)


if __name__ == "__main__":
    # Execute main if the script is running as main
    main()
