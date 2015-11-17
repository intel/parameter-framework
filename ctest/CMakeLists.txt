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

# Ctest configuration variables must be set BEFORE include(Ctest)

# Check process children and give detail for each leak
set(MEMORYCHECK_COMMAND_OPTIONS
    "${MEMORYCHECK_COMMAND_OPTIONS} --trace-children=yes --leak-check=full")

# As dash is not used to submit results, there is no way to see valgrind result.
# Force it to log to stderr and fail in case of leak or error.
set(MEMORYCHECK_COMMAND_OPTIONS
    "${MEMORYCHECK_COMMAND_OPTIONS} --error-exitcode=255 --log-fd=2")

set(MEMORYCHECK_COMMAND_OPTIONS
    "${MEMORYCHECK_COMMAND_OPTIONS} --suppressions=${CMAKE_CURRENT_LIST_DIR}/valgrind.supp")

# Enable tests, coverage, memcheck, ...
# See http://www.cmake.org/Wiki/CMake/Testing_With_CTest#Dashboard_Preparation
include(CTest)

# Ctest requires its configuration to be placed at the build root
configure_file(${CMAKE_CURRENT_LIST_DIR}/CTestCustom.cmake ${PROJECT_BINARY_DIR} COPYONLY)

# Set environement variables so that executables and libraries can be find by tests.
# (avoids a make install before make test)
function(set_test_env TestName)
    # A cmake PROPERTY value is just a string.
    # The ENVIRONMENT PROPERTY is implemented as a semicolon separated
    # list of value pair. Each value pair being separated by `=`
    # Eg: the python env equivalent of `{"A":"1", "B":"/;/bin"}` is `A=1;B=/\;/bin`
    #
    # As a consequences cmake can not differentiate between a `;` that separates
    # two environment variable and a `;` contained in a variable.
    #
    # Thus all `;` in environment variables must be escaped
    # before inserting them in the ENVIRONMENT PROPERTY.
    string(REPLACE ";" "\\;" TEST_PATH "$ENV{PATH};$ENV{TEST_PATH}")
    string(REPLACE ";" "\\;" TEST_LD_LIBRARY_PATH "$ENV{LD_LIBRARY_PATH}")
    string(REPLACE ";" "\\;" TEST_PYTHONPATH "$ENV{PYTHONPATH}")

    if(WIN32)
        set(SEP "\\;")
    else()
        set(SEP ":")
    endif()


    # With nmake and nmake, executables are build in:
    #     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} => `bin/`
    #
    # Nevertheless Visual studio (and xcode) can build for debug and release
    # from the same build tree. As a result those multi-configuration
    # build systems build executables in:
    #     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION> =>
    #         - `bin/Debug` for debug
    #         - `bin/Release` for release.
    #
    # In order to support both build system types add
    #     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} *and* ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>
    # in the path.
    #
    # Note: Quotes are necessary. Otherwise `;` escapes are discarded
    # TODO: when cmake 3 will be used, split long lines with trailing backslash
    set_property(TEST ${TestName} PROPERTY ENVIRONMENT
                 "PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${SEP}${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>${SEP}${TEST_PATH}"
                 "LD_LIBRARY_PATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}${SEP}${TEST_LD_LIBRARY_PATH}"
                 "PYTHONPATH=${PROJECT_BINARY_DIR}/bindings/python${SEP}${TEST_PYTHONPATH}")
endfunction()

