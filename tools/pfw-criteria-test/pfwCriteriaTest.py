#!/usr/bin/python3
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

from criterion.CriterionClassFactory import CriterionClassFactory
from testGenerator.TestVectorFactory import TestVectorFactory
from testGenerator.TestLauncher import TestLauncher
from configuration.ConfigParser import ConfigParser
from scenario.Scenario import Scenario
from userInteraction.UserInteractor import UserInteractor
from userInteraction.DynamicCallHelper import DynamicCallHelper
import argparse
import signal
import time
import logging
import os

def close(logger):
    """ SIGINT Handler which clean up processes  """
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

    print("Type Ctrl+C to quit")

    while True:
        time.sleep(1)


def main():

    ## Handle Arguments

    parser = argparse.ArgumentParser()

    parser.add_argument("test_directory", type=str, default=None,
                        help="precise a test directory (required).")

    parser.add_argument("-s", "--scenario", type=int, default=None,
                        help="precise a scenario to launch.")

    parser.add_argument("--interactive", action='store_true',
                        help="run in interactive mode.")

    parser.add_argument("-v","--verbose", action='store_true',
                        help="display test-platform's and scripts' log on stdout.")

    args = parser.parse_args()

    ## Logging Configuration
    logger = logging.getLogger(__name__)

    # Decide what to write in console depending on verbose argument
    consoleLogger = logging.StreamHandler()
    if args.verbose:
        consoleLogger.setLevel(logging.DEBUG)
    else:
        consoleLogger.setLevel(logging.INFO)
    logger.addHandler(consoleLogger)

    # The given directory should have a conf.json file
    if not os.path.isfile(os.path.join(args.test_directory,"conf.json")):
        # This error will only be logged in the terminal
        logger.error(
                "Cannot find configuration file : conf.json in {} directory.".format(
                    args.test_directory))
        exit(1)

    configParser=ConfigParser(
            os.path.join(args.test_directory,"conf.json"),args.test_directory,consoleLogger)

    # Always write all log in the file
    logging.basicConfig(level=logging.DEBUG,
                        format='%(name)-12s %(levelname)-8s %(message)s',
                        filename=configParser["LogFile"],
                        filemode='w')

    ## Parsing criterion file and classes generation
    logger.info("Criterion analysis")
    classFactory = CriterionClassFactory(configParser["CriterionFile"])
    criterionClasses = classFactory.generateCriterionClasses()

    ## Tests Handlers Generation
    testFactory = TestVectorFactory(
            criterionClasses,
            consoleLogger)

    testLauncher = TestLauncher(
            criterionClasses,
            configParser,
            consoleLogger)

    ## Initialisation
    testLauncher.init(criterionClasses,args.verbose)

    ## Launching
    try:
        if args.interactive:
            # Launch Interactive Mode with default criterions values
            UserInteractor(testLauncher, testFactory.generateTestVector()).launchInteractiveMode()
        else:
            scenarioOptions = {
                    scenarioNumber : (scenarioFileName,
                        DynamicCallHelper(
                            launchScenario,
                            logger,
                            consoleLogger,
                            configParser["ActionGathererFile"],
                            os.path.join(configParser["ScenariosDirectory"],scenarioFileName),
                            testFactory,
                            testLauncher
                        ))
                    for scenarioNumber, scenarioFileName in enumerate(
                            [file for file in sorted(os.listdir(
                            configParser["ScenariosDirectory"]))])
                }
            if args.scenario != None:
                scenarioOptions[args.scenario][1]()
            else:
                UserInteractor.getMenu(scenarioOptions)
    except KeyboardInterrupt as e:
        close(logger)


if __name__ == "__main__":
    """ Execute main if the script is running as main  """
    main()

