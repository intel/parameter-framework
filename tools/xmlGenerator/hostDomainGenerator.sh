#!/bin/bash
#
# INTEL CONFIDENTIAL
# Copyright (c) 2013 Intel
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
# disclosed in any way without Intel's prior express written permission.
#
# No license under any patent, copyright, trade secret or other intellectual
# property right is granted to or conferred upon you by disclosure or delivery
# of the Materials, either expressly, by implication, inducement, estoppel or
# otherwise. Any license under such intellectual property rights must be
# express and approved by Intel in writing.
#

set -ueo pipefail

# In order to dispatch log properly 4 output streams are available:
# - 1 info
# - 2 error
# - 3 (reserved)
# - 4 standard
# - 5 warning

# Leave standard output unmodified
exec 4>&1
# If the verbose long option is provided, output info log lines prefixed on stderr.
if test "$1" == --verbose
then
    shift
    exec 1> >(sed 's/^/Info: /' >&2)
else
    exec 1>/dev/null
fi
# Prefix all warning and error log lines and redirect them to stderr
exec 5> >(sed 's/^/Warning: /' >&2)
exec 2> >(sed 's/^/Error: /' >&2)

# Get script arguments
PFWconfigurationFilePath="$1"; shift
CriterionFilePath="$1"; shift
xmlDomainFilePath="$1"; shift

# Set constant variables
testPlatform="test-platform_host"
remoteProcess="remote-process_host"

hostConfig="hostConfig.py"
PFWScriptGenerator="PFWScriptGenerator.py"

TPHost=localhost
PFWHost=localhost

HostRoot="$ANDROID_HOST_OUT"
TargetRoot="$ANDROID_PRODUCT_OUT/system"

# Global variables
TPSocket=5003
PFWSocket=5000
PFWStartTimeout=60

tmpFile=$(mktemp)
testPlatformPID=0

# [Workaround]
# The build system does not preserve execution right in external prebuild
for file in "$testPlatform" "$remoteProcess" "$hostConfig" "$PFWScriptGenerator"
do
    chmod +x "${HostRoot}/bin/${file}"
done

# Set environment paths
export LD_LIBRARY_PATH="$HostRoot/lib:${LD_LIBRARY_PATH:-}"

# Setup clean trap, it will be called automatically on exit
clean_up () {
    status=$?

    echo "Clean sub process: $testPlatformPID"
    test $testPlatformPID != 0 && kill $testPlatformPID 2>&1
    rm "$tmpFile"
    return $status
}

trap clean_up SIGHUP SIGINT SIGTERM EXIT

# Create a symlink link to rename a library.
# Eg: /lib/ contains the lib mylib_host.so but it must be found under the name
# mylib.so. linkLibrary mylib_host.so mylib.so will create a symlink in mylib_host.so's folder
# called mylib.so, pointing to mylib_host.so
linkLibrary () {
    local src="$1"
    local dest="$2"
    local path=$(find $HostRoot/lib -name "$src")
    if test "$(basename "$path")" != "$dest"
    then
        ln -fs "$src" "$(dirname "$path")/$dest"
    else
        return 1
    fi
}

# The retry function will execute the provided command nbRety times util success.
# It also sleep sleepTime second between each retry.
retry() {
    local command=$1
    local nbRetry=$2
    local sleepTime=$3
    local retry=0
    while ! $command 2>/dev/null >&2
    do
        (($retry < $nbRetry)) || return 1
        retry=$(($retry + 1))
        sleep $sleepTime
    done
    return 0;
}

# Configure the PFW main config file for simulation
formatConfigFile () {
    "$hostConfig" $PFWSocket "$(readlink -f "$(dirname "$1")")" <"$1"
}

# Test if socket is currently used
portIsInUse () {
    port=$1
    test $(ss -an | grep ":${port}" | wc --lines) -gt 0
}

# The initTestPlatform starts a testPlatform instance with the config file given in argument.
# It will also set the PFWSocket global variable to the PFW remote processor listening socket.
initTestPlatform () {
    # Format the PFW config file
    formatConfigFile "$1" >"$tmpFile"

    # Check port is free
    echo "Checking port $TPSocket for TestPlatform"
    ! portIsInUse $TPSocket || return 4
    echo "Port $TPSocket is available for TestPlatform"

    # Start test platform
    $testPlatform "$tmpFile" $TPSocket 2>&5 &
    testPlatformPID=$!

    if ! retry "$remoteProcess $TPHost $TPSocket help" 2 0.1
    then
        echo "Unable to launch the simulation platform (using socket $TPSocket)" >&5
        return 4
    else
        echo "Test platform successfuly loaded!"
    fi
}

# Execute a command for each input line, stopping in case of error
forEachLine () {
    xargs -I@ sh -c "echo \> $1;$1 || exit 255"
}

# Configure test platform (mainly criterion) and start the PFW
launchTestPlatform () {
    local TPSendCommand="$remoteProcess $TPHost $TPSocket"
    sed -e 's/InclusiveCriterion/createInclusiveSelectionCriterionFromStateList/' \
        -e 's/ExclusiveCriterion/createExclusiveSelectionCriterionFromStateList/' \
        -e 's/[[:space:]]:[[:space:]]/ /g' "$CriterionFilePath" |
        forEachLine "$TPSendCommand @"

    $TPSendCommand setFailureOnMissingSubsystem false
    $TPSendCommand setFailureOnFailedSettingsLoad false

    # Check port is free
    echo "Checking port $PFWSocket for PFW"
    ! portIsInUse $PFWSocket || return 5
    echo "Port $PFWSocket is available for PFW"

    $TPSendCommand start
    if ! retry "$remoteProcess $PFWHost $PFWSocket help" 2 0.1
    then
        echo "Unable to launch the parameter framework (using socket $PFWSocket)" >&5
        return 5
    else
        echo "Parameter framework successfully started!"
    fi
}


startPFW () {
    initTestPlatform "$PFWconfigurationFilePath" &&
        launchTestPlatform "$CriterionFilePath"
}

# Start the pfw using different socket if it fails
safeStartPFW () {
    local retry=0
    local nbRetry=10

    while ! startPFW
    do
        (($retry < $nbRetry)) || return 1
        retry=$(($retry + 1))

        clean_up || true
        TPSocket=$(($TPSocket + 10))
        PFWSocket=$(($PFWSocket + 10))
        echo "unable to start PFW, try again with socket $TPSocket and $PFWSocket"
    done
}

deleteEscapedNewLines () {
    sed -r ':a;/\\$/{N;s/\\\n//;ba}'
}

# The PFW looks for a libremote-processor.so library, not a libremote-processor_host.so
linkLibrary libremote-processor_host.so libremote-processor.so

# Start test platform and the PFW
#
# Creation of a critical section to ensure that the startup of the PFW (involving
# sockets creation to communicate with the test platform and the PFW) is serialized
# between potential concurrent execution of this script
#
# Acquire an exclusive lock on the file descriptor 200
exec 200>/var/lock/.hostDomainGenerator.lockfile
flock --timeout $PFWStartTimeout 200

# Start the pfw using different socket if it fails
safeStartPFW

# Release the lock
flock --unlock 200

PFWSendCommand="$remoteProcess $PFWHost $PFWSocket"

$PFWSendCommand setTuningMode on

echo "Send the xml domain tunning file: $xmlDomainFilePath"
$PFWSendCommand setDomainsWithSettingsXML "$(cat $xmlDomainFilePath)"

# Send the extended domain description routing files converted to pfw commands
m4 "$@" |
    "$PFWScriptGenerator" --pfw |
    deleteEscapedNewLines |
    forEachLine "$PFWSendCommand @" | sed '/^Done$/d'

# Export the global xml domains description
$PFWSendCommand getDomainsWithSettingsXML |
    # Delete trailing carriage return and format absolute paths
    sed -r -e 's/\r$//' \
           -e 's/(xsi:noNamespaceSchemaLocation=")[^"]*tmp/\1/' >&4

