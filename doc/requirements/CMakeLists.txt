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

if(REQUIREMENTS)
    find_program(PANDOC pandoc)

    if(NOT PANDOC)
        message(SEND_ERROR
                "'pandoc' executable has not been found."
                " Please install it to be able to generate the html version of"
                " the 'requirements' documentation.")
    endif()

    configure_file(requirements.css
                   ${CMAKE_CURRENT_BINARY_DIR}/requirements.css
                   COPYONLY)

    set(REQ_OUT_DOC ${CMAKE_CURRENT_BINARY_DIR}/index.html)

    add_custom_target(requirements-doc DEPENDS ${REQ_OUT_DOC})

    add_custom_command(OUTPUT ${REQ_OUT_DOC}
                       DEPENDS requirements.md requirements.css
                       VERBATIM COMMAND
        ${PANDOC} --output ${REQ_OUT_DOC}
                  --from markdown --to html5
                  --number-sections --table-of-contents
                  --smart # typographic dashes
                  --variable "pagetitle:Parameter Framework Requirements"
                  --id-prefix req- # avoid tag name conflicts
                  --css requirements.css # this path is relative to the html doc
                  --css http://sindresorhus.com/github-markdown-css/github-markdown.css
                  ${CMAKE_CURRENT_SOURCE_DIR}/requirements.md)
endif()
