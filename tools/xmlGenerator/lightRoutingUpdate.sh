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


# This script generate an xml from a .pfw in an easy and fast way
# It only works if you have an parameter running with criterion compatible
# with the .pfw

set -eu -o pipefail

adbShell="adb shell"
parameterCommandAccess="eval remote-process localhost 5000"
parameter="$adbShell $parameterCommandAccess"

tmpfile="/tmp/pfw_commands"
target_tmpfile="/data/pfw_commands"


adbShellForward () {

    echo 'echo $?; exit' >> "$1"

    # Send commands
    adb push "$1" "$target_tmpfile"
    $adbShell chmod 700 "$target_tmpfile"

    $adbShell "$target_tmpfile" |
        # keep only the -2 line, the output of "echo $?"
        tee /dev/stderr | tail -2 | sed '1{s/\r//;q}' |
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

function log ()
{
    echo "$@" >&2
}

# Clean tmp file
echo > "${tmpfile}"

#################
# Configure PFW #
#################

echo "setTuningMode on" >> "${tmpfile}"
echo "setAutoSync off"  >> "${tmpfile}"


log "Delete routing domains"
for domain in $(parameterExecute listDomains |grep -io '^Routing.[^ ]*')
do
    log "Will delete domain $domain"
    echo "deleteDomain $domain" >> "${tmpfile}"
done

#########################
# Generate PFW commands #
#########################

log "Generate domain commands from file(s): $*"
m4 "$@" \
    | $(dirname $0)/PFWScriptGenerator.py --output-kind pfw  >> "${tmpfile}"


echo "setAutoSync off" >> "${tmpfile}"
echo "setTuningMode off" >> "${tmpfile}"

sed -i -e':a
      # look for line finishing with \
      /\\$/{
        # Delete the last char (\)
        s/\\$//;
        # Append the next line and delete the \n separator
        N;
        s/\n/ /;
        # Jump back to the expression beginning
        ta;
      };
      # delete empty lines;
      /^$/d
      # delete leading space
      s/^ *//
      # delete multiple spaces
      s/  */ /g;
      # Prefix each line with "$parameterCommandAccess
      '"s/^/$parameterCommandAccess /" \
  "${tmpfile}"

echo "set -xeu" > "${tmpfile}2"
cat "${tmpfile}" >> "${tmpfile}2"

log "Execute commands"
adbShellForward "${tmpfile}2"


#####################
# Generate xml file #
#####################

outputFilePath="domains.xml"

if test $# -ne 0
then
    # Output file is the input file with the xml extension
    outputFilePath="${1%.*}.xml"
    if test "$outputFilePath" == "$1"
    then
        # There is a conflict
        outputFilePath="${1}.xml"
    fi
fi

log "Output file: $outputFilePath"

parameter_execute_if_exist () {
    $parameter help | grep --quiet --word $1 || return $?
    $parameter $1 | sed 's/\r//'
}

parameter_execute_if_exist getDomainsWithSettingsXML > "$outputFilePath" ||
    # Fall back on old command
    parameter_execute_if_exist getDomainsXML > "$outputFilePath"


log "The media server PFW domains have been change, please restart it to restore old domains"
