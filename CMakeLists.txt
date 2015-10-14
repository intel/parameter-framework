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

# working on 2.8.12 - broken on older versions
cmake_minimum_required(VERSION 2.8.12)

# linking policy (see cmake --help-policy CMP0003)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

project(parameter-framework)

option(COVERAGE "Build with coverage support" OFF)
option(BASH_COMPLETION "Install bash completion configuration" ON)
option(DOXYGEN "Enable doxygen generation (you still have to run 'make doc')" OFF)
option(REQUIREMENTS "Generate the html version of the 'requirements' documentation" OFF)
option(PYTHON_BINDINGS "Python library to use the Parameter Framrwork from python" ON)
option(C_BINDINGS "Library to use the Parameter Framework using a C API" ON)


# find and set the Parameter Framework's version
execute_process(COMMAND git describe --tags --dirty
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    OUTPUT_VARIABLE PARAMETER_FRAMEWORK_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)

if (WIN32)
    # By default cmake adds a warning level.
    # Nevertheless a different level is wanted for this project.
    # If a two different warning levels are present on the command line, cl raises a warning.
    # Thus delete the default level to set a different one.
    STRING(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

    # Force include iso646.h to support alternative operator form (and, or, not...)
    # Such support is require by the standard and can be enabled with /Za
    # but doing so breaks compilation of windows headers...
    #
    # Suppress warning 4127 (Conditional expression is constant) as it break
    # compilation when testing template value arguments or writing `while(true)`.
    #
    # Suppress warning 4251 (related to exported symbol without dll interface)
    # as it refers to private members (coding style forbids exposing attribute)
    # and thus are not to be used by the client. A better fix would be to export
    # only public methods instead of the whole class, but they are too many to
    # do that. A separated plugin interface would fix that.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /FIiso646.h -wd4127 -wd4251")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Werror -Wall -Wextra -Wconversion -Wno-sign-conversion")
endif ()

# Hide symbols by default, then exposed symbols are the same in linux and windows
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN true)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include(ctest/CMakeLists.txt)

if(COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
endif()

add_subdirectory(xmlserializer)
add_subdirectory(parameter)
add_subdirectory(utility)
add_subdirectory(remote-processor)

add_subdirectory(remote-process)

add_subdirectory(test)

if (BASH_COMPLETION)
    add_subdirectory(tools/bash_completion)
endif()

add_subdirectory(tools/xmlGenerator)
add_subdirectory(tools/xmlValidator)

add_subdirectory(bindings)

add_subdirectory(doc)
