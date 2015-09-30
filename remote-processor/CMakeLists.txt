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

add_library(remote-processor SHARED
        Message.cpp
        RequestMessage.cpp
        AnswerMessage.cpp
        RemoteProcessorServer.cpp)

include (GenerateExportHeader)
generate_export_header(remote-processor
                       BASE_NAME remote_processor)

set(CMAKE_THREAD_PREFER_PTHREAD 1)
find_package(Threads REQUIRED)

# Find ASIO (the standalone version, not Boost) and declare it as a dependency
# for remote-processor's users too. (FIXME: this should not be the case)
# If asio isn't installed in a standard directory, add the correct directory to
# CMAKE_PREFIX_PATH (see the main README for more information).
find_path(ASIO_DIR NAMES asio.hpp)
target_include_directories(remote-processor SYSTEM PUBLIC "${ASIO_DIR}")

include_directories("${PROJECT_SOURCE_DIR}/utility"
                    "${CMAKE_CURRENT_BINARY_DIR}")

# Ubuntu 14.04 packages asio 1.10.1 and clang 3.4.1.
# In this environment, asio stand alone (set ASIO_STANDALONE)
# does not correcly detect that the stl has CHRONO support (c++11).
# Force the use of std::chrono by setting ASIO_HAS_STD_CHRONO
target_compile_definitions(remote-processor PUBLIC ASIO_STANDALONE ASIO_HAS_STD_CHRONO)

target_link_libraries(remote-processor pfw_utility ${CMAKE_THREAD_LIBS_INIT})

install(TARGETS remote-processor LIBRARY DESTINATION lib RUNTIME DESTINATION bin)
