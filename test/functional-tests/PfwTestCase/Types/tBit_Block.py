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
Bit block parameter type testcases.

List of tested functions :
--------------------------
    - [setParameter]  function
    - [getParameter] function

Initial Settings :
------------------
    Block size = 8bits :
        - BIT_0_3, size = 3, pos=0
        - BIT_3_1, size = 1, pos=3
        - BIT_4_1, size = 1, pos=4
        - BIT_6_2, size = 2, pos=6
        - BIT_7_1, size = 1, pos=7

Test cases :
------------
    - Testing nominal TestCase : set bit to bit
    - Testing error testcase : set block in one shot
    - Testing out of bound TestCase
    - Testing conflicting TestCase : two bits at the same position
    - Testing out of size TestCase : Bit define on a wrong position
"""

import commands
import unittest
from Util.PfwUnitTestLib import PfwTestCase
from Util import ACTLogging
log=ACTLogging.Logger()

# Test of type UINT16 - range [0, 1000]
class TestCases(PfwTestCase):
    def setUp(self):
        self.block_name = "/Test/Test/TEST_TYPES/BLOCK_8BIT"
        self.filesystem_name="$PFW_RESULT/BLOCK_8BIT"

        self.bit_name=[]

        #BIT_0_3, size = 3, pos = 0
        self.bit_name.append("/Test/Test/TEST_TYPES/BLOCK_8BIT/BIT_0_3")
        #BIT_3_1, size = 1, pos = 3
        self.bit_name.append("/Test/Test/TEST_TYPES/BLOCK_8BIT/BIT_3_1")
        #BIT_4_1, size = 1, pos = 4
        self.bit_name.append("/Test/Test/TEST_TYPES/BLOCK_8BIT/BIT_4_1")
        #BIT_6_2, size = 2, pos = 6
        self.bit_name.append("/Test/Test/TEST_TYPES/BLOCK_8BIT/BIT_6_2")
        #BIT_7_1, size = 1, pos = 7
        self.bit_name.append("/Test/Test/TEST_TYPES/BLOCK_8BIT/BIT_7_1")

        self.pfw.sendCmd("setTuningMode", "on")

    def tearDown(self):
        self.pfw.sendCmd("setTuningMode", "off")


    def test_Nominal_Case(self):
        """
        Testing Bit block parameter in nominal case
        -------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set Bit parameter in nominal case :
                    - BLOCK_BIT = 0b01011101, 0x5D, 93
                    - BIT_0_3 = 5
                    - BIT_3_1 = 1
                    - BIT_4_1 = 1
                    - BIT_6_1 = 1
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - BIT parameters set to nominal value
                - FILESYSTEM BLOCK_8BIT set to 0x5D
        """
        log.D(self.test_Nominal_Case.__doc__)

        value_bit=["5","1","1","1"]
        filesystem_value=["0x5","0xd","0x1d","0x5d"]

        for index_bit in range(4):
            log.I("set parameter %s to %s"%(self.bit_name[index_bit],value_bit[index_bit]))
            out,err = self.pfw.sendCmd("setParameter",self.bit_name[index_bit],value_bit[index_bit])
            assert err == None, log.E("setParameter %s %s : %s" % (self.bit_name[index_bit],value_bit[index_bit], err))
            assert out == "Done", log.F("setParameter %s %s" %(self.bit_name[index_bit],value_bit[index_bit]))
            log.I("Check bit %s value"%(self.bit_name[index_bit]))
            out,err = self.pfw.sendCmd("getParameter",self.bit_name[index_bit])
            assert err == None, log.E("getParameter %s : %s" % (self.block_name, err))
            assert out == value_bit[index_bit], log.F("getParameter %s - Expected : %s Found : %s" %(self.bit_name[index_bit],value_bit[index_bit], out))
            log.I("Check filesystem value")
            assert commands.getoutput("cat %s"%(self.filesystem_name)) == filesystem_value[index_bit], log.F("FILESYSTEM : parameter update error for %s after setting bit %s "%(self.block_name, self.bit_name[index_bit]))


    def test_Set_Block_Directly_Case(self):
        """
        Testing setting Bit block parameter in one shot
        -----------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set Bit block directly without setting bit to bit :
                    - BLOCK_BIT = 0b01011101, 0x5D, 93
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - Unable to set directly a block bit
                - FILESYSTEM BLOCK_8BIT must not change
        """
        log.D(self.test_Set_Block_Directly_Case.__doc__)

        value = "93"

        log.I("Load the initial value of bit parameter")
        init_value_bit=[]
        for index_bit in range(4):
            out,err=self.pfw.sendCmd("getParameter",self.bit_name[index_bit])
            assert err == None, log.E("getParameter %s"%self.bit_name[index_bit])
            init_value_bit.append(out)

        init_filesystem_value=commands.getoutput("cat %s"%(self.filesystem_name))

        log.I("Try to set parameter %s to %s, failed expected"%(self.block_name,value))
        out,err = self.pfw.sendCmd("setParameter",self.block_name, value)
        assert err == None, log.E("setParameter %s %s : %s" % (self.block_name, value, err))
        assert out != "Done", log.F("Error not detected when setting directly the block %s" % (self.block_name))
        log.I("Try to get parameter %s to %s, failed expected"%(self.block_name,value))
        out,err = self.pfw.sendCmd("getParameter",self.block_name, value)
        assert err == None, log.E("getParameter %s : %s" % (self.block_name, err))
        assert out != value, log.F("Error not detected when getting directly the block %s" % (self.block_name))
        log.I("Check filesystem value")
        assert commands.getoutput("cat %s"%(self.filesystem_name)) == init_filesystem_value, log.F("FILESYSTEM : parameter update error for %s"%(self.block_name))

        log.I("Check Bit value")
        for index_bit in range(4):
            out,err=self.pfw.sendCmd("getParameter",self.bit_name[index_bit])
            assert out==init_value_bit[index_bit], log.F("BLACKBOARD: Forbidden change value for bit %s - Expected : %s Found : %s"%(self.bit_name[index_bit],init_value_bit[index_bit],out))

    def test_Out_Of_Bound_Bit_Value_Case(self):
        """
        Testing setting bit in out of bound
        -----------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set bit BIT_3_1 to 2
                - check bit BIT_3_1 value
                - check bit BIT_0_3 value
                - check bit BIT_4_1 value
                - check block bit Filesystem value
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - Error detected when setting BIT_3_1 to out of bound value
                - FILESYSTEM BLOCK_8BIT must not change
        """
        log.D(self.test_Out_Of_Bound_Bit_Value_Case.__doc__)

        bit_value="3"

        log.I("Load the initial value of bit parameter")
        init_value_bit=[]
        for index_bit in range(4):
            out,err=self.pfw.sendCmd("getParameter",self.bit_name[index_bit])
            assert err == None, log.E("getParameter %s"%self.bit_name[index_bit])
            init_value_bit.append(out)

        init_filesystem_value=commands.getoutput("cat %s"%(self.filesystem_name))

        log.I("set parameter %s to %s, failed expected"%(self.bit_name[1],bit_value))
        out,err = self.pfw.sendCmd("setParameter",self.bit_name[1],bit_value)
        assert err == None, log.E("setParameter %s %s : %s" % (self.bit_name[1],bit_value, err))
        assert out != "Done", log.F("Error not detected when setting the bit %s to out of bound value %s" % (self.bit_name[1],bit_value))
        log.I("Check Bit value")
        for index_bit in range(4):
            out,err=self.pfw.sendCmd("getParameter",self.bit_name[index_bit])
            assert out==init_value_bit[index_bit], log.F("BLACKBOARD: Forbidden change value for bit %s - Expected : %s Found : %s"%(self.bit_name[index_bit],init_value_bit[index_bit],out))
        log.I("Check filesystem value")
        assert commands.getoutput("cat %s"%(self.filesystem_name)) == init_filesystem_value, log.F("FILESYSTEM : parameter update error for %s"%(self.block_name))

    def test_Undefined_Bit_Case(self):
        """
        Testing setting an undefined bit
        --------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set bit BIT_UNDEF to 1
                - check block bit Filesystem value
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - Error detected when setting BIT_UNDEF
                - FILESYSTEM BLOCK_8BIT must not change
        """
        log.D(self.test_Undefined_Bit_Case.__doc__)

        bit_value="1"
        bit_undefined_name="/Test/Test/TEST_TYPES/BLOCK_8BIT/BIT_UNDEF"

        log.I("Load the initial value of bit parameter")
        init_value_bit=[]
        for index_bit in range(4):
            out,err=self.pfw.sendCmd("getParameter",self.bit_name[index_bit])
            assert err == None, log.E("getParameter %s"%self.bit_name[index_bit])
            init_value_bit.append(out)

        init_filesystem_value=commands.getoutput("cat %s"%(self.filesystem_name))

        log.I("set parameter %s to %s, failed expected"%(bit_undefined_name,bit_value))
        out,err = self.pfw.sendCmd("setParameter",bit_undefined_name,bit_value)
        assert err == None, log.E("setParameter %s %s : %s" % (bit_undefined_name,bit_value, err))
        assert out != "Done", log.F("Error not detected when setting the bit %s to out of bound value %s" % (bit_undefined_name,bit_value))
        log.I("Check Bit value")
        for index_bit in range(4):
            out,err=self.pfw.sendCmd("getParameter",self.bit_name[index_bit])
            assert out==init_value_bit[index_bit], log.F("BLACKBOARD: Forbidden change value for bit %s - Expected : %s Found : %s"%(self.bit_name[index_bit],init_value_bit[index_bit],out))
        log.I("Check filesystem value")
        assert commands.getoutput("cat %s"%(self.filesystem_name)) == init_filesystem_value, log.F("FILESYSTEM : parameter update error for %s"%(self.block_name))

    @unittest.expectedFailure
    def test_Position_Conflicting_Case(self):
        """
        Testing conflicting position
        ----------------------------
            Parameter :
            ~~~~~~~~~~~
                BIT_6_2 : Position = 6, size = 2 conflicting with
                BIT_7_1 : Position = 7; size = 1
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set bit BIT_6_2 to 0
                - set bit BIT_7_1 to 1
                - check bit BIT_6_2 value
                - check block bit Filesystem value
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - Unable to define 2 bits at the same position
        """
        log.D(self.test_Position_Conflicting_Case.__doc__)

        bit_value_6_2="0"
        bit_value_7_1="1"

        log.I("set parameter %s to %s"%(self.bit_name[3],bit_value_6_2))
        out,err = self.pfw.sendCmd("setParameter",self.bit_name[3],bit_value_6_2)
        assert err == None, log.E("setParameter %s %s : %s" % (self.bit_name[3],bit_value_6_2, err))
        assert out == "Done", log.F("setParameter %s %s" %(self.bit_name[3],bit_value_6_2))

        log.I("Load the value of bit parameter")
        init_value_bit=[]
        for index_bit in range(4):
            out,err=self.pfw.sendCmd("getParameter",self.bit_name[index_bit])
            init_value_bit.append(out)

        init_filesystem_value=commands.getoutput("cat %s"%(self.filesystem_name))

        log.I("set parameter %s to %s, failed expected"%(self.bit_name[4],bit_value_7_1))
        out,err = self.pfw.sendCmd("setParameter",self.bit_name[4],bit_value_7_1)
        assert err == None, log.E("setParameter %s %s : %s" % (self.bit_name[4],bit_value_7_1, err))
        assert out != "Done", log.F("Error not detected when setting the conflicting bit %s" % (self.bit_name[4]))
        log.I("Check Bit value")
        for index_bit in range(4):
            out,err=self.pfw.sendCmd("getParameter",self.bit_name[index_bit])
            assert out==init_value_bit[index_bit], log.F("BLACKBOARD: Forbidden change value for bit %s - Expected : %s Found : %s"%(self.bit_name[index_bit],init_value_bit[index_bit],out))
        log.I("Check filesystem value")
        assert commands.getoutput("cat %s"%(self.filesystem_name)) == init_filesystem_value, log.F("FILESYSTEM : parameter update error for %s"%(self.block_name))
