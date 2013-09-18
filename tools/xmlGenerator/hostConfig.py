#!/usr/bin/python
# -*-coding:utf-8 -*

# INTEL CONFIDENTIAL
# Copyright  2013 Intel
# Corporation All Rights Reserved.
#
# The source code contained or described herein and all documents related to
# the source code ("Material") are owned by Intel Corporation or its suppliers
# or licensors. Title to the Material remains with Intel Corporation or its
# suppliers and licensors. The Material contains trade secrets and proprietary
# and confidential information of Intel or its suppliers and licensors. The
# Material is protected by worldwide copyright and trade secret laws and
# treaty provisions. No part of the Material may be used, copied, reproduced,
# modified, published, uploaded, posted, transmitted, distributed, or
# disclosed in any way without Intels prior express written permission.
#
# No license under any patent, copyright, trade secret or other intellectual
# property right is granted to or conferred upon you by disclosure or delivery
# of the Materials, either expressly, by implication, inducement, estoppel or
# otherwise. Any license under such intellectual property rights must be
# express and approved by Intel in writing.

import xml.dom.minidom
import sys

serverPort=sys.argv[1]
structPath=sys.argv[2]

def main():
    """ Format an xml PFW config file (standard input) for simulation.

    Allow tuning on argv[1] port, remove the plugins and settings need,
    and change the structure path to absolute."""

    dom = xml.dom.minidom.parse(sys.stdin)

    for node in dom.getElementsByTagName("ParameterFrameworkConfiguration"):
        node.setAttribute("ServerPort", serverPort)
        node.setAttribute("TuningAllowed", "true")

    def delete(tag):
        for node in dom.getElementsByTagName(tag):
            node.parentNode.removeChild(node)
    delete("Location")
    delete("SettingsConfiguration")

    for node in dom.getElementsByTagName("StructureDescriptionFileLocation"):
        node.setAttribute("Path", structPath + "/" + node.getAttribute("Path"))

    sys.stdout.write(dom.toxml())

if __name__ == "__main__" :
    """ Execute main if the python interpreter is running this module as the main program """
    main()

