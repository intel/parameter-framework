#! /usr/bin/env python
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

import PyPfw
import sys
import logging

class MyLogger(PyPfw.ILogger):
    def __init__(self):
        # Calling the base constructor is necessary: if you don't, MyLogger
        # won't be recognised as a derived class of PyPfw.ILogger
        super(MyLogger, self).__init__()
        self.info = logging.info
        self.warning = logging.warning

logging.root.setLevel(logging.INFO)

pfw = PyPfw.ParameterFramework(sys.argv[1])

# warning: don't pass MyLogger() directly as argument to setLogger() or it will
# be garbage collected
mylogger = MyLogger()
pfw.setLogger(mylogger);

mood, error = pfw.createExclusiveCriterion("Mood", {"mad" : 1, "sad" : 2, "glad" : 3})
if not mood:
    logging.error("Can't create the 'Mood' criterion: %s" % error)

colors, error = pfw.createInclusiveCriterion("Colors", {"red" : 1, "green" : 2, "blue" : 3})
if not colors:
    logging.error("Can't create the 'Colors' criterion: %s" % error)

ok, error = pfw.start()

if not ok:
    print("Error while starting the pfw: {}".format(error))

ok, error = mood.setState([2])
if not ok:
    print("Error while setting 'mood' criterion: {}".format(error))

ok, error = colors.setState([1, 2])
if not ok:
    print("Error while setting 'colors' criterion: {}".format(error))

raw_input("[Press enter to exit]")
