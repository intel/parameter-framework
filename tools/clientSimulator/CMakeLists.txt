# Copyright (c) 2015-2017, Intel Corporation
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

# Force usage of Python 3.x; here, we use find_program instead of find_package
# because other parts of Parameter Framework request Python 2.x and CMake has
# troubles with the cohabitation.
find_program(PYTHON3 python3 REQUIRED)

# Find the python modules install path.
# plat_specific is needed because we are installing a shared-library python
# module and not only a pure python module.
# prefix='' makes get_python_lib return a relative path.
execute_process(COMMAND
    ${PYTHON3} -c
        "from distutils import sysconfig;\\
         print(sysconfig.get_python_lib(plat_specific=True, prefix=''))"
    OUTPUT_VARIABLE PYTHON_MODULE_PATH OUTPUT_STRIP_TRAILING_WHITESPACE)

install(DIRECTORY clientsimulator
        DESTINATION ${PYTHON_MODULE_PATH}
        COMPONENT client_simulator)

install(PROGRAMS pfClientSimulator.py
        DESTINATION bin
        COMPONENT client_simulator)
