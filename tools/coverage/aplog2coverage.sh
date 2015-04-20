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


set -euo pipefail

xml_report_generation="$(dirname $0)/coverage.py"
xsl_report_formator="$(dirname $0)/coverage.xsl"

help () {
    echo "Usage: $0: [OPTION]... [LOGS_FILE]..."
    echo "Generate domain coverage report from aplogs."
    echo
    echo "Supported options:
  -h, --help            Display this help
  -d, --domains         The domain xml file
  -o, --ouput           Output the report to a file instead of stdout
  -e, --regexp          A regex to filter (egrep) logs in order to keep only the PFW log lines
  -f, --force           Force log parser to continue on ignorable errors
  -p, --parser_option   Options to apply to the log parser"
    echo
    echo 'FIXME: Having more than one dot (".") in aplog paths is not supported.'
    echo " - OK: log/aplog.12"
    echo " - KO: ../aplog.12"
}

# Default values
outputFile="-"
coverage_report_generator_ignorable_errors="\
--ignore-unknown-criterion \
--ignore-incoherent-criterion-state \
--ignore-ineligible-configuration-application"
coverage_report_generator_options=""

# Parse command line arguments
ARGS="$(getopt --options "hd:o:e:p:f" \
               --longoptions "help,domains:,ouput:,regexp:,parser_option:,force" \
               --name "$0" -- "$@" )"

eval set -- "$ARGS"


while true;
do
    case "$1" in
        -h|--help)
            shift
            help
            exit 0
            ;;
        -d|--domains)
            shift
            domainsFile="$1"
            shift
            ;;
        -o|--output)
            shift
            outputFile="$1"
            shift
            ;;
        -e|--regex)
            shift
            filterRegex="$1"
            shift
            ;;
        -p|--parser_option)
            shift
            coverage_report_generator_options+="$1 "
            shift
            ;;
        -f|--force)
            shift
            coverage_report_generator_options+="$coverage_report_generator_ignorable_errors "
            ;;
        --)
            shift
            break
            ;;
    esac
done

if ! test "${domainsFile:-}"
then
    echo "Please provide a xml domain file."
    exit 2
fi

if ! test "${filterRegex:-}"
then
    echo "Please provide a regex to filter log."
    echo "Other PFW instances log lines must not be matched by this regex."
    exit 3
fi

printf "%s\0" "$@" |
    # Sort aplogs in chronological order
    sort --key=2 --field-separator=. --numeric-sort --zero-terminated --reverse |
    # Filter log to leave only PFW log lines
    xargs --null grep --extended-regexp "$filterRegex" |
    # Generate the xml report
    $xml_report_generation --xml $coverage_report_generator_options "$domainsFile" |
    # Generate the html report
    xsltproc --output "$outputFile" $xsl_report_formator -

