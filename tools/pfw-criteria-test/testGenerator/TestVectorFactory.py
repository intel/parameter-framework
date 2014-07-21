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

import logging

class TestVectorFactory:
    """
        Factory of Test Vectors Objects
    """

    def __init__(self, criterionClasses, consoleLogger):
        """
            Init function

            :param criterionClasses: available criterion types
            :type criterionClasses: list of classes
            :param consoleLogger: console log handler
            :type consoleLogger: Handler
        """
        self.__criterionClasses = criterionClasses
        self.__logger = logging.getLogger(__name__)
        self.__logger.addHandler(consoleLogger)


    def generateTestVector(self, rawCriterions=None):
        """
            Function invoqued to generate TestVector object

            :param rawCriterions: the desired criterions state descriptions
            :type rawCriterions: dictionnary
        """

        criterions = []

        for criterionClass in self.__criterionClasses:
            # Instanciate the criterion class requested
            newCriterion = criterionClass()

            if rawCriterions:
                try:
                    newCriterion.currentValue = rawCriterions[criterionClass.__name__]
                    criterions.append(newCriterion)
                except KeyError as e:
                    self.__logger.debug(
                            "Missing Criterion {}, old value keeped".format(criterionClass.__name__))
            else:
                # if rawCriterions is None, we create an empty criterions lists
                newCriterion.currentValue = newCriterion.noValue
                criterions.append(newCriterion)
        return criterions

