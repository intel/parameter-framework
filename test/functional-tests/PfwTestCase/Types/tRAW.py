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
Raw Value testcases.

List of tested functions :
--------------------------
    - [setParameter]  function
    - [getParameter] function

Initial Settings :
------------------
    UINT16_MAX :
        - 16 bits Unsigned Integer
        - range [0x0, 0xFFFF]
    UINT_16 :
        - 16 bits Unsigned Integer
        - range [0x0, 0x03E8]

Test cases :
------------
    - Testing setValueSpace/getValueSpace functions
    - Testing setOutputRawFormat/getOutputRawFormat functions
    - UINT16_max parameter in nominal case = 50 / 0x32 :
        - Writing Raw / Reading Hex
        - Writing Raw / Reading Dec
        - Writing Real / Reading Hex
    - UINT16_max parameter min value = 0 :
        - Writing Raw / Reading Hex
        - Writing Raw / Reading Dec
        - Writing Real / Reading Hex
    - UINT16_max parameter max value = 65535 / 0xFFFF :
        - Writing Raw / Reading Hex
        - Writing Raw / Reading Dec
        - Writing Real / Reading Hex
    - UINT16_max parameter max value out of bounds = 0x10000 :
        - Writing Raw
    - UINT16 parameter max value out of bounds = 0x03E9 :
        - Writing Raw
"""
import commands
from Util.PfwUnitTestLib import PfwTestCase
from Util import ACTLogging
log=ACTLogging.Logger()

# Test of type UINT16 - range [0, 1000]
class TestCases(PfwTestCase):
    def setUp(self):
        self.param_name = "/Test/Test/TEST_DIR/UINT16_Max"
        self.filesystem_name="$PFW_RESULT/UINT16_Max"
        self.param_name_2 = "/Test/Test/TEST_DIR/UINT16"
        self.filesystem_name_2="$PFW_RESULT/UINT16"
        self.pfw.sendCmd("setTuningMode", "on")

    def tearDown(self):
        self.pfw.sendCmd("setTuningMode", "off")


    def test_01_SettingOutputRawFormat(self):
        """
        Testing RAW - setOutputRawFormat/getOutputRawFormat functions
        -------------------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set hex in output raw format
                - get output raw format
                - set dec in output raw format
                - get output raw format
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setOutputRawFormat] function
                - [getOutputRawFormat] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - getOutputRawFormat return hex after setting hex
                - getOutputRawFormat return dec after setting dec
        """
        log.D(self.test_01_SettingOutputRawFormat.__doc__)
        value = "hex"
        log.I("Setting %s in output raw format"
              %(value))
        out, err = self.pfw.sendCmd("setOutputRawFormat", value)
        assert err == None, log.E("When setting output raw format : %s"
                                  % (err))
        assert out == "Done", log.F("setOutputRawFormat - expected : Done , found : %s"
                                    % (out))
        log.I("Check output raw format state")
        out, err = self.pfw.sendCmd("getOutputRawFormat","")
        assert err == None, log.E("When getting output raw format : %s"
                                  % (err))
        assert out == value, log.F("getOutputRawFormat - expected : %s , found : %s"
                                   % (value,out))
        value = "dec"
        log.I("Setting %s in output raw format"
              %(value))
        out, err = self.pfw.sendCmd("setOutputRawFormat", value)
        assert err == None, log.E("When setting output raw format : %s"
                                  % (err))
        assert out == "Done", log.F("setOutputRawFormat - expected : Done , found : %s"
                                    % (out))
        log.I("Check output raw format state")
        out, err = self.pfw.sendCmd("getOutputRawFormat","")
        assert err == None, log.E("When getting output raw format : %s"
                                  % (err))
        assert out == value, log.F("getOutputRawFormat - expected : %s , found : %s"
                                   % (value,out))

    def test_02_SettingValueSpace(self):
        """
        Testing RAW - setValueSpace/getValueSpace functions
        ---------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - set raw in value space
                - get value space
                - set real in value space
                - get value space
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setValueSpace] function
                - [getValueSpace] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - getValueSpace return 'raw' after setting raw
                - getValueSpace return 'real' after setting real
        """
        log.D(self.test_02_SettingValueSpace.__doc__)
        value = "raw"
        log.I("Setting %s in value space"
              % (value))
        out, err = self.pfw.sendCmd("setValueSpace", value)
        assert err == None, log.E("When setting value space : %s"
                                  % (err))
        assert out == "Done", log.F("setValueSpace - expected : done , found : %s"
                                    % (out))
        log.I("check value space state")
        out, err = self.pfw.sendCmd("getValueSpace","")
        assert err == None, log.E("When setting value space : %s"
                                  % (err))
        assert out == value, log.F("getValueSpace - expected : %s , found : %s"
                                   % (value,out))
        value = "real"
        log.I("Setting %s in value space" % (value))
        out, err = self.pfw.sendCmd("setValueSpace", value)
        assert err == None, log.E("When setting value space : %s"
                                  % (err))
        assert out == "Done", log.F("setValueSpace - expected : done , found : %s"
                                    % (out))
        log.I("check value space state")
        out, err = self.pfw.sendCmd("getValueSpace","")
        assert err == None, log.E("When setting value space : %s"
                                  % (err))
        assert out == value, log.F("getValueSpace - expected : %s , found : %s"
                                   % (value,out))

    def test_03_WRaw_RHex_Nominal_Case(self):
        """
        Testing RAW - Nominal Case - UINT16_Max - Writing Raw / Reading Hex
        -------------------------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - UINT16_Max parameter in nominal case = 0x32
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - UINT16_Max parameter set to 0x32
                - Blackboard and filesystem values checked
        """
        log.D(self.test_03_WRaw_RHex_Nominal_Case.__doc__)
        value = "0xFF00"
        # When read back, parameter value will be in lowercase
        filesystem_value = "0xff00"
        blackboard_value = "0xFF00"
        value_space = "raw"
        outputraw_format = "hex"

        log.I("UINT16_Max parameter in nominal case = %s"
              %(value))
        log.I("Value space = %s - Output Raw Format = %s"
              %(value_space,outputraw_format))
        self.pfw.sendCmd("setValueSpace", value_space)
        self.pfw.sendCmd("setOutputRawFormat", outputraw_format)
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, log.E("When setting parameter %s : %s"
                                  % (self.param_name, err))
        assert out == "Done", log.F("setParameter - Unable to set the value %s for the  parameter %s"
                                    % (value,self.param_name))
        #Check parameter value on blackboard
        out, err = self.pfw.sendCmd("getParameter", self.param_name, "")
        assert err == None, log.E("When setting parameter %s : %s"
                                  % (self.param_name, err))
        assert out == blackboard_value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s"
                                              % (self.param_name, blackboard_value, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_name)) == filesystem_value, log.F("FILESYSTEM : parameter update error for %s"
                                                                                              % (self.param_name))

    def test_04_WReal_RHex_Nominal_Case(self):
        """
        Testing RAW - Nominal Case - UINT16_Max - Writing Real / Reading Hex
        --------------------------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - UINT16_Max parameter in nominal case = 0x32
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - UINT16_Max parameter set to 0x32
                - Blackboard and filesystem values checked
                - When value space setting to Real, Output Raw Format is
                disabled. Even if Output Raw Format is setting to Hex, the
                output is in decimal.
        """
        log.D(self.test_04_WReal_RHex_Nominal_Case.__doc__)
        value = "50"
        filesystem_value = "0x32"
        blackboard_value = "50"
        value_space = "real"
        outputraw_format = "hex"

        log.I("UINT16_Max parameter in nominal case = %s"
              %(value))
        log.I("Value space = %s - Output Raw Format = %s"
              %(value_space,outputraw_format))
        self.pfw.sendCmd("setValueSpace", value_space)
        self.pfw.sendCmd("setOutputRawFormat", outputraw_format)
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, log.E("When setting parameter %s : %s"
                                  % (self.param_name, err))
        assert out == "Done", log.F("setParameter - Unable to set the value %s for the  parameter %s"
                                    % (value,self.param_name))
        #Check parameter value on blackboard
        out, err = self.pfw.sendCmd("getParameter", self.param_name, "")
        assert err == None, log.E("When setting parameter %s : %s"
                                  % (self.param_name, err))
        assert out == blackboard_value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s"
                                              % (self.param_name, blackboard_value, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_name)) == filesystem_value, log.F("FILESYSTEM : parameter update error for %s"
                                                                                              %(self.param_name))

    def test_05_WRaw_RDec_Nominal_Case(self):
        """
        Testing RAW - Nominal Case - UINT16_Max - Writing Raw / Reading Dec
        -------------------------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - UINT16_Max parameter in nominal case = 0x32
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
                - [setValueSpace] function
                - [setOutputRawFormat] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - UINT16_Max parameter set to 0x32
                - Blackboard and filesystem values checked
        """
        log.D(self.test_05_WRaw_RDec_Nominal_Case.__doc__)
        value = "0x32"
        filesystem_value = "0x32"
        blackboard_value = "50"
        value_space = "raw"
        outputraw_format = "dec"

        log.I("UINT16_Max parameter in nominal case = %s"
              %(value))
        log.I("Value space = %s - Output Raw Format = %s"
              %(value_space,outputraw_format))
        self.pfw.sendCmd("setValueSpace", value_space)
        self.pfw.sendCmd("setOutputRawFormat", outputraw_format)
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, log.E("When setting parameter %s : %s"
                                  % (self.param_name, err))
        assert out == "Done", log.F("setParameter - Unable to set the value %s for the  parameter %s"
                                    % (value,self.param_name))
        #Check parameter value on blackboard
        out, err = self.pfw.sendCmd("getParameter", self.param_name, "")
        assert err == None, log.E("When setting parameter %s : %s"
                                  % (self.param_name, err))
        assert out == blackboard_value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s"
                                              % (self.param_name, blackboard_value, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_name)) == filesystem_value, log.F("FILESYSTEM : parameter update error for %s"
                                                                                              %(self.param_name))



    def test_06_WRaw_RHex_TypeMin_Case(self):
        """
        Testing RAW - Minimum Case - UINT16_Max - Writing Raw / Reading Hex
        -------------------------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - UINT16_Max parameter in nominal case = 0x0
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
                - [setValueSpace] function
                - [setOutputRawFormat] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - UINT16_Max parameter set to 0x0
                - Blackboard and filesystem values checked

        """
        log.D(self.test_06_WRaw_RHex_TypeMin_Case.__doc__)
        value = "0x0"
        filesystem_value = "0x0"
        blackboard_value = "0x0000"
        value_space = "raw"
        outputraw_format = "hex"

        log.I("UINT16_Max parameter in nominal case = %s"
              %(value))
        log.I("Value space = %s - Output Raw Format = %s"
              %(value_space,outputraw_format))
        self.pfw.sendCmd("setValueSpace", value_space)
        self.pfw.sendCmd("setOutputRawFormat", outputraw_format)
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, log.E("When setting parameter %s : %s"
                                  % (self.param_name, err))
        assert out == "Done", log.F("setParameter - Unable to set the value %s for the  parameter %s"
                                    %(value,self.param_name))
        #Check parameter value on blackboard
        out, err = self.pfw.sendCmd("getParameter", self.param_name, "")
        assert err == None, log.E("When setting parameter %s : %s"
                                  % (self.param_name, err))
        assert out == blackboard_value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s"
                                              % (self.param_name, blackboard_value, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_name)) == filesystem_value, log.F("FILESYSTEM : parameter update error for %s"
                                                                                              %(self.param_name))

    def test_07_WReal_RHex_TypeMin_Case(self):
        """
        Testing RAW - Minimum Case - UINT16_Max - Writing Real / Reading Hex
        --------------------------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - UINT16_Max parameter in nominal case = 0x0
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
                - [setValueSpace] function
                - [setOutputRawFormat] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - UINT16_Max parameter set to 0x0
                - Blackboard and filesystem values checked
                - When value space setting to Real, Output Raw Format is
                disabled. Even if Output Raw Format is setting to Hex, the
                output is in decimal.
        """
        log.D(self.test_07_WReal_RHex_TypeMin_Case.__doc__)
        value = "0"
        filesystem_value = "0x0"
        blackboard_value = "0"
        value_space = "real"
        outputraw_format = "hex"

        log.I("UINT16_Max parameter in nominal case = %s"
              %(value))
        log.I("Value space = %s - Output Raw Format = %s"
              %(value_space,outputraw_format))
        self.pfw.sendCmd("setValueSpace", value_space)
        self.pfw.sendCmd("setOutputRawFormat", outputraw_format)
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, log.E("When setting parameter %s : %s"
                                  % (self.param_name, err))
        assert out == "Done", log.F("setParameter - Unable to set the value %s for the  parameter %s"
                                    % (value,self.param_name))
        #Check parameter value on blackboard
        out, err = self.pfw.sendCmd("getParameter", self.param_name, "")
        assert err == None, log.E("When setting parameter %s : %s"
                                  % (self.param_name, err))
        assert out == blackboard_value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s"
                                              % (self.param_name, blackboard_value, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_name)) == filesystem_value, log.F("FILESYSTEM : parameter update error for %s"
                                                                                              %(self.param_name))

    def test_08_WRaw_RDec_TypeMin_Case(self):
        """
        Testing RAW - Minimum Case - UINT16_Max - Writing raw / Reading dec
        -------------------------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - UINT16_Max parameter in nominal case = 0x0
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
                - [setValueSpace] function
                - [setOutputRawFormat] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - UINT16_Max parameter set to 0x0
                - Blackboard and filesystem values checked
                - When value space setting to Real, Output Raw Format is
                disabled. Even if Output Raw Format is setting to Hex, the
                output is in decimal.
        """
        log.D(self.test_08_WRaw_RDec_TypeMin_Case.__doc__)
        value = "0x0"
        filesystem_value = "0x0"
        blackboard_value = "0"
        value_space = "raw"
        outputraw_format = "dec"

        log.I("UINT16_Max parameter in nominal case = %s"
              %(value))
        log.I("Value space = %s - Output Raw Format = %s"
              %(value_space,outputraw_format))
        self.pfw.sendCmd("setValueSpace", value_space)
        self.pfw.sendCmd("setOutputRawFormat", outputraw_format)
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, log.E("When setting parameter %s : %s"
                                  % (self.param_name, err))
        assert out == "Done", log.F("setParameter - Unable to set the value %s for the  parameter %s"
                                    % (value,self.param_name))
        #Check parameter value on blackboard
        out, err = self.pfw.sendCmd("getParameter", self.param_name, "")
        assert err == None, log.E("When setting parameter %s : %s"
                                  % (self.param_name, err))
        assert out == blackboard_value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s"
                                              % (self.param_name, blackboard_value, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_name)) == filesystem_value, log.F("FILESYSTEM : parameter update error for %s"
                                                                                              %(self.param_name))


    def test_09_WRaw_RHex_TypeMax_Case(self):
        """
        Testing RAW - Maximum Case - UINT16_Max - Writing Raw / Reading Hex
        -------------------------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - UINT16_Max parameter in nominal case = 0xFFFF / 65535
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
                - [setValueSpace] function
                - [setOutputRawFormat] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - UINT16_Max parameter set to 0xFFFF
                - Blackboard and filesystem values checked
        """
        log.D(self.test_09_WRaw_RHex_TypeMax_Case.__doc__)
        value = "0xFFFF"
        filesystem_value = "0xffff"
        blackboard_value = "0xFFFF"
        value_space = "raw"
        outputraw_format = "hex"

        log.I("UINT16_Max parameter in nominal case = %s"
              %(value))
        log.I("Value space = %s - Output Raw Format = %s"
              %(value_space,outputraw_format))
        self.pfw.sendCmd("setValueSpace", value_space)
        self.pfw.sendCmd("setOutputRawFormat", outputraw_format)
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, log.E("When setting parameter %s : %s"
                                  % (self.param_name, err))
        assert out == "Done", log.F("setParameter - Unable to set the value %s for the  parameter %s"
                                    % (value,self.param_name))
        #Check parameter value on blackboard
        out, err = self.pfw.sendCmd("getParameter", self.param_name, "")
        assert err == None, log.E("When setting parameter %s : %s"
                                  % (self.param_name, err))
        assert out == blackboard_value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s"
                                              % (self.param_name, blackboard_value, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_name)) == filesystem_value, log.F("FILESYSTEM : parameter update error for %s"
                                                                                              %(self.param_name))

    def test_10_WReal_RHex_TypeMax_Case(self):
        """
        Testing RAW - Maximum Case - UINT16_Max - Writing Real / Reading Hex
        --------------------------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - UINT16_Max parameter in nominal case = 0xFFFF / 65535
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
                - [setValueSpace] function
                - [setOutputRawFormat] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - UINT16_Max parameter set to 0xFFFF
                - Blackboard and filesystem values checked
                - When value space setting to Real, Output Raw Format is
                disabled. Even if Output Raw Format is setting to Hex, the
                output is in decimal.
        """
        log.D(self.test_10_WReal_RHex_TypeMax_Case.__doc__)
        value = "65535"
        filesystem_value = "0xffff"
        blackboard_value = "65535"
        value_space = "real"
        outputraw_format = "hex"

        log.I("UINT16_Max parameter in nominal case = %s"
              %(value))
        log.I("Value space = %s - Output Raw Format = %s"
              %(value_space,outputraw_format))
        self.pfw.sendCmd("setValueSpace", value_space)
        self.pfw.sendCmd("setOutputRawFormat", outputraw_format)
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, log.E("When setting parameter %s : %s"
                                  % (self.param_name, err))
        assert out == "Done", log.F("setParameter - Unable to set the value %s for the  parameter %s"
                                    % (value,self.param_name))
        #Check parameter value on blackboard
        out, err = self.pfw.sendCmd("getParameter", self.param_name, "")
        assert err == None, log.E("When setting parameter %s : %s"
                                  % (self.param_name, err))
        assert out == blackboard_value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s"
                                              % (self.param_name, blackboard_value, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_name)) == filesystem_value, log.F("FILESYSTEM : parameter update error for %s"
                                                                                              %(self.param_name))

    def test_11_WRaw_RDec_TypeMax_Case(self):
        """
        Testing RAW - Maximum Case - UINT16_Max - Writing Real / Reading Hex
        --------------------------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - UINT16_Max parameter in nominal case = 0xFFFF / 65535
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
                - [setValueSpace] function
                - [setOutputRawFormat] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - UINT16_Max parameter set to 0xFFFF
                - Blackboard and filesystem values checked
                - When value space setting to Real, Output Raw Format is
                disabled. Even if Output Raw Format is setting to Hex, the
                output is in decimal.
        """
        log.D(self.test_11_WRaw_RDec_TypeMax_Case.__doc__)
        value = "0xFFFF"
        filesystem_value = "0xffff"
        blackboard_value = "65535"
        value_space = "raw"
        outputraw_format = "dec"

        log.I("UINT16_Max parameter in nominal case = %s"
              %(value))
        log.I("Value space = %s - Output Raw Format = %s"
              %(value_space,outputraw_format))
        self.pfw.sendCmd("setValueSpace", value_space)
        self.pfw.sendCmd("setOutputRawFormat", outputraw_format)
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, log.E("When setting parameter %s : %s"
                                  % (self.param_name, err))
        assert out == "Done", log.F("setParameter - Unable to set the value %s for the  parameter %s"
                                    % (value,self.param_name))
        #Check parameter value on blackboard
        out, err = self.pfw.sendCmd("getParameter", self.param_name, "")
        assert err == None, log.E("When setting parameter %s : %s"
                                  % (self.param_name, err))
        assert out == blackboard_value, log.F("BLACKBOARD : Incorrect value for %s, expected: %s, found: %s"
                                              % (self.param_name, blackboard_value, out))
        #Check parameter value on filesystem
        assert commands.getoutput("cat %s"%(self.filesystem_name)) == filesystem_value, log.F("FILESYSTEM : parameter update error for %s"
                                                                                              %(self.param_name))


    def test_12_WRaw_UINT16_Max_OutOfBound(self):
        """
        Testing RAW - Out of range Case - UINT16_Max - Writing Raw
        ----------------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - UINT16_Max parameter in nominal case = 0x10000 / 65536
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
                - [setValueSpace] function
                - [setOutputRawFormat] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - error detected
                - UINT16_max parameter not updated
                - Blackboard and filesystem values checked
        """
        log.D(self.test_12_WRaw_UINT16_Max_OutOfBound.__doc__)
        value = "0x10000"
        filesystem_value = commands.getoutput("cat %s"%(self.filesystem_name))
        value_space = "raw"
        outputraw_format = "hex"

        log.I("UINT16_Max parameter max value out of bound = %s"%(value))
        log.I("Value space = %s - Output Raw Format = %s"
              %(value_space,outputraw_format))
        self.pfw.sendCmd("setValueSpace", value_space)
        self.pfw.sendCmd("setOutputRawFormat", outputraw_format)
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name, value)
        assert err == None, log.E("when setting parameter %s -> %s"
                                  % (self.param_name, err))
        assert out != "Done", log.F("Error not detected when setting parameter %s out of bound"
                                    % (self.param_name))
        #Check parameter value on blackboard
        assert commands.getoutput("cat %s"%(self.filesystem_name)) == filesystem_value, "FILESYSTEM : Forbiden parameter change"


    def test_13_WRaw_UINT16_OutOfBound(self):
        """
        Testing RAW - Out of range Case - UINT16 - Writing Raw
        ------------------------------------------------------
            Test case description :
            ~~~~~~~~~~~~~~~~~~~~~~~
                - UINT16_Max parameter in nominal case = 0x03E9 / 1001
            Tested commands :
            ~~~~~~~~~~~~~~~~~
                - [setParameter] function
                - [setValueSpace] function
                - [setOutputRawFormat] function
            Used commands :
            ~~~~~~~~~~~~~~~
                - [getParameter] function
            Expected result :
            ~~~~~~~~~~~~~~~~~
                - error detected
                - UINT16 parameter not updated
                - Blackboard and filesystem values checked
        """
        log.D(self.test_13_WRaw_UINT16_OutOfBound.__doc__)
        value = "0x03E9"
        filesystem_value = commands.getoutput("cat %s"%(self.filesystem_name_2))
        value_space = "raw"
        outputraw_format = "hex"

        log.I("UINT16_Max parameter max value out of bound = %s"%(value))
        log.I("Value space = %s - Output Raw Format = %s"
              %(value_space,outputraw_format))
        self.pfw.sendCmd("setValueSpace", value_space)
        self.pfw.sendCmd("setOutputRawFormat", outputraw_format)
        #Set parameter value
        out, err = self.pfw.sendCmd("setParameter", self.param_name_2, value)
        assert err == None, log.E("when setting parameter %s -> %s"
                                  % (self.param_name_2, err))
        assert out != "Done", log.F("Error not detected when setting parameter %s out of bound"
                                    % (self.param_name_2))
        #Check parameter value on blackboard
        assert commands.getoutput("cat %s"%(self.filesystem_name_2)) == filesystem_value, "FILESYSTEM : Forbiden parameter change"
