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
m4 "$DomainFile" | $(dirname $0)/PFWScriptGenerator.py --pfw  -o "$scriptPFWFile"

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


