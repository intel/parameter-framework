#
# INTEL CONFIDENTIAL
# Copyright  2014 Intel
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

class DynamicCallHelper:
    """
        Callable object which wrap a function call.

        This simple class is designed to avoid the use of lambda
        hack to capture arguments when generating function call.
        Indeed, to generate a call to a function which take
        parameters at runtime, we need to force the copy by capture
        of the parameter by using default value such as :

            callerDict = {
                a: lambda a=a : print(a)
                for a in range(5)
                }

        With the DynamicCallHelper you can simply use :

            callerDict = {
                DynamicCallHelper(print, a)
                for a in range(5)
                }
    """

    def __init__(self, func, *args):
        """
            Init method

            :param func: the function that will be called when calling the object
            :type func: function
            :param *args: arguments destinated to func
            :type *args: list
        """

        self.__func = func
        self.__args = args

    def __call__(self):
        """
            Call the function __func with __args as arguments

            :return: __func return
        """

        return self.__func(*self.__args)

