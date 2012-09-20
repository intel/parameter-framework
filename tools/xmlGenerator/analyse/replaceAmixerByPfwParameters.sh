#!/bin/sh

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

set -ue

# Replace amixer in stdio by the parameter equivalent
# A convertion table file must be passed in argument
# It is design to be work with the output of scripts/sortAsound.conf.sh

# a perl regexp that matches only alsa mixer lines
regexp='^[^\s/]'

if test $# != 1 || test ! -f $1
then
    echo "argument 1 must be a convertion file like this :
/.../parameter1       alsa mixeur 1
/.../parameter2       alsa mixeur 2
..."
    exit 1
fi

find_pfw_equivalent(){
    if pfw_equivalent="$(grep $1 $2)"
    then
        echo $pfw_equivalent | sed 's/ .*//'
    else
        return 1
    fi
}


while IFS=''; read -r line
do
    if echo "$line" | grep -qP "$regexp" && parameter="$(find_pfw_equivalent "$line" $1)"
    then
        echo "$parameter ( $line )"
    else
        echo "$line"
    fi
done
