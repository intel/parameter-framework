#! /usr/bin/python
#
# Copyright (c) 2011-2015, Intel Corporation
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

import EddParser
from PFWScriptGenerator import PfwScriptTranslator
import hostConfig

import argparse
import re
import sys
import tempfile
import os
import logging
import subprocess

# If this file is directly executed
if __name__ == "__main__":
    logging.root.setLevel(logging.INFO)

    argparser = argparse.ArgumentParser(description="Parameter-Framework XML \
        Settings file generator")
    argparser.add_argument('--toplevel-config',
            help="Top-level parameter-framework configuration file. Mandatory.",
            metavar="TOPLEVEL_CONFIG_FILE",
            required=True)
    argparser.add_argument('--criteria',
            help="Criteria file, in '<type> <name> : <value> <value...>' \
        format. Mandatory.",
            metavar="CRITERIA_FILE",
            type=argparse.FileType('r'),
            required=True)
    argparser.add_argument('--initial-settings',
            help="Initial XML settings file (containing a \
        <ConfigurableDomains>  tag",
            nargs='?',
            metavar="XML_SETTINGS_FILE")
    argparser.add_argument('--add-domains',
            help="List of single domain files (each containing a single \
        <ConfigurableDomain> tag",
            metavar="XML_DOMAIN_FILE",
            nargs='*',
            dest='xml_domain_files',
            default=[])
    argparser.add_argument('--add-edds',
            help="List of files in EDD syntax (aka \".pfw\" files)",
            metavar="EDD_FILE",
            type=argparse.FileType('r'),
            nargs='*',
            default=[],
            dest='edd_files')
    argparser.add_argument('--schemas-dir',
            help="Directory of parameter-framework XML Schemas for generation \
        validation",
            default=None)
    argparser.add_argument('--target-schemas-dir',
            help="Ignored. Kept for retro-compatibility")
    argparser.add_argument('--validate',
            help="Validate the settings against XML schemas",
            action='store_true')
    argparser.add_argument('--verbose',
            action='store_true')

    args = argparser.parse_args()

    #
    # Criteria file
    #
    # This file define one criteria per line; they should respect this format:
    #
    # <type> <name> : <values>
    #
    # Where <type> is 'InclusiveCriterion' or 'ExclusiveCriterion';
    #       <name> is any string w/o whitespace
    #       <values> is a list of whitespace-separated values, each of which is any
    #                string w/o a whitespace
    criteria_pattern = re.compile(
        r"^(?P<type>(?:Inclusive|Exclusive)Criterion)\s*" \
        r"(?P<name>\S+)\s*:\s*" \
        r"(?P<values>.*)$")
    criterion_inclusiveness_table = {
        'InclusiveCriterion' : "inclusive",
        'ExclusiveCriterion' : "exclusive"}
    all_criteria = []

    # Parse the criteria file
    for line_number, line in enumerate(args.criteria, 1):
        match = criteria_pattern.match(line)
        if not match:
            raise ValueError("The following line is invalid: {}:{}\n{}".format(
                args.criteria.name, line_number, line))

        criterion_name = match.groupdict()['name']
        criterion_type = match.groupdict()['type']
        criterion_values = re.split("\s*", match.groupdict()['values'])

        criterion_inclusiveness = criterion_inclusiveness_table[criterion_type]

        all_criteria.append({
            "name" : criterion_name,
            "inclusive" : criterion_inclusiveness,
            "values" : criterion_values})

    #
    # EDD files (aka ".pfw" files)
    #
    parsed_edds = []
    for edd_file in args.edd_files:
        try:
            root = EddParser.Parser().parse(edd_file, args.verbose)
        except EddParser.MySyntaxError as ex:
            logging.critical(str(ex))
            logging.info("EXIT ON FAILURE")
            exit(2)

        try:
            root.propagate()
        except EddParser.MyPropagationError, ex :
            logging.critical(str(ex))
            logging.info("EXIT ON FAILURE")
            exit(1)

        parsed_edds.append((edd_file.name, root))

    # We need to modify the toplevel configuration file to account for differences
    # between development setup and target (installation) setup, in particular, the
    # TuningMode must be enforced, regardless of what will be allowed on the target
    with tempfile.NamedTemporaryFile(mode='w') as fake_toplevel_config:
        install_path = os.path.dirname(os.path.realpath(args.toplevel_config))
        hostConfig.configure(
                infile=args.toplevel_config,
                outfile=fake_toplevel_config,
                structPath=install_path)
        fake_toplevel_config.flush()

        # Create the connector. Pipe its input to us in order to write commands;
        # connect its output to stdout in order to have it dump the domains
        # there; connect its error output to stderr.
        connector = subprocess.Popen(["domainGeneratorConnector",
                                fake_toplevel_config.name,
                                '1' if args.verbose else '0',
                                '1' if args.validate else '0',
                                args.schemas_dir],
                               stdout=sys.stdout, stdin=subprocess.PIPE, stderr=sys.stderr)

        # create and inject all the criteria
        logging.info("Creating all criteria")
        for criterion in all_criteria:
            instruction = "createSelectionCriterion\0{}\0{}\0{}".format(
                    criterion['inclusive'],
                    criterion['name'],
                    "\0".join(criterion['values']))
            connector.stdin.write("{}\n".format(instruction))

        connector.stdin.write("start\n")
        connector.stdin.flush()

        # Import initial settings file
        if args.initial_settings:
            initial_settings = os.path.realpath(args.initial_settings)
            logging.info(
                "Importing initial settings file {}".format(initial_settings))
            connector.stdin.write("importDomainsWithSettingsXML\0{}\n".format(initial_settings))

        # Import each standalone domain files
        for domain_file in args.xml_domain_files:
            logging.info("Importing single domain file {}".format(domain_file))
            connector.stdin.write("importDomainWithSettingsXML\0{}\n".format(os.path.realpath(domain_file)))

        # Generate the script for each EDD file
        for filename, parsed_edd in parsed_edds:
            logging.info("Translating and injecting EDD file {}".format(filename))
            translator = PfwScriptTranslator(separator="\0")
            parsed_edd.translate(translator)
            connector.stdin.write("\n".join(translator.getScript()))
            connector.stdin.write("\n")

        # Closing the connector's input triggers the domain generation
        connector.stdin.flush()
        connector.stdin.close()
        connector.wait()
