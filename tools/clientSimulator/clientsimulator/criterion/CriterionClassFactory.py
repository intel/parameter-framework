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
from clientsimulator.criterion.ExclusiveCriterion import ExclusiveCriterion
from clientsimulator.criterion.InclusiveCriterion import InclusiveCriterion


class CriterionClassFactory:

    def __init__(self, criteriaFileName):
        self.__criteriaFileName = criteriaFileName

    def __classFactory(self, name, allowedValues, base):
        """
            Private function which allows to dynamically create a new Criterion SubClass

            :param name: name of the futur subclass
            :type name: string
            :param allowedValues: all values the criterion can take
            :type allowedValues: string list
            :param base: direct mother class of the created criterion subclass
                         should be InclusiveCriterion or ExclusiveCriterion
            :type base: type

            :return: A criterion subclass
            :rtype: type
        """

        def __init__(self):
            """ Init Function of a Criterion Child Class """
            base.__init__(self)

        # Creation of the class with allowed values
        # (allows to have only one instance of the list)
        return type(
            name, (base,), {
                "__init__": __init__, "_allowedValues": allowedValues})

    def generateCriterionClasses(self):
        """ Function invoqued to generate Criterion Childs classes from an AudioCriteria file"""

        # Parsing criterions File
        with open(self.__criteriaFileName, "r") as criteria:
            criteriaLines = criteria.readlines()

        parsedCriteria = [(typeAndName.split(), allowedValues.split())
                          for typeAndName, allowedValues
                          in (line.split(':') for line in criteriaLines)]

        generatedClasses = {}
        # Creation of needed classes thanks to criteria File information,
        # such as BaseClass, criterion name and possible values
        generatedClasses = [
            self.__classFactory(className, allowedValues, globals()[classBase])
            for (classBase, className), allowedValues in parsedCriteria
        ]

        return generatedClasses
