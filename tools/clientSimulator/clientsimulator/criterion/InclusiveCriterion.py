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

from clientsimulator.criterion.Criterion import Criterion
from clientsimulator.criterion.Criterion import InvalidCriterionException


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
