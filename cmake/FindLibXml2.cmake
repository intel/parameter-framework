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

# Wrapper around the official FindLibXml2.cmake in order to provide imported targets.
# This has the advantage of propagating Transitive Usage Requirements to consumers.
# See: https://cmake.org/cmake/help/git-master/manual/cmake-developer.7.html#find-modules
# See: https://cmake.org/gitweb?p=cmake.git;a=blob;f=Modules/FindGLUT.cmake
#      for a modern Find<package>.cmake example
# TODO: make a real FindLibXml2.cmake in order to upstream it.

# More info on how to write Find*.cmake on:
# https://cmake.org/cmake/help/git-master/manual/cmake-developer.7.html#find-modules

# Remove this directory from CMAKE_MODULE_PATH for the call to the original FindLibXml2.cmake
set(CMAKE_MODULE_PATH_BACKUP "${CMAKE_MODULE_PATH}")
list(REMOVE_ITEM CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
find_package(LibXml2)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH_BACKUP}")
unset(CMAKE_MODULE_PATH_BACKUP)

add_library(LibXml2::libxml2 UNKNOWN IMPORTED)

set_target_properties(LibXml2::libxml2 PROPERTIES
    IMPORTED_LOCATION "${LIBXML2_LIBRARIES}"
    INTERFACE_INCLUDE_DIRECTORIES "${LIBXML2_INCLUDE_DIR}"
    INTERFACE_LINK_LIBRARIES "${LIBXML2_LIBRARIES}"
    IMPORTED_LINK_INTERFACE_LANGUAGES "C"
    INTERFACE_COMPILE_DEFINITIONS "${LIBXML2_DEFINITIONS}")

# Do not call find_package_handle_standard_args as this has already been done
# in find_package(LibXml2)
