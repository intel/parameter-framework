#!/bin/sh

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
