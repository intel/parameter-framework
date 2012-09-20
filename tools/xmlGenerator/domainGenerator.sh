#!/bin/bash

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
