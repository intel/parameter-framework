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


class Criterion:

    """ A simple class that defines common criterions attributes """

    @classmethod
    def allowedValues(cls):
        """
            The getter of the allowedValues attribute

            :return: allowedValues attribute : The lexical states list the
            criterion can take.
            :rtype: string list
        """

        # _allowedValues variable is created in CriterionClassFactory
        # when creating dynamic child classes
        return cls._allowedValues

    @property
    def noValue(self):
        return '0'


class InvalidCriterionException(Exception):

    """ Exception raised in case of problem with a criterion """

    def __init__(self, msg):
        self.__msg = msg

    def __str__(self):
        return "Invalid Criterion Error : " + self.__msg
