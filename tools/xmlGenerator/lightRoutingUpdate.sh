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


# This script generate an xml from a .pfw in an easy and fast way
# It only works if you have an parameter running with criterion compatible
# with the .pfw

set -eu -o pipefail

adbShell="adb shell"
parameterCommandAccess="eval parameter"
parameter="$adbShell $parameterCommandAccess"

tmpfile="/tmp/pfw_commands"


adbShellForward () {
    # Send commands
    (
        echo 'PS1="# "'
        echo 'set -xeu'
        cat "$1"
        echo 'echo $?'
        echo 'exit'
     )|
        # adb shell can not handle a too fast input, create artificial delay :(
        while read line; do echo  "$line"; sleep 0.04; done |
        $adbShell |
        # keep only the -3line, the output of "echo $?"
        tee /dev/stderr | tail -3 | sed '1{s/\r//;q}' |
        # stop if $? != 0 (as of set -e)
        xargs test 0 -eq 2> /dev/null

}

function parameterExecute ()
{
    echo "   \$ $parameter $@"
    result="$($parameter $@ | sed 's/\r$//')"

    if [[ "$result" != "Done"* ]]; then
         echo "$result"
         return 2
    fi
    return 0
}

# Clean tmp file
rm "$tmpfile" || true

if test $# -eq 0
then
    domainFile="$(realpath "$PFWtest_DomainFile")"
else
    domainFile="$1"
fi

#################
# Configure PFW #
#################

parameterExecute setTuningMode on
parameterExecute setAutoSync off


echo "Delete routing domains"
for domain in $(parameterExecute listDomains |grep -io '^Routing.[^ ]*')
do
    echo "Will delete domain $domain"
    echo "deleteDomain $domain" >> "$tmpfile"
done

#########################
# Generate PFW commands #
#########################

echo "Generate domain commands from file $(realpath $domainFile)"
m4 "$domainFile" | $(dirname $0)/PFWScriptGenerator.py --pfw  >> "$tmpfile"


sed -i -e':a' \
  -e '# look for line finishing wih \
      /\\$/{
        # Delete the last char (\)
        s/\\$//;
        # Append the next line and delete the \n separator
        N;
        s/\n/ /;
        # Jump back to the expression begining
        ta;
      };' \
  -e '/^$/d;# delete empty lines' \
  -e 's/^ *//;# delete leading space' \
  -e 's/  */ /g;# delete multiple spaces' \
  -e 's/^.*$/'"$parameterCommandAccess"' "\0"\;/;# Add a prefix ($parameterCommandAccess) on each line' "$tmpfile"

echo "Execute commands"
adbShellForward "$tmpfile"


parameterExecute setAutoSync off
parameterExecute setTuningMode off

#####################
# Generate xml file #
#####################

# Output file is the input file with the xml extension
outputFile="$(echo "$domainFile" | sed 's#\.pfw$#.xml#')"

# Test if diferent from .pfw file (we do not whant to overwrite it)
if test "$outputFile" == "$domainFile"
then
    outputFile="${outputFile}.xml"
fi

echo "Output file: $outputFile"
$parameter getDomainsXML |sed 's/\r//' > "$outputFile"


echo "The media serveur PFW domains have been change, please restart it to restore old domains"
