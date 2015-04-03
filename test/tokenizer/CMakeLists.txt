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

if(BUILD_TESTING)
    # Add catch unit test framework
    # TODO Use gtest as it is the team recommendation
    #      Unfortunately gtest is very hard to setup as not binary distributed
    #      catch is only one header so it is very easy
    # Catch can be downloaded from:
    # https://raw.github.com/philsquared/Catch/master/single_include/catch.hpp
    # Then append the download folder to the CMAKE_INCLUDE_PATH variable or
    # copy it in a standard location (/usr/include on most linux distribution).
    find_path(CATCH_HEADER catch.hpp)
    include_directories(${CATCH_HEADER})

    # Add unit test
    add_executable(tokenizerTest Test.cpp)

    include_directories(${PROJECT_SOURCE_DIR}/utility)
    target_link_libraries(tokenizerTest pfw_utility)

    add_test(NAME tokenizerTest
             COMMAND tokenizerTest)

    # Custom function defined in the top-level CMakeLists
    set_test_env(tokenizerTest)
endif()
