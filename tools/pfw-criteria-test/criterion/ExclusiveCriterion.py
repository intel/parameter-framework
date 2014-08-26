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


class ExclusiveCriterion(Criterion):

    """
    This file describe Exclusive Criterion Behavior

    This types of criterion can only have one value at a time
    """

    def __init__(self):
        super().__init__()
        self.__currentValue = None

    @property
    def currentValue(self):
        return self.__currentValue

    @currentValue.setter
    def currentValue(self, currentValue):
        if currentValue in self._allowedValues or currentValue == self.noValue:
            self.__currentValue = currentValue
        else:
            raise InvalidCriterionException(
                "Value {} is not allowed for {}.".format(
                    currentValue, self.__class__.__name__))

    def __str__(self):
        return self.__class__.__name__ + ' : ' + str(self.__currentValue)
