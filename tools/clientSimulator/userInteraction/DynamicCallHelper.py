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
