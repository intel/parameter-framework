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

from criterion.Criterion import Criterion
from criterion.Criterion import InvalidCriterionException


class InclusiveCriterion(Criterion):

    """
    This file describe Inclusive Criterion Behavior

    This types of criterion can have several values at the same time
    """

    def __init__(self):
        super().__init__()
        self.__currentValues = set()

    @property
    def currentValue(self):
        return self.__currentValues

    @currentValue.setter
    def currentValue(self, stringNewVal):
        """
            Criterion current value setter

            :param stringNewVal: the string containing desired values
            separated by a space
            :type stringNewVal: string
        """

        # TODO: Avoid string splitting by using a list as parameter
        # Warning, we have chosen this solution for now because
        # it avoids TestVectorFactory to test criterions type before
        # to use this setter. Indeed, ExclusiveCriterion setter has
        # exactly the same prototype.
        valueList = stringNewVal.split()

        if self.noValue in valueList:
            # if we put default value, we empty the list first
            self.__currentValues = set()
            self.__currentValues.add(self.noValue)
        else:
            # We set a value, we have to remove default is it's set
            if self.noValue in self.__currentValues:
                self.__currentValues.remove(self.noValue)

            for currentValue in valueList:
                if currentValue in self._allowedValues \
                        and currentValue != self.noValue:
                    self.__currentValues.add(currentValue)
                else:
                    raise InvalidCriterionException(
                        "Value {} is not allowed for {}.".format(
                            currentValue, self.__class__.__name__))

    def removeValue(self, currentValue):
        if currentValue in self.__currentValues:
            self.__currentValues.remove(currentValue)
        else:
            raise InvalidCriterionException(
                "Value {} is not currently setted.".format(currentValue))
        # If there is no value, assume it's default
        if not self.__currentValues:
            self.__currentValues.append(self.noValue)

    def __str__(self):
        return self.__class__.__name__ + ' : ' + str(self.__currentValues)
