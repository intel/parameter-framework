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
Parameter block type testcases.

List of tested functions :
--------------------------
    - [setParameter]  function
    - [getParameter] function

Initial Settings :
------------------
    Block component - 3 UINT:
        - UINT8, size = 8 bits, range : [0, 100]
        - UINT16, size = 16 bits, range : [0, 1000]
        - UINT16, size = 32 bits, range : [0, 1000]

Test cases :
------------
    - Testing nominal situation
    - Testing one-shot setting (setting directly a value for the block)
    - Testing error : Out of range TestCase
    - Testing error : Try to set an undefined param
"""
import commands
import unittest
from Util.PfwUnitTestLib import PfwTestCase
from Util import ACTLogging
log=ACTLogging.Logger()

# Test of type UINT16 - range [0, 1000]
class TestCases(PfwTestCase):
    def setUp(self):
        self.block_name = "/Test/Test/TEST_TYPES/BLOCK_PARAMETER"

        self.param_name = []
        self.filesystem_name = []

        #UINT8_0, size = 8
        self.param_name.append(self.block_name+"/UINT8")
        self.filesystem_name.append("$PFW_RESULT/BLOCK_UINT8")
        #UINT16_1, size = 16
        self.param_name.append(self.block_name+"/UINT16")
        self.filesystem_name.append("$PFW_RESULT/BLOCK_UINT16")
        #UINT32_2, size = 32
        self.param_name.append(self.block_name+"/UINT32")
        self.filesystem_name.append("$PFW_RESULT/BLOCK_UINT32")

        self.pfw.sendCmd("setTuningMode", "on")

    def tearDown(self):
        self.pfw.sendCmd("setTuningMode", "off")


    @unittest.expectedFailure
    def test_Nominal_Case(self):
        """
        Testing BLOCK_PARAMETER in nominal case
        ---------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set UINT parameters in nominal case :
                    - UINT8 = 5
                    - UINT16 = 5
                    - UINT32 = 5
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - Parameters set to nominal value
                - FILESYSTEM parameters set to nominal value
        """
        log.D(self.test_Nominal_Case.__doc__)

        value_param = ["5", "5", "5"]
        filesystem_value = ["0x5", "0x5", "0x5"]

        for index_param in range(len(self.param_name)) :
            log.I("set parameter %s to %s"
                  %(self.param_name[index_param],value_param[index_param]))
            out,err = self.pfw.sendCmd("setParameter",self.param_name[index_param],value_param[index_param])
            assert err == None, log.E("setParameter %s %s : %s"
                                      % (self.param_name[index_param],value_param[index_param], err))
            assert out == "Done", log.F("setParameter %s %s"
                                        %(self.param_name[index_param],value_param[index_param]))
            log.I("Check parameter %s value"
                  %(self.param_name[index_param]))
            out,err = self.pfw.sendCmd("getParameter",self.param_name[index_param])
            assert err == None, log.E("getParameter %s : %s"
                                      % (self.block_name, err))
            assert out == value_param[index_param], log.F("getParameter %s - Expected : %s Found : %s"
                                                          %(self.param_name[index_param],value_param[index_param], out))
            log.I("Check filesystem value")
            assert (commands.getoutput("cat %s" % (self.filesystem_name[index_param]))
                    == filesystem_value[index_param]), log.F("FILESYSTEM : parameter update error for %s after setting %s "
                                                             %(self.block_name, self.param_name[index_param]))


    def test_Set_Block_Directly_Case(self):
        """
        Testing error BLOCK_PARAMETER : set block value directly
        --------------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set Param block directly without setting parameters :
                    - BLOCK_PARAMETER = Dec : 1000000 Hexa : 0xF4240
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - Unable to set directly a parameter block
                - FILESYSTEM parameters set to nominal value
        """
        log.D(self.test_Set_Block_Directly_Case.__doc__)

        value = "1000000"

        log.I("Load the initial value of parameters")
        init_value_param = []
        init_filesystem_value = []

        for index_param in range(len(self.param_name)):
            out,err = self.pfw.sendCmd("getParameter",self.param_name[index_param])
            init_value_param.append(out)
            init_filesystem_value.append(commands.getoutput("cat %s"
                                                            %(self.filesystem_name[index_param])))

        log.I("Try to set parameter %s to %s, failed expected"
              %(self.block_name,value))
        out,err = self.pfw.sendCmd("setParameter",self.block_name, value)
        assert err == None, log.E("setParameter %s %s : %s"
                                   % (self.block_name, value, err))
        assert out != "Done", log.F("Error not detected when setting directly the block %s"
                                    % (self.block_name))
        log.I("Try to get parameter %s to %s, failed expected"
              %(self.block_name,value))
        out,err = self.pfw.sendCmd("getParameter",self.block_name, value)
        assert err == None, log.E("getParameter %s : %s"
                                  % (self.block_name, err))
        assert out != value, log.F("Error not detected when getting directly the block %s"
                                   % (self.block_name))
        log.I("Check filesystem value")
        for index_param in range(len(self.param_name)):
            assert (commands.getoutput("cat %s"%(self.filesystem_name[index_param]))
                == init_filesystem_value[index_param]), log.F("FILESYSTEM : parameter update error for %s"
                                                            %(self.block_name))

        log.I("Check Param value")
        for index_param in range(len(self.param_name)):
            out,err = self.pfw.sendCmd("getParameter",self.param_name[index_param])
            assert (out == init_value_param[index_param]), log.F("BLACKBOARD: Forbidden change value for parameter %s - Expected : %s Found : %s"
                                                               %(self.param_name[index_param],init_value_param[index_param],out))

    def test_Out_Of_Bound_Param_Value_Case(self):
        """
        Testing error BLOCK_PARAMETER : Out of range TestCase
        -----------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set param UINT16 to 65536
                - check parameter UINT16 value
                - check parameter UINT8 value
                - check parameter UINT32 value
                - check block Filesystem value
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - Error detected when setting UINT16_1 to wrong value
                - FILESYSTEM parameters set to nominal value
        """
        log.D(self.test_Out_Of_Bound_Param_Value_Case.__doc__)

        param_value = "65536"

        log.I("Load the initial value of parameters")
        init_value_param = []
        init_filesystem_value = []

        for index_param in range(len(self.param_name)):
            out,err = self.pfw.sendCmd("getParameter",self.param_name[index_param])
            init_value_param.append(out)
            init_filesystem_value.append(commands.getoutput("cat %s"
                                                            %(self.filesystem_name[index_param])))

        log.I("set parameter %s to %s, failed expected"
              %(self.param_name[1],param_value))
        out,err = self.pfw.sendCmd("setParameter",self.param_name[1],param_value)
        assert err == None, log.E("setParameter %s %s : %s"
                                  % (self.param_name[1],param_value, err))
        assert out != "Done", log.F("Error not detected when setting parameter %s to out of bound value %s"
                                    % (self.param_name[1],param_value))
        log.I("Check parameter value")
        for index_param in range(len(self.param_name)):
            out,err = self.pfw.sendCmd("getParameter",self.param_name[index_param])
            assert out == init_value_param[index_param], log.F("BLACKBOARD: Forbidden change value for %s - Expected : %s Found : %s"
                                                             %(self.param_name[index_param],init_value_param[index_param],out))
        log.I("Check filesystem value")
        assert (commands.getoutput("cat %s"%(self.filesystem_name[index_param]))
                == init_filesystem_value[index_param]), log.F("FILESYSTEM : parameter update error for %s"
                                                              %(self.block_name))

    def test_Undefined_Param_Case(self):
        """
        Testing error BLOCK_PARAMETER : Out of range TestCase
        -----------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set parameter PARAM_UNDEF to 1
                - check block parameter Filesystem value
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - Error detected when setting PARAM_UNDEF
                - FILESYSTEM parameters set to nominal value
        """
        log.D(self.test_Undefined_Param_Case.__doc__)

        param_value = "1"
        param_undefined_name = self.block_name + "/PARAM_UNDEF"

        log.I("Load the initial value of parameters")
        init_value_param=[]
        init_filesystem_value=[]

        for index_param in range(len(self.param_name)) :
            out,err = self.pfw.sendCmd("getParameter",self.param_name[index_param])
            init_value_param.append(out)
            init_filesystem_value.append(commands.getoutput("cat %s"
                                                            %(self.filesystem_name[index_param])))

        log.I("set parameter %s to %s, failed expected"
              %(param_undefined_name,param_value))
        out,err = self.pfw.sendCmd("setParameter",param_undefined_name,param_value)
        assert err == None, log.E("setParameter %s %s : %s"
                                  % (param_undefined_name,param_value, err))
        assert out != "Done", log.F("Error not detected when setting parameter %s to out of bound value %s"
                                    % (param_undefined_name,param_value))
        log.I("Check parameter value")
        for index_param in range(len(self.param_name)):
            out,err = self.pfw.sendCmd("getParameter",self.param_name[index_param])
            assert out == init_value_param[index_param], log.F("BLACKBOARD: Forbidden change value for %s - Expected : %s Found : %s"
                                                             %(self.param_name[index_param],init_value_param[index_param],out))
        log.I("Check filesystem value")
        assert (commands.getoutput("cat %s"%(self.filesystem_name[index_param]))
                == init_filesystem_value[index_param]), log.F("FILESYSTEM : parameter update error for %s"
                                                              %(self.block_name))
