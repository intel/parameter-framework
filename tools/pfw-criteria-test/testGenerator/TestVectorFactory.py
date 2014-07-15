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

from testGenerator.TestVector import TestVector
import json
import logging
import os

class TestVectorFactory:

    def __init__(self, criterionClasses, routingCriterionName, testTypeFile, consoleLogger):
        self.__criterionClasses = criterionClasses
        self.__routingCriterionName = routingCriterionName
        self.__logger = logging.getLogger(__name__)
        self.__logger.addHandler(consoleLogger)

        #Loading possible types and behavior
        with open(testTypeFile,'r') as typeFile:
            self.__testTypes = json.load(typeFile)

        #Setting up a command to launch the script
        for testType, (script,isSynchronous) in self.__testTypes.items():
            self.__testTypes[testType] = ("{}/{}".format(
                                            os.path.split(testTypeFile)[0],
                                            script),isSynchronous)

    def generateTestVector(self, testFileName):
        """ Function invoqued to generate TestVector object from a Json file """

        criterions = []

        if testFileName:
            # Parsing of Json test file
            with open(testFileName, "r") as testFile:
                #handle exception if error in the json file  ValueError ?
                testList = json.load(testFile)

            name = testList[0]
            testType = testList[1]

            if testType not in self.__testTypes:
                raise InvalidTestTypeValueException(
                        "The value {} of the test {} is invalid".format(testType,
                                                                        name))

            rawCriteria = testList[2]

            for criterionClass in self.__criterionClasses.values():
                # Instanciate the criterion class requested
                newCriterion = criterionClass()
                if criterionClass.__name__ != self.__routingCriterionName:
                    try:
                        newCriterion.currentValue = rawCriteria[criterionClass.__name__]
                    except KeyError as e:
                        self.__logger.warning(
                                "Warning {}: Missing Criterion {}, default value applied".format(
                                                                        e,criterionClass.__name__))
                        newCriterion.currentValue = newCriterion.noValue
                    criterions.append(newCriterion)
        else:
            # if no test or init file has been provided, use default values
            testType="none"
            name="generatedDefault"
            for criterionClass in self.__criterionClasses.values():
                # Instanciate the criterion class requested
                newCriterion = criterionClass()
                # Useful to avoid warning when no explicit init file is provided
                if criterionClass.__name__ != self.__routingCriterionName:
                    newCriterion.currentValue = newCriterion.noValue
                criterions.append(newCriterion)

        return TestVector(name, criterions, testType)


    @property
    def testTypes(self):
        return self.__testTypes
