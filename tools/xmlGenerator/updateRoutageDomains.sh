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

# It creates XML settings from a .pfw description
# It Also instanciate a PFW with them loaded

# The generated pfw commands
scriptPFWFile="/tmp/scriptPFW"

# A tmp file, the pfw must have write priviledge on it
ExportTmpFile="/data/routageDomains.xml"

set -e -u -o pipefail


if test $# -eq 0
then
        DomainFile="${PFWtest_DomainFile}"
else
        DomainFile="$@"
fi

function echoColor ()
{
    if test -t 1  ;
    then
        # stdout is a tty => colors
        /bin/echo -e "\033[32m$@\033[0m"
    else
        # stdout is not a tty => no color
        /bin/echo -e "$@"
    fi
}


function androidWithError ()
{
    echo "    \$ $PFWtest_prefixCommand $@"
    local result

    result=$( $PFWtest_prefixCommand "$*"' ; echo $?' | sed -e 's#[\r]##' );

    echo "$(echo "$result" | sed '$d')" ;

    return "$(echo "$result" | tail -n1 )";
}

echoColor "Translate domains to pfw commands"
echoColor "Domains source file: $DomainFile"
m4 "$DomainFile" | $(dirname $0)/PFWScriptGenerator.py --output-kind pfw  -o "$scriptPFWFile"

echoColor "List of generated domains :"
sed -ne 's/createDomain \(.*\)/    \1/p'  "$scriptPFWFile"

echoColor "Make fs writable"
adb remount

echoColor "instanciate pseudo hal"
$PFWtest_test_scripts/instanciatePseudoHal.sh "$PFWtest_ParameterFrameworkConfigurationFile" "$PFWtest_CriterionFile"

echoColor "Create Domains"
$(dirname $0)/domainGenerator.sh "$scriptPFWFile" --keep-autoSync-disable

echoColor "Export domains and settings"
androidWithError remote-process $PFWtest_ParameterFramworkHost exportDomainsWithSettingsXML "$ExportTmpFile"
androidWithError cp "$ExportTmpFile" "$PFWtest_RemoteOutputFile"

echoColor "restart PseudoHal"
$PFWtest_test_scripts/instanciatePseudoHal.sh "$PFWtest_ParameterFrameworkConfigurationFile" "$PFWtest_CriterionFile"

echoColor "Synchronization with local file : $PFWtest_LocalOutputFile"
adb pull "$PFWtest_RemoteOutputFile" "$PFWtest_LocalOutputFile"


