# Copyright (c) 2016, Intel Corporation
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

add_library(asio INTERFACE)

if (NETWORKING)
    # Find ASIO (the standalone version, not Boost) If asio isn't installed in
    # a standard directory, add the correct directory to CMAKE_PREFIX_PATH (see
    # the main README for more information).
    find_path(ASIO_DIR NAMES asio.hpp)
    # Hide this variable from CMake GUIs and `cmake -L`
    set_property(CACHE ASIO_DIR PROPERTY ADVANCED TRUE)
    if (NOT ASIO_DIR)
        message(SEND_ERROR
    " ASIO header (asio.hpp) could not be found.
    ASIO is used for networking. On Linux, you should install it using your
    package manager. On Windows, please refer to the main README.")
    endif()

    # Ubuntu 14.04 packages asio 1.10.1 and clang 3.4.1.
    # In this environment, asio stand alone (set ASIO_STANDALONE)
    # does not correcly detect that the stl has CHRONO support (c++11).
    # Force the use of std::chrono by setting ASIO_HAS_STD_CHRONO
    target_include_directories(asio SYSTEM INTERFACE "${ASIO_DIR}")
    target_link_libraries(asio INTERFACE "${CMAKE_THREAD_LIBS_INIT}")
    target_compile_definitions(asio
                               INTERFACE ASIO_STANDALONE
                               INTERFACE ASIO_HAS_STD_CHRONO)
else()
    # Stubbed version
    target_include_directories(asio INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/stub")
endif()
