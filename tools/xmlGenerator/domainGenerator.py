#! /usr/bin/python
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

import PyPfw
import EddParser
from PfwBaseTranslator import PfwBaseTranslator, PfwException
import hostConfig

import argparse
import re
import sys
import tempfile
import os
import logging

def wrap_pfw_error_semantic(func):
    def wrapped(*args, **kwargs):
        ok, error = func(*args, **kwargs)
        if not ok:
            raise PfwException(error)

    return wrapped

class PfwTranslator(PfwBaseTranslator):
    """Generates calls to the Pfw's python bindings"""

    def __init__(self, pfw_instance, error_handler):
        super(PfwTranslator, self).__init__()
        self._pfw = pfw_instance
        self._error_handler = error_handler

    def _handleException(self, ex):
        if isinstance(ex, PfwException):
            # catch and handle translation errors...
            self._error_handler(ex, self._getContext())
        else:
            # ...but let any other error fall through
            raise ex

    @wrap_pfw_error_semantic
    def _doCreateDomain(self, name):
        return self._pfw.createDomain(name)

    @wrap_pfw_error_semantic
    def _doSetSequenceAware(self):
        return self._pfw.setSequenceAwareness(self._ctx_domain, True)

    @wrap_pfw_error_semantic
    def _doAddElement(self, path):
        return self._pfw.addConfigurableElementToDomain(self._ctx_domain, path)

    @wrap_pfw_error_semantic
    def _doCreateConfiguration(self, name):
        return self._pfw.createConfiguration(self._ctx_domain, name)

    @wrap_pfw_error_semantic
    def _doSetElementSequence(self, paths):
        return self._pfw.setElementSequence(self._ctx_domain, self._ctx_configuration, paths)

    @wrap_pfw_error_semantic
    def _doSetRule(self, rule):
        return self._pfw.setApplicationRule(self._ctx_domain, self._ctx_configuration, rule)

    @wrap_pfw_error_semantic
    def _doSetParameter(self, path, value):
        ok, _, error = self._pfw.accessConfigurationValue(
                self._ctx_domain, self._ctx_configuration, path, value, True)

        return ok, error


class PfwTranslationErrorHandler:
    def __init__(self):
        self._errors = []
        self._hasFailed = False

    def __call__(self, error, context):
        sys.stderr.write("Error in context {}:\n\t{}\n".format(context, error))
        self._hasFailed = True

    def hasFailed(self):
        return self._hasFailed

class PfwLogger(PyPfw.ILogger):
    def __init__(self):
        super(PfwLogger, self).__init__()
        self.__logger = logging.root.getChild("parameter-framework")

    def log(self, is_warning, message):
        log_func = self.__logger.warning if is_warning else self.__logger.info
        log_func(message)

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
            help="Directory of parameter-framework XML Schemas on target \
        machine (may be different than generating machine). \
        Defaults to \"Schemas\"",
            default="Schemas")
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
        'InclusiveCriterion' : True,
        'ExclusiveCriterion' : False}
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
            root = parser = EddParser.Parser().parse(edd_file, args.verbose)
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

        # Create a new Pfw instance
        pfw = PyPfw.ParameterFramework(fake_toplevel_config.name)

        # create and inject all the criteria
        logging.info("Creating all criteria")
        for criterion in all_criteria:
            criterion_type = pfw.createSelectionCriterionType(criterion['inclusive'])

            for numerical, literal in enumerate(criterion['values']):
                if criterion['inclusive']:
                    # inclusive criteria are "bitfields"
                    numerical = 1 << numerical

                ok = criterion_type.addValuePair(numerical, literal)
                if not ok:
                    logging.critical("valuepair {}/{} rejected for {}".format(
                        numerical, literal, criterion['name']))
                    exit(1)

            # we don't need the reference to the created criterion type; ignore the
            # return value
            pfw.createSelectionCriterion(criterion['name'], criterion_type)

        # Set failure conditions
        pfw.setFailureOnMissingSubsystem(False)
        pfw.setFailureOnFailedSettingsLoad(False)
        if args.validate:
            pfw.setValidateSchemasOnStart(True)
            if args.schemas_dir is not None:
                schemas_dir = args.schemas_dir
            else:
                schemas_dir = os.path.join(install_path, "Schemas")
            pfw.setSchemaFolderLocation(schemas_dir)

        logger = PfwLogger()
        pfw.setLogger(logger)

        # Disable the remote interface because we don't need it and it might
        # get in the way (e.g. the port is already in use)
        pfw.setForceNoRemoteInterface(True)

        # Finally, start the Pfw
        ok, error = pfw.start()
        if not ok:
            logging.critical("Error while starting the pfw: {}".format(error))
            exit(1)

    ok, error = pfw.setTuningMode(True)
    if not ok:
        logging.critical(error)
        exit(1)

    # Import initial settings file
    if args.initial_settings:
        initial_settings = os.path.realpath(args.initial_settings)
        logging.info(
            "Importing initial settings file {}".format(initial_settings))
        ok, error = pfw.importDomainsXml(initial_settings, True, True)
        if not ok:
            logging.critical(error)
            exit(1)

    # Import each standalone domain files
    for domain_file in args.xml_domain_files:
        logging.info("Importing single domain file {}".format(domain_file))
        ok, error = pfw.importSingleDomainXml(os.path.realpath(domain_file),
                                              False, True, True)
        if not ok:
            logging.critical(error)
            exit(1)

    # Parse and inject each EDD file
    error_handler = PfwTranslationErrorHandler()
    translator = PfwTranslator(pfw, error_handler)

    for filename, parsed_edd in parsed_edds:
        logging.info("Translating and injecting EDD file {}".format(filename))
        parsed_edd.translate(translator)
        if error_handler.hasFailed():
            logging.error("Error while importing parsed EDD files.\n")
            exit(1)

    # dirty hack: we change the schema location (right before exporting the
    # domains) to their location on the target (which may be different than on
    # the machine that is generating the domains)
    pfw.setSchemaFolderLocation(args.target_schemas_dir)

    # Export the resulting settings to the standard output
    ok, domains, error = pfw.exportDomainsXml("", True, False)
    sys.stdout.write(domains)
