#! python2
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

import sys
import os
import subprocess
import difflib

basedir = os.path.dirname(sys.argv[0])

configDir = os.path.join(basedir, "PFConfig")
vectorDir = os.path.join(basedir, "testVector")

command = [sys.executable, "domainGenerator.py",
        "--validate",
        "--verbose",
        "--toplevel-config", os.path.join(configDir, "configuration.xml"),
        "--criteria", os.path.join(configDir, "criteria.txt"),
        "--initial-settings", os.path.join(vectorDir, "initialSettings.xml"),
        "--add-edds", os.path.join(vectorDir, "first.pfw"), os.path.join(vectorDir, "second.pfw"),
        "--add-domains", os.path.join(vectorDir, "third.xml"), os.path.join(vectorDir, "fourth.xml"),
        "--schemas-dir", os.path.join(basedir, "../../schemas")]

reference = open(os.path.join(vectorDir, "reference.xml")).read().splitlines()

process = subprocess.Popen(command, stdout=subprocess.PIPE)
actual = process.stdout.read().splitlines()

unified = difflib.unified_diff(reference,
                               actual,
                               fromfile="reference.xml",
                               tofile="-",
                               lineterm="")
diffs = list(unified)
if diffs:
    for d in diffs:
        print(d)
    sys.exit(1)
if process.wait() != 1:
    print("Error: Expected 1 error, found {}".format(process.returncode))
    sys.exit(1)
