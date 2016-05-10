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
include(CMakePushCheckState)
# Save the previous state of CMAKE_REQUIRED_* variables and empty them
cmake_push_check_state()
cmake_reset_check_state()
include(CheckSymbolExists)
set(CMAKE_REQUIRED_DEFINITIONS ${LIBXML2_DEFINITIONS})
set(CMAKE_REQUIRED_INCLUDES ${LIBXML2_INCLUDE_DIR})
# Check that the LIBXML_XINCLUDE_ENABLED macro is defined
# This is the safest check, as it does not involve linking against anything
# (which might fail for another reason than the lack of XInclude support)
check_symbol_exists(LIBXML_XINCLUDE_ENABLED libxml/xmlversion.h XML2_XINCLUDE_SUPPORT)
# Restore the previous state
cmake_pop_check_state()
if(NOT XML2_XINCLUDE_SUPPORT)
    message(SEND_ERROR
            "libxml2 has not been built with support for XInclude."
            "xmlserializer needs that feature;"
            "please install a version of libxml2 supporting it.")
endif()

target_link_libraries(xmlserializer
    PUBLIC pfw_utility # For NonCopyable and ErrorContext
    PRIVATE LibXml2::libxml2)

install(FILES
    XmlSink.h
    XmlSource.h
    XmlElement.h
    XmlSerializingContext.h
    DESTINATION "include/parameter/xmlserializer"
    COMPONENT dev)
