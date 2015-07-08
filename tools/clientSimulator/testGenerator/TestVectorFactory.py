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
                    newCriterion.currentValue = rawCriterions[
                        criterionClass.__name__]
                    criterions.append(newCriterion)
                except KeyError as e:
                    self.__logger.debug(
                        "Missing Criterion {}, old value keeped".format(
                            criterionClass.__name__))
            else:
                # if rawCriterions is None, we create an empty criterions lists
                newCriterion.currentValue = newCriterion.noValue
                criterions.append(newCriterion)
        return criterions
