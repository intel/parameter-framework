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

add_library(xmlserializer STATIC
    XmlElement.cpp
    XmlSerializingContext.cpp
    XmlDocSource.cpp
    XmlMemoryDocSink.cpp
    XmlMemoryDocSource.cpp
    XmlStreamDocSink.cpp
    XmlUtil.cpp)

set_target_properties(xmlserializer PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

find_package(LibXml2 REQUIRED)

# libxml2 has been found, but does it support XInclude ?
include(CheckLibraryExists)
# The first argument of check_library_exists is used as a library to be linked
# into a test executable.
# As such, this argument can only contain *one* library. Here, we use the
# result of CMake's libxml2 detection - the variable name suggests that several
# libraries can be found. At the time of writing, with CMake 2.8, this is not
# the case.
# Thus, it is probably safe to do so.
check_library_exists("${LIBXML2_LIBRARIES}" xmlXIncludeProcess "" XML2_XINCLUDE_SUPPORT)
if(NOT XML2_XINCLUDE_SUPPORT)
    message(SEND_ERROR
            "libxml2 has not been built with support for XInclude."
            "xmlserializer needs that feature;"
            "please install a version of libxml2 supporting it.")
endif()

# Unfortunatly libxml2 include dir and definition need to be manualy added as:
# "many modules currently shipped with CMake do not currently provide IMPORTED targets."
# https://cmake.org/cmake/help/v3.2/prop_tgt/INTERFACE_INCLUDE_DIRECTORIES.html
# When this will be fixed, remove the LIBXML2_INCLUDE_DIR and LIBXML2_DEFINITIONS
# lines, and replace LIBXML2_LIBRARIES by libxml2.
target_include_directories(xmlserializer
    PRIVATE ${LIBXML2_INCLUDE_DIR}
    PUBLIC .)

target_compile_definitions(xmlserializer PRIVATE "${LIBXML2_DEFINITIONS}")


target_link_libraries(xmlserializer
    PUBLIC pfw_utility # For NonCopyable
    PRIVATE ${LIBXML2_LIBRARIES})


install(FILES
    XmlSink.h
    XmlSource.h
    XmlElement.h
    XmlSerializingContext.h
    DESTINATION "include/xmlserializer")
