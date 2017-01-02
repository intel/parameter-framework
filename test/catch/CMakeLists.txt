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
    # TODO Use gtest as it is the team recommendation
    #      Unfortunately gtest is very hard to setup as not binary distributed
    #      catch is only one header so it is very easy
    find_path(CATCH_HEADER catch.hpp PATH_SUFFIXES catch)
    set_property(CACHE CATCH_HEADER PROPERTY ADVANCED TRUE)
    if(NOT CATCH_HEADER)
        message(SEND_ERROR
" Catch header `catch.hpp' could not be found.
  Catch is a single header unit test framework required to build some tests.
  How to install:
    If it is not available on your package manager, Catch can be downloaded from:
      https://raw.github.com/philsquared/Catch/master/single_include/catch.hpp
    Then either:
      - append the download folder to the `CMAKE_INCLUDE_PATH' or `CMAKE_PREFIX_PATH' variable
      - copy it in a standard location (/usr/include on most linux distribution).
  To remove test dependancies all together, set `BUILD_TESTING' to false.
    It will disable test targets, thus the framework they depend on.")
    endif()

    add_library(catch STATIC catch.cpp)
    target_include_directories(catch PUBLIC "${CATCH_HEADER}")
endif()
