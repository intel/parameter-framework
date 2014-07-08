#
# INTEL CONFIDENTIAL
# Copyright 2014 Intel
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
#

import logging
import json


class ConfigParser:
    """ This class define needed configuration environment information """

    def __init__(self,confFileName,consoleLogger):

        # Parsing of Json test file
        with open(confFileName, "r") as testFile:
            #handle exception if error in the json file  ValueError ?
            self.__conf = json.load(testFile)
            self.__logger = logging.getLogger(__name__)
            self.__logger.addHandler(consoleLogger)


    def __getitem__(self,item):
        try:
            return self.__conf[item]
        except KeyError as e:
            self.__logger.error(
                    "The item : {} is not in the configuration file".format(item))
            raise e
