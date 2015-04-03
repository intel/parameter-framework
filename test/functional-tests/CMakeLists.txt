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

if (BUILD_TESTING)
    find_program(python2 python2)

    set(PFW_ROOT ${CMAKE_BINARY_DIR}/tmp/test-parameters)
    set(PFW_RESULT ${PFW_ROOT}/result)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/xml/configuration/Structure/Test/TestSubsystem.xml.in
        ${PFW_ROOT}/xml/configuration/Structure/Test/TestSubsystem.xml @ONLY)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/xml/configuration/Structure/Test/TestClass.xml
        ${PFW_ROOT}/xml/configuration/Structure/Test/TestClass.xml
        COPYONLY)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/xml/configuration/ParameterFrameworkConfiguration.xml
        ${PFW_ROOT}/xml/configuration/ParameterFrameworkConfiguration.xml
        COPYONLY)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/xml/configuration/Settings/Test/TestConfigurableDomains.xml
        ${PFW_ROOT}/xml/configuration/Settings/Test/TestConfigurableDomains.xml
        COPYONLY)

    add_test(NAME functional-test
             WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
             COMMAND ${python2} ACTCampaignEngine.py)

    # Custom function defined in the top-level CMakeLists
    set_test_env(functional-test)
    set_property(TEST functional-test APPEND PROPERTY ENVIRONMENT
                 PFW_ROOT=${PFW_ROOT}
                 PFW_RESULT=${PFW_RESULT})
endif()
