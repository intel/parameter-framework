#!/usr/bin/python2.7
#
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

import PyPfw

import logging
from decimal import Decimal
from math import log10

class PfwLogger(PyPfw.ILogger):
    def __init__(self):
        super(PfwLogger, self).__init__()
        self.__logger = logging.root.getChild("parameter-framework")

    def log(self, is_warning, message):
        log_func = self.__logger.warning if is_warning else self.__logger.info
        log_func(message)

class FixedPointTester():
    """ Made for testing a particular Qn.m number

    As a convention, we use:
        * n is the fractional part
        * m is the integral part

    This class computes several specific numbers for a given Qn.m number.

    For each of those numbers, we run 4 checks:
        * Bound check
        * Sanity check
        * Consistency check
        * Bijectivity check
    Which are documented below.
    """
    def __init__(self, pfwClient, size, integral, fractional):
        self._pfwClient = pfwClient
        self._paramPath = '/Test/test/%d/q%d.%d' % (size, integral, fractional)

        # quantum is the step we have between two numbers
        # encoded in Qn.m format
        self._quantum = 2 ** -fractional

        # The maximum value we can encode for a given Qn.m.
        # Since we also need to encode the 0, we have one quantum missing on
        # the positive maximum
        self._upperAllowedBound = (2 ** integral) - self._quantum

        # The minimum value that we can encode for a given Qn.m.
        # This one does not need a quantum substraction since we already did
        # that on the maximum
        self._lowerAllowedBound = -(2 ** integral)

        self._shouldWork = [
                Decimal(0),
                Decimal(self._lowerAllowedBound),
                Decimal(self._upperAllowedBound)
                ]

        # bigValue is to be sure a value far out of range is refused
        bigValue = (2 * self._quantum)
        # little is to be sure a value just out of range is refused
        littleValue = 10 ** -(int(fractional * log10(2)))
        self._shouldBreak = [
                Decimal(self._lowerAllowedBound) - Decimal(bigValue),
                Decimal(self._upperAllowedBound) + Decimal(bigValue),
                Decimal(self._lowerAllowedBound) - Decimal(littleValue),
                Decimal(self._upperAllowedBound) + Decimal(littleValue)
                ]

        self._chainingTests = [
                ('Bound', self.checkBounds),
                ('Sanity', self.checkSanity),
                ('Consistency', self.checkConsistency),
                ('Bijectivity', self.checkBijectivity)]


    def run(self):
        """ Runs the test suite for a given Qn.m number
        """

        runSuccess = True

        for value in self._shouldWork:
            value = value.normalize()
            print('Testing %s for %s' % (value, self._paramPath))

            for testName, testFunc in self._chainingTests:
                value, success = testFunc(value)
                if not success:
                    runSuccess = False
                    print("%s ERROR for %s" % (testName, self._paramPath))
                    break

        for value in self._shouldBreak:
            value = value.normalize()
            print('Testing invalid value %s for %s' % (value, self._paramPath))
            value, success = self.checkBounds(value)
            if success:
                runSuccess = False
                print("ERROR: This test should have failed but it has not")

        return runSuccess

    def checkBounds(self, valueToSet):
        """ Checks if we are able to set valueToSet via the parameter-framework

        valueToSet -- the value we are trying to set

        returns: the value we are trying to set
        returns: True if we are able to set, False otherwise
        """
        (success, errorMsg) = self._pfwClient.set(self._paramPath, str(valueToSet))

        return valueToSet, success


    def checkSanity(self, valuePreviouslySet):
        """ Checks if the value we get is still approximately the same
        as we attempted to set. The value can have a slight round error which
        is tolerated.

        valuePreviouslySet -- the value we had previously set

        returns: the value the parameter-framework returns us after the get
        returns: True if we are able to set, False otherwise
        """
        firstGet = self._pfwClient.get(self._paramPath)

        try:
            returnValue = Decimal(firstGet)
        except ValueError:
            print("ERROR: Can't convert %s to a decimal" % firstGet)
            return firstGet, False

        upperAllowedValue = Decimal(valuePreviouslySet) + (Decimal(self._quantum) / Decimal(2))
        lowerAllowedValue = Decimal(valuePreviouslySet) - (Decimal(self._quantum) / Decimal(2))

        if not (lowerAllowedValue <= returnValue <= upperAllowedValue):
            print('%s <= %s <= %s is not true' %
                    (lowerAllowedValue, returnValue, upperAllowedValue))
            return firstGet, False

        return firstGet, True

    def checkConsistency(self, valuePreviouslyGotten):
        """ Checks if we are able to set the value that the parameter framework
        just returned to us.

        valuePreviouslyGotten -- the value we are trying to set

        valueToSet -- the value we are trying to set
        returns: True if we are able to set, False otherwise
        """
        (success, errorMsg) = pfw.set(self._paramPath, valuePreviouslyGotten)

        return valuePreviouslyGotten, success

    def checkBijectivity(self, valuePreviouslySet):
        """ Checks that the second get value is strictly equivalent to the
        consistency set. This ensures that the parameter-framework behaves as
        expected.

        valuePreviouslySet -- the value we had previously set

        returns: value the parameter-framework returns us after the second get
        returns: True if we are able to set, False otherwise
        """
        secondGet = pfw.get(self._paramPath)

        if secondGet != valuePreviouslySet:
            return secondGet, False

        return secondGet, True

class PfwClient():

    def __init__(self, configPath):
        self._instance = PyPfw.ParameterFramework(configPath)

        self._logger = PfwLogger()
        self._instance.setLogger(self._logger)
        # Disable the remote interface because we don't need it and it might
        # get in the way (e.g. the port is already in use)
        self._instance.setForceNoRemoteInterface(True)

        self._instance.start()
        self._instance.setTuningMode(True)

    def set(self, parameter, value):
        print('set %s <--- %s' % (parameter, value))
        (success, _, errorMsg) = self._instance.accessParameterValue(parameter, str(value), True)
        return success, errorMsg

    def get(self, parameter):
        (success, value, errorMsg) = self._instance.accessParameterValue(parameter, "", False)
        if not success:
            raise Exception("A getParameter failed, which is unexpected. The"
                            "parameter-framework answered:\n%s" % errorMsg)

        print('get %s ---> %s' % (parameter, value))
        return value

if __name__ == '__main__':
    # It is necessary to add a ./ in front of the path, otherwise the parameter-framework
    # does not recognize the string as a path.
    pfw = PfwClient('./ParameterFrameworkConfiguration.xml')

    success = True

    for size in [8, 16, 32]:
        for integral in range(0,  size):
            for fractional in range (0,  size - integral):
                tester = FixedPointTester(pfw, size, integral, fractional)
                success = tester.run() and success

    exit(0 if success else 1)
