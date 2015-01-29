#!/usr/bin/python
# -*-coding:utf-8 -*

# Copyright (c) 2011-2014, Intel Corporation
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

import xml.dom.minidom
import sys

def configure(infile=sys.stdin, outfile=sys.stdout, serverPort=None, structPath=None):
    """ Format an xml PFW config file (standard input) for simulation.

    Allow tuning on @serverPort port, remove the plugins and settings need,
    and change the structure path to absolute."""

    dom = xml.dom.minidom.parse(infile)

    for node in dom.getElementsByTagName("ParameterFrameworkConfiguration"):
        if serverPort is not None:
            node.setAttribute("ServerPort", serverPort)
        node.setAttribute("TuningAllowed", "true")

    def delete(tag):
        for node in dom.getElementsByTagName(tag):
            node.parentNode.removeChild(node)
    delete("Location")
    delete("SettingsConfiguration")

    if structPath is not None:
        for node in dom.getElementsByTagName("StructureDescriptionFileLocation"):
            node.setAttribute("Path", structPath + "/" + node.getAttribute("Path"))

    outfile.write(dom.toxml())

if __name__ == "__main__" :
    """ Execute main if the python interpreter is running this module as the main program """

    configure(serverPort=sys.argv[1], structPath=sys.argv[2])

