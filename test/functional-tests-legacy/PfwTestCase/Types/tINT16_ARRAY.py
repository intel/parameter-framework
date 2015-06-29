# -*-coding:utf-8 -*

# Copyright (c) 2011-2015, Intel Corporation
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

"""
Array parameter type testcases : INT16 Array

List of tested functions :
--------------------------
    - [setParameter]  function
    - [getParameter] function

Initial Settings :
------------------
    UINT16 Array = 16bits signed int array :
        - Array size : 5
        - values range : [-50, 50]

Test cases :
------------
    - Testing nominal case
    - Testing minimum
    - Testing minimum overflow
    - Testing maximum
    - Testing maximum overflow
    - Testing array index out of bounds
"""
import commands
from Util.PfwUnitTestLib import PfwTestCase
from Util import ACTLogging
log=ACTLogging.Logger()


from ctypes import c_uint16


class TestCases(PfwTestCase):

    def setUp(self):
        self.param_name = "/Test/Test/TEST_DIR/INT16_ARRAY"
        self.param_short_name = "$PFW_RESULT/INT16_ARRAY"
        print '\r'
        self.pfw.sendCmd("setTuningMode", "on")
        print '\r'
        self.array_size = 5
        self.array_min = -50
        self.array_max = 50

    def tearDown(self):
        self.pfw.sendCmd("setTuningMode", "off")

    def test_Nominal_Case(self):
        """
        Testing INT16_ARRAY Nominal Case
        --------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - Set every INT16_ARRAY elements to autorized values
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - INT16_ARRAY array elements correctly recorded
                - Blackboard and filesystem values checked
        """
        log.D(self.test_Nominal_Case.__doc__)

        for index in range (self.array_size):
            indexed_array_value = index + self.array_min
            if indexed_array_value>self.array_max:
                indexed_array_value=self.array_max
            hex_indexed_array_value = hex(c_uint16(indexed_array_value).value)
            #Check parameter value setting
            indexed_array_value_path = "".join([self.param_name, "/", str(index)])
            out, err = self.pfw.sendCmd("setParameter", str(indexed_array_value_path), str(indexed_array_value))
            assert err == None, log.E("when setting parameter %s[%s]: %s"
                                      % (self.param_name, str(index), err))
            assert out == "Done", log.F("when setting parameter %s[%s]: %s"
                                        % (self.param_name, str(index), out))
            #Check parameter value on blackboard
            out, err = self.pfw.sendCmd("getParameter", str(indexed_array_value_path), "")
            assert err == None, log.E("when setting parameter %s[%s] : %s"
                                      % (self.param_name, str(index), err))
            assert out == str(indexed_array_value), log.F("BLACKBOARD : Incorrect value for %s[%s], expected: %s, found: %s"
                                                          % (self.param_name, str(index), str(indexed_array_value), out))
            #Check parameter value on filesystem
            files_system_check = "awk -v ligne="+str(index)+" 'NR == ligne+1 { print $0}' "+self.param_short_name
            indexed_files_system_array_value = commands.getoutput(files_system_check)
            assert indexed_files_system_array_value == hex_indexed_array_value, log.F("FILESSYSTEM : %s[%s] update error"
                                                                                      % (self.param_name, str(index)))

    def test_Min_Value(self):
        """
        Testing INT16_ARRAY minimum value
        ---------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - Set every INT16_ARRAY elements to minimum values : 0
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - INT16_ARRAY array elements correctly recorded
                - Blackboard and filesystem values checked
        """
        log.D(self.test_Min_Value.__doc__)
        index = 0
        indexed_array_value = self.array_min
        indexed_array_value_path = "".join([self.param_name, "/", str(index)])
        hex_indexed_array_value = hex(c_uint16(indexed_array_value).value)
        #Check parameter value setting
        out, err = self.pfw.sendCmd("setParameter", str(indexed_array_value_path), str(indexed_array_value))
        assert err == None, log.E("when setting parameter %s[%s]: %s"
                                  % (self.param_name, str(index), err))
        assert out == "Done", log.E("when setting parameter %s[%s]: %s"
                                  % (self.param_name, str(index), out))
        #Check parameter value on blackboard
        out, err = self.pfw.sendCmd("getParameter", str(indexed_array_value_path), "")
        assert err == None, log.E("when setting parameter %s[%s] : %s"
                                  % (self.param_name, str(index), err))
        assert out == str(indexed_array_value), log.F("BLACKBOARD : Incorrect value for %s[%s], expected: %s, found: %s"
                                                      % (self.param_name, str(index), str(indexed_array_value), out))
        #Check parameter value on filesystem
        files_system_check = "awk -v ligne="+str(index)+" 'NR == ligne+1 { print $0}' "+self.param_short_name
        indexed_files_system_array_value = commands.getoutput(files_system_check)
        assert indexed_files_system_array_value == hex_indexed_array_value, log.F("FILESSYSTEM : %s[%s] update error"
                                                                                  % (self.param_name, str(index)))

    def test_Min_Value_Overflow(self):
        """
        Testing INT16_ARRAY parameter values out of negative range
        ----------------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - Set every INT16_ARRAY elements to -1
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - INT16_ARRAY array elements not recorded
                - Error correctly detected
        """
        log.D(self.test_Min_Value_Overflow.__doc__)
        index = 0
        indexed_array_value = self.array_min
        indexed_array_value_path = "".join([self.param_name, "/", str(index)])
        #Check initial parameter value setting
        out, err = self.pfw.sendCmd("setParameter", str(indexed_array_value_path), str(indexed_array_value))
        assert err == None, log.E("when setting parameter %s[%s]: %s"
                                  % (self.param_name, str(index), err))
        assert out == "Done", log.F("when setting parameter %s[%s]: %s"
                                  % (self.param_name, str(index), out))
        files_system_check = "awk -v ligne="+str(index)+" 'NR == ligne+1 { print $0}' "+self.param_short_name
        param_check = commands.getoutput(files_system_check)
        #Check final parameter value setting
        indexed_array_value = indexed_array_value - 1
        out, err = self.pfw.sendCmd("setParameter", str(indexed_array_value_path), str(indexed_array_value))
        assert err == None, log.E("when setting parameter %s[%s]: %s"
                                  % (self.param_name, str(index), err))
        assert out != "Done", log.F("Error not detected when setting parameter %s[%s] out of bounds"
                                    % (self.param_name, str(index)))
        #Check parameter value on filesystem
        indexed_files_system_array_value = commands.getoutput(files_system_check)
        assert indexed_files_system_array_value == param_check, log.F("FILESSYSTEM : %s[%s] forbiden update"
                                                                      % (self.param_name, str(index)))

    def test_Max_Value(self):
        """
        Testing INT16_ARRAY maximum value
        ---------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - Set every INT16_ARRAY elements to maximum values : 15
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - INT16_ARRAY array elements correctly recorded
                - Blackboard and filesystem values checked
        """
        log.D(self.test_Max_Value.__doc__)
        index = 0
        indexed_array_value = self.array_max
        indexed_array_value_path = "".join([self.param_name, "/", str(index)])
        hex_indexed_array_value = hex(c_uint16(indexed_array_value).value)
        #Check parameter value setting
        out, err = self.pfw.sendCmd("setParameter", str(indexed_array_value_path), str(indexed_array_value))
        assert err == None, log.E("when setting parameter %s[%s]: %s"
                                  % (self.param_name, str(index), err))
        assert out == "Done", log.F("when setting parameter %s[%s]: %s"
                                  % (self.param_name, str(index), out))
        #Check parameter value on blackboard
        out, err = self.pfw.sendCmd("getParameter", str(indexed_array_value_path), "")
        assert err == None, log.E("when setting parameter %s[%s] : %s"
                                  % (self.param_name, str(index), err))
        assert out == str(indexed_array_value), log.F("BLACKBOARD : Incorrect value for %s[%s], expected: %s, found: %s"
                                                      % (self.param_name, str(index), str(indexed_array_value), out))
        #Check parameter value on filesystem
        files_system_check = "awk -v ligne="+str(index)+" 'NR == ligne+1 { print $0}' "+self.param_short_name
        indexed_files_system_array_value = commands.getoutput(files_system_check)
        assert indexed_files_system_array_value == hex_indexed_array_value, log.F("FILESSYSTEM : %s[%s] update error"
                                                                                  % (self.param_name, str(index)))

    def test_Max_Value_Overflow(self):
        """
        Testing INT16_ARRAY parameter values out of positive range
        ----------------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - Set every INT16_ARRAY elements to 16
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - INT16_ARRAY array elements not recorded
                - Error correctly detected
        """
        log.D(self.test_Max_Value_Overflow.__doc__)
        index = 0
        indexed_array_value = self.array_max
        indexed_array_value_path = "".join([self.param_name, "/", str(index)])
        #Check initial parameter value setting
        out, err = self.pfw.sendCmd("setParameter", str(indexed_array_value_path), str(indexed_array_value))
        assert err == None, log.E("when setting parameter %s[%s]: %s"
                                  % (self.param_name, str(index), err))
        assert out == "Done", log.F("when setting parameter %s[%s]: %s"
                                  % (self.param_name, str(index), out))
        files_system_check = "awk -v ligne="+str(index)+" 'NR == ligne+1 { print $0}' "+self.param_short_name
        param_check = commands.getoutput(files_system_check)
        #Check final parameter value setting
        indexed_array_value = indexed_array_value + 1
        out, err = self.pfw.sendCmd("setParameter", str(indexed_array_value_path), str(indexed_array_value))
        assert err == None, log.E("when setting parameter %s[%s]: %s"
                                  % (self.param_name, str(index), err))
        assert out != "Done", log.F("Error not detected when setting parameter %s[%s] out of bounds"
                                    % (self.param_name, str(index)))
        #Check parameter value on filesystem
        indexed_files_system_array_value = commands.getoutput(files_system_check)
        assert indexed_files_system_array_value == param_check, log.F("FILESSYSTEM : %s[%s] forbiden update"
                                                                      % (self.param_name, str(index)))

    def test_Array_Index_Overflow(self):
        """
        Testing Array index out of bounds
        ---------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - Set an out of bounds array indexed element
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - INT16_ARRAY array elements not recorded
                - Error correctly detected
        """
        log.D(self.test_Array_Index_Overflow.__doc__)
        index_values = (self.array_size-1, self.array_size+1, -1)
        for index in index_values:
            print index
            indexed_array_value = self.array_max
            indexed_array_value_path = "".join([self.param_name, "/", str(index)])
            #Check parameter value setting
            out, err = self.pfw.sendCmd("setParameter", str(indexed_array_value_path), str(indexed_array_value))
            if index in [0, self.array_size-1]:
                assert err == None, log.E("when setting parameter %s[%s]: %s"
                                          % (self.param_name, str(index), err))
                assert out == "Done", log.F("when setting parameter %s[%s]: %s"
                                          % (self.param_name, str(index), out))
            else:
                assert err == None, log.E("when setting parameter %s[%s]: %s"
                                          % (self.param_name, str(index), err))
                assert out != "Done", log.F("Error not detected when setting array %s index out of bounds"
                                            % (self.param_name))
