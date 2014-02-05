#!/bin/bash

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



# Execute a PFW script on the target
# In order to regenerate all domains

set -e


if test "$1" == "-" -o ! "$1" ; then
    file="stdin"
elif  test -a "$1"  ; then
    file="$1"
else
    echo "Usage: The argurment must be a file that exists"
    exit 2
fi

#as the shell do not interprete quotes in varable, force it with eval
parameter="$PFWtest_prefixCommand eval remote-process $PFWtest_ParameterFramworkHost"

function echoColor ()
{
    color="$1"
    shift
    if test -t 1  ;
    then
        # stdout is a tty => colors
        /bin/echo -e "\033[${color}m${@}\033[0m"
    else
        # stdout is not a tty => no color
        /bin/echo -e "$@"
    fi
}

echoGreenColor ()
{
        echoColor "32" "$@"
}

echoBlueColor ()
{
        echoColor "34" "$@"
}


function parameterExecute ()
{
    echoGreenColor "   \$ $parameter $@"
    result="$($parameter $@)"

    if [[ "$result" != "Done"* ]]; then
         echo "$result"
         return 2
    fi
    return 0
}


echoBlueColor "Set tuning mode on"
parameterExecute setTuningMode on

echoBlueColor "Set auto sync off"
parameterExecute setAutoSync off

echoBlueColor "deleting old Domains"
parameterExecute deleteAllDomains

echoBlueColor "executing file '$file'"
cat $1 | \
while read line
do

    if [[ "$line" ==  *[a-z]* ]]
    then

        parameterExecute $line
    fi
done

if test $2 != --keep-autoSync-disable
then
    echoBlueColor "Set auto sync on"
    parameterExecute setAutoSync on
fi
