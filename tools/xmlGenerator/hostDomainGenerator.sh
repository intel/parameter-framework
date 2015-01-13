#!/bin/bash
#
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

set -ueo pipefail

# In order to dispatch log properly 4 output streams are available:
# - 1 info
# - 2 error
# - 3 (reserved)
# - 4 standard
# - 5 warning

# Leave standard output unmodified
exec 4>&1
# If the nonverbose long option is provided, do not output info log lines prefixed on stderr.
if test "$1" == --nonverbose
then
    shift
    exec 1>/dev/null
else
    exec 1> >(sed "s/^/($$) Info: /" >&2)
fi
# Prefix all warning and error log lines and redirect them to stderr
exec 5> >(sed "s/^/($$) Warning: /" >&2)
exec 2> >(sed "s/^/($$) Error: /" >&2)

# Get script arguments
validationEnabled="false"
if [ "$1" = "--validate" ]; then
    validationEnabled="true"
    shift
fi
PFWconfigurationFilePath="$1"; shift
CriterionFilePath="$1"; shift
xmlDomainFilePath="$1"; shift

# Set constant variables
testPlatform="test-platform_host"
remoteProcess="remote-process_host"

hostConfig="hostConfig.py"
PFWScriptGenerator="PFWScriptGenerator.py"
portAllocator="portAllocator.py"

TPHost=127.0.0.1
PFWHost=127.0.0.1
TPCreated=false

HostRoot="$ANDROID_HOST_OUT"
TargetRoot="$ANDROID_PRODUCT_OUT/system"

# Global variables
TPSocket=5003
PFWSocket=5000
PFWStartTimeout=60

tmpDir=$(mktemp -d)
tmpFile=$(mktemp --tmpdir="$tmpDir")

# [Workaround]
# The build system does not preserve execution right in external prebuild
for file in "$testPlatform" "$remoteProcess" "$hostConfig" "$PFWScriptGenerator" "$portAllocator"
do
    chmod +x "${HostRoot}/bin/${file}"
done

# Set environment paths
export LD_LIBRARY_PATH="$HostRoot/lib:${LD_LIBRARY_PATH:-}"

# Setup clean trap, it will be called automatically on exit
clean_up () {
    status=$?
    set +e # An error should not abort clean up

    # Exit the test-platform only if it was created by this process
    if $TPCreated
    then
        echo "Exiting test-platform listening on port $TPSocket"
        $remoteProcess $TPHost $TPSocket exit
    fi

    echo "Cleaning $tmpFile ..."
    rm "$tmpFile" || true

    if [ "$validationEnabled" = "true" ]; then
        echo "Cleaning $tmpDir/Schemas ..."
        rm -r "$tmpDir/Schemas" || true
        rmdir "$tmpDir" || true
    fi

    echo "Cleaning status: $status ..."
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

    # Check that both names are different, otherwise there is an error
    if ! test "$(basename "$path")" != "$dest"
    then
        echo "Cannot link $dest to $src !"
        return 1
    fi

    # Check that destination file does not already exist
    if ! test -f "$(dirname "$path")/$dest"
    then
        # Create the symlink. Do not force if it has been created after the previous
        # test, in this case simply ignore the error
        ln -s "$src" "$(dirname "$path")/$dest" || true
    fi
    return 0
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

# The initTestPlatform starts a testPlatform instance with the config file given in argument.
# It will also set the PFWSocket global variable to the PFW remote processor listening socket.
initTestPlatform () {
    # Format the PFW config file
    formatConfigFile "$1" >"$tmpFile"

    # Start test platform
    echo "Starting test-platform on port $TPSocket ..."
    $testPlatform -d "$tmpFile" $TPSocket 2>&5

    res=$?
    if test $res -ne 0
    then
        echo "Unable to launch the simulation platform (using socket $TPSocket)" >&5
        return 4
    fi

    echo "Test platform successfuly loaded!"
    return 0
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
    $TPSendCommand setValidateSchemasOnStart $validationEnabled

    echo "Asking test-platform (port $TPSocket) to start a new PFW instance (listening on port $PFWSocket) ..."
    $TPSendCommand start
    res=$?
    if test $res -ne 0
    then
        echo "Unable to launch the parameter framework (using port $PFWSocket)" >&5
        return 5
    fi

    echo "Parameter framework successfully started!"
    return 0
}

startPFW () {
    # Init the test-platform
    initTestPlatform "$PFWconfigurationFilePath" || return 1
    TPCreated=true

    # Ask the test-platform to start the PFW
    if ! launchTestPlatform "$CriterionFilePath"
    then
        # If PFW didn't start, exit the current test-platform, and return failure in
        # order to choose new socket ports
        echo "Exiting test-platform listening on port $TPSocket"
        $remoteProcess $TPHost $TPSocket exit
        TPCreated=false
        return 1
    fi
}

# Get a new pair of available ports for TP and PFW sockets
changeSocketsPorts() {
    TPSocket=$($portAllocator) || return 1
    PFWSocket=$($portAllocator) || return 1
}

# Start the pfw using different socket if it fails
safeStartPFW () {
    local retry=0
    local nbRetry=1000 # Workaround to avoid build failure, it very very rarely fail this many time

    # Choose a new pair of socket ports
    changeSocketsPorts
    echo "Trying to start test-platform and PFW, with socket $TPSocket and $PFWSocket"

    while ! startPFW
    do
        (($retry < $nbRetry)) || return 1
        retry=$(($retry + 1))

        # Choose a new pair of socket ports
        changeSocketsPorts || continue

        echo "Unable to start PFW, try again with socket $TPSocket and $PFWSocket"
    done
}

deleteEscapedNewLines () {
    sed -r ':a;/\\$/{N;s/\\\n//;ba}'
}

copySchemaFiles() {
    cp -r "$HostRoot"/etc/parameter-framework/Schemas "$tmpDir/Schemas"
}

# Copy the schema files needed for validation
if [ "$validationEnabled" = "true" ]; then
    copySchemaFiles
fi

# The PFW looks for a libremote-processor.so library, not a libremote-processor_host.so
linkLibrary libremote-processor_host.so libremote-processor.so

# Start test platform and the PFW
# Start the pfw using different socket if it fails
safeStartPFW

PFWSendCommand="$remoteProcess $PFWHost $PFWSocket"

$PFWSendCommand setTuningMode on

# Send the xml domain tunning file
if test -s "$xmlDomainFilePath"
then
    echo "Import the xml domain tunning file: $(readlink -e $xmlDomainFilePath)"
    $PFWSendCommand importDomainsWithSettingsXML "$(readlink -e $xmlDomainFilePath)"
fi

# Send the extended domain description routing files converted to pfw commands
m4 "$@" |
    "$PFWScriptGenerator" --output-kind pfw |
    deleteEscapedNewLines |
    forEachLine "$PFWSendCommand @" | sed '/^Done$/d'

# Export the global xml domains description
$PFWSendCommand getDomainsWithSettingsXML |
    # Delete trailing carriage return and format absolute paths
    sed -r -e 's/\r$//' \
           -e 's@(xsi:noNamespaceSchemaLocation=")'"$tmpDir"'/?@\1@' >&4

