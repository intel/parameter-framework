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
from criterion.ExclusiveCriterion import ExclusiveCriterion
from criterion.InclusiveCriterion import InclusiveCriterion
import os

class CriterionClassFactory:

    def __init__(self, criteriaFileName):
        self.__criteriaFileName = criteriaFileName


    def __classFactory(self,name, values, Base):
        """ Private function which allows to create dynamically a new Criterion SubClasse """

        def __init__(self):
            """ Init Function of a Criterion Child Function, allowedValues are fixed """
            Base.__init__(self)

        # Creation of the class with allowed values (allows to have only one instance of the list)
        return type(name,(Base,),{"__init__":__init__, "_allowedValues":values})


    def generateCriteriaClasses(self):
        """ Function invoqued to generate Criterion Childs classes from an AudioCriteria file"""
        # Parsing of criterias
        with open(self.__criteriaFileName, "r") as audioCriteria:
            criteriaLines = audioCriteria.readlines()

        parsedCriteria = [(typeAndName.split(), allowedValues.split())
                          for typeAndName, allowedValues
                          in (line.split(':') for line in criteriaLines)]

        generatedClasses = {}
        # Creation of needed classes
        generatedClasses = {
            className: self.__classFactory(className,allowedValues,globals()[classBase])
            for (classBase,className), allowedValues in parsedCriteria
        }

        return generatedClasses

