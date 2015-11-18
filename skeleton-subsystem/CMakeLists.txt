# Copyright (c) 2015, Intel Corporation
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

project(parameter-framework-plugins-skeleton)

if(WIN32)
    # Force include iso646.h to support alternative operator form (and, or, not...)
    # Such support is require by the standard and can be enabled with /Za
    # but doing so breaks compilation of windows headers...
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /FIiso646.h")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Werror -Wall -Wextra -Wconversion")
endif()

# Hide symbols by default, then exposed symbols are the same in linux and windows
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN true)

# Force libs and executable to all be at a known place - simplifies a lot of
# things, expecially setting the test environment
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

#
# Find PFW libraries and include directories
#
find_path(PFW_INCLUDE_ROOT NAMES parameter/plugin/Plugin.h)

find_library(PFW_CORE_LIBRARY NAMES parameter)

set(PFW_INCLUDE_DIRS
    ${PFW_INCLUDE_ROOT}/parameter/plugin
    ${PFW_INCLUDE_ROOT}/xmlserializer
    ${PFW_INCLUDE_ROOT}/utility)

add_library(skeleton-subsystem MODULE
SkeletonSubsystemBuilder.cpp
SkeletonSubsystem.cpp
SkeletonSubsystemObject.cpp)

target_include_directories(skeleton-subsystem PRIVATE ${PFW_INCLUDE_DIRS})
target_link_libraries(skeleton-subsystem ${PFW_CORE_LIBRARY})

install(TARGETS skeleton-subsystem
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

include(../ctest/CMakeLists.txt)
add_subdirectory(test)
