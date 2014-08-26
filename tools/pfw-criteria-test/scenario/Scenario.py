#
# INTEL CONFIDENTIAL
# Copyright  2014 Intel
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

import json
import logging
import os


class Scenario:

    """
        Class which can handle several TestVectors and script
        to play a complete scenario.
    """

    def __init__(self,
                 consoleLogger,
                 scenarioFileName,
                 actionGathererFileName,
                 testFactory,
                 testLauncher):
        """
            Init function

            :param consoleLogger: console log handler
            :type consoleLogger: Handler
            :param scenarioFileName: name of file containing scenario description
            :type scenarioFileName: string
            :param actionGathererFileName: conf file which allows to reduce action repetition
            :type actionGathererFileName: string
            :param testFactory: the factory used to generate tests from setCriterion actions
            :type testFactory: TestVectorFactory
            :param testLauncher: object used to execute actions from scenarios
            :type testLauncher: TestLauncher
        """
        self.__logger = logging.getLogger(__name__)
        self.__logger.addHandler(consoleLogger)

        self.__testFactory = testFactory
        self.__testLauncher = testLauncher

        # Simplify the way to get an action behaviour
        # Python way to replace switch statement but keeping the possibility
        # to get keys (usefull in __parseScenarioActions)
        self.__actionTypeBehaviour = {
            "setCriterion":
                lambda rawCriterions:
                    self.__testLauncher.executeTestVector(
                        self.__testFactory.generateTestVector(rawCriterions)),
            "script":
                self.__testLauncher.executeScript
        }

        self.__scenarioActions = self.__parseScenarioActions(
            scenarioFileName,
            actionGathererFileName)

    def __parseScenarioActions(self, scenarioFileName, actionGathererFileName):
        """
            Parse actions from a scenario.
            Convert user-defined actions in system-known actions.

            :param scenarioFileName: name of file containing scenario description
            :type scenarioFileName: string
            :param actionGathererFileName: conf file which allows to reduce action repetition
            :type actionGathererFileName: string

            :return: parsed scenario's actions with system-known types
            :rtype: dict
        """

        # Parsing of Json test file
        with open(scenarioFileName, "r") as scenarioFile:
            scenarioActions = json.load(scenarioFile)

        # Parsing the action Gatherer file which allows defining new
        # actions types
        with open(actionGathererFileName, "r") as actionGathererFile:
            scenarioGatheredActions = json.load(actionGathererFile)

        for action in scenarioActions:
            try:
                actionDefinedType = self.__getActionType(action)
                if actionDefinedType not in self.__actionTypeBehaviour.keys():
                    actionValue = action.pop(actionDefinedType)
                    actionGatherer = scenarioGatheredActions[actionDefinedType]

                    if self.__getActionType(actionGatherer) == "script":
                        raise UngatherableTypeException(
                            "Unable to redefine {} type, please edit your {} file".format(
                                self.__getActionType(actionGatherer),
                                actionGathererFileName))

                    # Fusion of gathered Actions and other desired actions which
                    # are directly writed in the scenario's file
                    actionValue.update(
                        self.__getActionValue(actionGatherer))

                    # Change the user defined key which was previously popped
                    # by the known one
                    action[self.__getActionType(actionGatherer)] = actionValue
            except KeyError as e:
                self.__logger.error(
                    "Actions {} from {} file is not valid".format(
                        actionDefinedType,
                        scenarioFileName))
                raise e

        return scenarioActions

    def __getActionType(self, action):
        """
            Return the type of an action (the key)
            An action is a dictionary with only one element

            :param action: the action you want to get the type
            :type action: dict

            :return: the type of the desired action
            :rtype: string
        """
        return list(action.keys())[0]

    def __getActionValue(self, action):
        """
            Return the Value of an action
            An action is a dictionary with only one element

            :param action: the action you want to get the type
            :type action: dict

            :return: the value of the desired action
            :rtype: string or dict
        """
        return list(action.values())[0]

    def play(self):
        """
            Execute a Scenario
        """

        for action in self.__scenarioActions:
            # Launch the adequate behaviour depending on the key of the action dict
            # No need to try KeyError as it would have been raised during init
            # process
            self.__actionTypeBehaviour[self.__getActionType(action)](
                self.__getActionValue(action))


class UngatherableTypeException(Exception):

    """
        Exception raised in case of problem with a type that the
        user try to personalize
    """

    def __init__(self, msg):
        self.__msg = msg

    def __str__(self):
        return "Ungatherable type Error : " + self.__msg
