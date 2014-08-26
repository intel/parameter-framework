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


class TestVector:

    """ This class describe a test vector which can be launched by a TestLauncher object """

    def __init__(self, name, criterions, testType):
        self.__name = name
        self.__testType = testType
        self.__criterions = criterions

    @property
    def criterions(self):
        return self.__criterions

    @property
    def testType(self):
        return self.__testType

    @testType.setter
    def testType(self, value):
        self.__testType = value

    def __str__(self):
        toString = "Test Type : {}\n".format(self.__testType)
        for criterion in self.__criterions:
            toString += (str(criterion) + '\n')
        return toString


class InvalidTestTypeValueException(Exception):

    """ Exception raised in case of problem with the test type """

    def __init__(self, msg):
        self.__msg = msg

    def __str__(self):
        return "Invalid Test Type Error : " + self.__msg
