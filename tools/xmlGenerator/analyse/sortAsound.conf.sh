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


# This script reads an asound.conf file and produce for each alsa mixeur
# it's differend values and pcm
#
# ouput example :
#
# my_alsa_mixeur
#     true
#        a_pcm
#         an_other_pcm
#    false
#         in_this_pcm_my_alsa_mixer_takes_the_value_false
# ...

set -eu

if test $# = 0
then
    file="-"
else
    file="$1"
fi

space="    "
previous_command=""

sed -nr -e 's#^pcm.(.*)\{.*$#\1#; t save;
        b next;
        :save;h;b' \
    \
    -e ':next; s/.*name\s+"(.*)"\s+value\s+([^}]*).*/\1 = \2 # /; t pcm;
        b;
        :pcm; G;s/\n//p;' -- "$file" |
    sort | \
    while read line
    do
        current_command="$( echo "$line" | sed 's/ =.*#.*//' )"
        #values are case insensitive
        current_value="$( echo "$line" | sed 's/.*= \(.*\) #.*/\1/' | tr [:upper:] [:lower:] )"
        current_mode="$( echo "$line" | sed 's/.*# //' )"

        if test "$previous_command" != "$current_command"
        then
            echo "$current_command"
            previous_command="$current_command"
            previous_value=""
        fi


        if test  "$previous_value" != "$current_value"
        then
            echo "$space$current_value"
            previous_value="$current_value"
        fi

        echo "$space$space$current_mode"
    done
