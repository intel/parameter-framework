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

set(CPACK_PACKAGE_VERSION
    "${PF_VERSION_MAJOR}.${PF_VERSION_MINOR}.${PF_VERSION_PATCH}.${PF_VERSION_TWEAK}")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Plugin-based and rule-based framework for describing and controlling parameters.")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_FILE_README "${PROJECT_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_VENDOR "Intel")
set(CPACK_PACKAGE_CONTACT "parameter-framework@lists.01.org")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/COPYING.txt")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://01.org/parameter-framework")

# Arbitrary GUID needed by the installer on windows to abort
# if it detects a pre-existing installation that uses the same GUID.
set(CPACK_WIX_PRODUCT_GUID "5CB589A1-8AE6-4CD2-AF46-15272B93B462")

# Arbitrary GUID needed by the installer on windows to replace
# existing installations that use the same GUID.
set(CPACK_WIX_UPGRADE_GUID "47E60D10-B344-445D-A2F6-5684CC8B1D47")

if (WIN32)
    # On windows, pack compiler provided libraries (MSVC redistributable) with the project
    include(InstallRequiredSystemLibraries)
    # TODO: pack libxml2.dll with the project
endif()

# Use dpkg-shlibdeps (debian generator only) to generate better package dependency list.
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)
