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

