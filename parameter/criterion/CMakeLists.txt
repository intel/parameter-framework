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

add_library(criterion STATIC
    src/InclusiveCriterion.cpp
    src/Criteria.cpp
    src/Criterion.cpp)

include_directories(
    include
    "${PROJECT_SOURCE_DIR}/parameter/log/include"
    "${PROJECT_SOURCE_DIR}/xmlserializer/"
    "${PROJECT_SOURCE_DIR}/utility/")

target_link_libraries(criterion pfw_utility xmlserializer)

# '-fPIC' needed for shared library to link against libcriterion
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

# Client headers
install(FILES
    include/criterion/client/Criterion.h
    DESTINATION "include/parameter/client/criterion")

if(BUILD_TESTING)
    find_path(CATCH_HEADER catch.hpp)
    include_directories(
        include
        ${CATCH_HEADER})

    # Add unit test
    add_executable(criterionUnitTest test/CriterionUnitTest.cpp)

    target_link_libraries(criterionUnitTest criterion)
    add_test(NAME criterionUnitTest
             COMMAND criterionUnitTest)

    # Custom function defined in the top-level CMakeLists
    set_test_env(criterionUnitTest)
endif()
