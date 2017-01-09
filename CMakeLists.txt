# Copyright (c) 2014-2016, Intel Corporation
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

# Known to work with CMake 3.2.2, might work with older 3.x versions, will not
# work with versions prior to 3.0.0.
# Visual Studio 14 needs 3.3.0; see https://cmake.org/Bug/print_bug_page.php?bug_id=15552
cmake_minimum_required(VERSION 3.2.2)
if((CMAKE_GENERATOR MATCHES "Visual Studio .*")
    AND (CMAKE_GENERATOR STRGREATER "Visual Studio 14 2015"))
    cmake_minimum_required(VERSION 3.3.0)
endif()

project(parameter-framework)

include(CMakeDependentOption)

option(COVERAGE "Build with coverage support" OFF)
cmake_dependent_option(BASH_COMPLETION "Install bash completion configuration"
    ON
    UNIX # Only allow installing bash completion on Unix environments
    OFF)
option(DOXYGEN "Enable doxygen generation (you still have to run 'make doc')" OFF)
option(REQUIREMENTS "Generate the html version of the 'requirements' documentation" OFF)
option(PYTHON_BINDINGS "Python library to use the Parameter Framework from python" ON)
option(C_BINDINGS "Library to use the Parameter Framework using a C API" ON)
option(FATAL_WARNINGS "Turn warnings into errors (-Werror flag)" ON)
option(NETWORKING "Set to OFF in order to stub networking code" ON)
option(CLIENT_SIMULATOR "Set to OFF to disable client simulator" ON)

include(SetVersion.cmake)

# Add FindLibXml2.cmake in cmake path so that `find_package(LibXml2)` will
# call the wrapper
list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_LIST_DIR}/cmake")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS NO)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

if(WIN32)
    # By default cmake adds a warning level.
    # Nevertheless a different level is wanted for this project.
    # If a two different warning levels are present on the command line, cl raises a warning.
    # Thus delete the default level to set a different one.
    string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

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
    add_compile_options(/W4 /FIiso646.h -wd4127 -wd4251)

    # FIXME: Once we have removed all warnings on windows, add the /WX flags if
    # FATAL_WARNINGS is enabled
else()
    add_compile_options(-Wall -Wextra -Wconversion -Wno-sign-conversion
                 $<$<BOOL:FATAL_WARNINGS>:-Werror>)
endif()

# Hide symbols by default, then exposed symbols are the same in linux and windows
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN true)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
# Automatically add the current source and build dirs to the private and
# interface include directories.
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

include(ctest/CMakeLists.txt)

# Since there is no directory-wide property for linker flags, we can't use
# set_property for the link-time coverage flags.
if(COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

add_subdirectory(xmlserializer)
add_subdirectory(parameter)
add_subdirectory(utility)
add_subdirectory(asio)
add_subdirectory(remote-processor)

add_subdirectory(remote-process)

add_subdirectory(test)

if(BASH_COMPLETION)
    add_subdirectory(tools/bash_completion)
endif()

add_subdirectory(tools/xmlGenerator)
add_subdirectory(tools/xmlValidator)
if (CLIENT_SIMULATOR)
    add_subdirectory(tools/clientSimulator)
endif()

add_subdirectory(bindings)

add_subdirectory(doc)

add_subdirectory(schemas)

add_subdirectory(cpack)
add_subdirectory(cmake)
