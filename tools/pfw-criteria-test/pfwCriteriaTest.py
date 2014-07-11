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
from criterion.InclusiveCriterion import InclusiveCriterion
from testGenerator.TestVectorFactory import TestVectorFactory
from testGenerator.TestLauncher import TestLauncher
from configuration.ConfigParser import ConfigParser
import argparse
import signal
import time
import logging

def close(logger,launcher):
    """ SIGINT Handler which clean up processes  """
    logger.info("Closing")
    exit(0)

def test_mode(testFactory, launcher, testFile, logger):
    """ Simple Test Launcher wait for SIGINT when Done """
    logger.info("Test Initialisation")
    testVector = testFactory.generateTestVector(testFile)

    launcher.execute(testVector)

    print("Type Ctrl+C to quit")

    while True:
        time.sleep(1)

def interactive_mode(launcher, testVector, logger):
    """
        Interactive Mode : Set up a menu which allow
        users to personnalize a Test and to Launch it
    """

    def menu(options,testQuit):
        """
            Dynamic Menu Generator :

            :param options: dictionnary containing, the invite string
            and the function to launch
            :param testQuit: Boolean for quitting the current Menu
        """
        options[len(options)] = \
            ("Go Back", lambda : False)
        while testQuit :
            print("\nCurrent Test state : \n")
            print(testVector)
            print("\nPlease Make a choice : ")
            for numMenu,(sentenceMenu, fonc) in sorted(options.items()):
                print("\t{}. {}".format(numMenu,sentenceMenu))

            choice = input("Your Choice : ")

            try:
                testQuit = options[int(choice)][1]()
            except (KeyError, ValueError) as e:
                logger.error("Invalid Choice")


    def applyConfiguration():
        launcher.execute(testVector)
        return True

    def editType():

        def setType(testType):
            testVector.testType = testType

        optionEditType = {}

        for num,testType in enumerate(testFactory.testTypes):
            optionEditType[num] = ("{} Type".format(testType),
                    lambda mType=testType : setType(mType))

        menu(optionEditType,True)
        return True

    def editCriterion(criterion):
        def setCriterion(value):
            criterion.currentValue = value

        def removeCriterionValue(value):
            criterion.removeValue(value)

        optionEditCriterion = {}
        for possibleValue in  [x for x in criterion.allowedValues
                       if not x in criterion.currentValue
                       and not x == criterion.noValue]:
            optionEditCriterion[len(optionEditCriterion)] = \
                ("Set {}".format(possibleValue),
                 lambda value=possibleValue: setCriterion(value))

        if InclusiveCriterion in criterion.__class__.__bases__:
            #Inclusive criterion : propose unset value (default when empty)
            for possibleValue in criterion.currentValue:
                optionEditCriterion[len(optionEditCriterion)] = \
                    ("Unset {}".format(possibleValue),
                     lambda value=possibleValue: removeCriterionValue(value))
        else:
            #Exclusive criterion : propose default value
            optionEditCriterion[len(optionEditCriterion)] = \
                ("Set Default",
                 lambda value=criterion.noValue: setCriterion(value))

        menu(optionEditCriterion,True)

        return True

    def editVector():
        optionEdit = {
            0:("Edit Type",editType),
            }
        for cri in testVector.criterions:
            optionEdit[len(optionEdit)] = \
                ("Edit {}".format(cri.__class__.__name__),
                 lambda cri=cri: editCriterion(cri))
                             #capture by copy

        menu(optionEdit,True)
        return True

    optionsMenu = {
        0:("Edit Vector",editVector),
        1:("Apply Configuration",applyConfiguration),
        }

    menu(optionsMenu,True)

    print("Type Ctrl+C to quit")

    while True:
        time.sleep(1)


def main():

    ### Handle Arguments

    parser = argparse.ArgumentParser()

    parser.add_argument("-t", "--test-file", type=str, default=None,
                        help="precise a test to launch (required)")

    parser.add_argument("-s", "--script-test-file", type=str, default=None,
                        help="precise a test type file (required)")

    parser.add_argument("-i", "--init-file", type=str, default=None,
                        help="precise an init file")

    parser.add_argument("--interactive", action='store_true',
                        help="run in interactive mode")

    parser.add_argument("-n", "--no-init", action='store_true',
                        help="do not create a test-platform instance")

    parser.add_argument("-v","--verbose", action='store_true',
                        help="choice if TestPlatform log are in stdout or in the log file")

    parser.add_argument("-c", "--config-file", type=str,
                        default="configuration/default_configuration.json",
                        help="precise a command platform file (default : android_local)")

    args = parser.parse_args()

    if args.init_file == None:
        logger.error("An init file is required, please provide one with -i option")
        exit(1)

    if args.script_test_file == None:
        logger.error("A test type file is required, please provide one with -s option")
        exit(1)

    if args.test_file==None and not args.interactive:
        logger.error(
                "A test file is required in not interactive, please provide one with -t option")
        exit(1)

    ## Logging Configuration

    logger = logging.getLogger(__name__)

    # Decide what to write in console depending on verbose argument
    partialLogHandler = logging.StreamHandler()
    if args.verbose:
        partialLogHandler.setLevel(logging.DEBUG)
    else:
        partialLogHandler.setLevel(logging.INFO)
    logger.addHandler(partialLogHandler)

    configParser=ConfigParser(args.config_file,partialLogHandler)

    # Always write all log in the file
    logging.basicConfig(level=logging.DEBUG,
                        format='%(name)-12s %(levelname)-8s %(message)s',
                        filename=configParser["PFWtest_LogFile"],
                        filemode='w')

    ### Parsing of the Criterion File and Generation of classes
    logger.info("Criterion analysis")
    classFactory = CriterionClassFactory(configParser["PFWtest_CriterionFile"])
    criterionClasses = classFactory.generateCriteriaClasses()

    ### Handle Tests
    testFactory = TestVectorFactory(
            criterionClasses,
            configParser["PFWtest_RouteStateCriterionName"],
            args.script_test_file,
            partialLogHandler)

    ### Initialisation
    launcher = TestLauncher(
            criterionClasses,
            configParser,
            testFactory.testTypes,
            partialLogHandler)

    testVectorDefault = testFactory.generateTestVector(args.init_file)

    if not args.no_init:
        launcher.init(testVectorDefault,args.verbose)
    try:
        if args.interactive:
            interactive_mode(launcher, testVectorDefault, logger)
        else:
            test_mode(testFactory, launcher, args.test_file, logger)
    except KeyboardInterrupt as e:
        close(logger,launcher)


if __name__ == "__main__":
    """ Execute main if the script is running as main  """
    main()

