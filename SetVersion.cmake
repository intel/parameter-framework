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

# Fallback values:
set(PF_VERSION_MAJOR 0)
set(PF_VERSION_MINOR 0)
set(PF_VERSION_PATCH 0)
set(PF_VERSION_TWEAK 0)
set(PF_VERSION_SHA1 "g0000000000")
set(PF_VERSION_DIRTY "")

# Find and set the Parameter Framework's version
# First, let's see if the user forced a version (i.e. "vX.Y.Z-N")
if(NOT DEFINED PF_VERSION)
    # Else, try to get it from git
    execute_process(COMMAND git describe --tags --long --dirty --abbrev=12
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        OUTPUT_VARIABLE PF_VERSION
        RESULT_VARIABLE GIT_DESCRIBE_RESULT
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET)

    if(GIT_DESCRIBE_RESULT GREATER 0)
        # Or fall back to reading it from .version (this will happen when
        # building from an archive)
        file(READ "${PROJECT_SOURCE_DIR}/.version" PF_VERSION_FILE_CONTENT)

        set(REGEX "tag: (v[0-9.]+)")
        if(PF_VERSION_FILE_CONTENT MATCHES ${REGEX})
            set(PF_VERSION "${CMAKE_MATCH_1}-0")
        endif()
    endif()
else()
    # Set the "nice version string" to the one forced by the user
    set(NICE_PF_VERSION "${PF_VERSION}")
endif()

# Parse the version number to extract the various fields
set(REGEX "([0-9]+).([0-9]+).([0-9]+).([0-9]+)(-(g[0-9a-f]+)?)?(-(dirty)?)?")
if(PF_VERSION MATCHES ${REGEX})
    set(PF_VERSION_MAJOR ${CMAKE_MATCH_1})
    set(PF_VERSION_MINOR ${CMAKE_MATCH_2})
    set(PF_VERSION_PATCH ${CMAKE_MATCH_3})
    set(PF_VERSION_TWEAK ${CMAKE_MATCH_4})
    set(PF_VERSION_SHA1 ${CMAKE_MATCH_6}) # Skip the 5th: it is a superset of the 6th
    set(PF_VERSION_DIRTY ${CMAKE_MATCH_8}) # Skip the 7th: it is a superset of the 8th
endif()

# If we are precisely on a tag, make a nicer version string (unless otherwise
# forced by the user - see above)
if((NOT DEFINED NICE_PF_VERSION) AND (PF_VERSION_TWEAK EQUAL 0) AND (NOT PF_VERSION_DIRTY))
    set(NICE_PF_VERSION "v${PF_VERSION_MAJOR}.${PF_VERSION_MINOR}.${PF_VERSION_PATCH}")
endif()
