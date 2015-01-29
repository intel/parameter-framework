#!/usr/bin/python2

# Copyright (c) 2015, Intel Corporation
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
from PfwBaseTranslator import PfwBaseTranslator

import argparse
import sys

class PfwScriptTranslator(PfwBaseTranslator):

    def __init__(self):
        super(PfwScriptTranslator, self).__init__()

        self._script = []

    def getScript(self):
        return self._script

    def _doCreateDomain(self, name):
        self._script.append(
                "{cmd} {domain}".format(
                cmd="createDomain",
                domain=name))

    def _doSetSequenceAware(self):
        self._script.append(
                "{cmd} {domain} {aware}".format(
                cmd="setSequenceAwareness",
                domain=self._ctx_domain,
                aware="true"))

    def _doAddElement(self, path):
        self._script.append(
                "{cmd} {domain} {path}".format(
                cmd="addElement",
                domain=self._ctx_domain,
                path=path))

    def _doCreateConfiguration(self, name):
        self._script.append(
                "{cmd} {domain} {config}".format(
                cmd="createConfiguration",
                domain=self._ctx_domain,
                config=name))

    def _doSetElementSequence(self, paths):
        self._script.append(
                "{cmd} {domain} {config} {paths}".format(
                cmd="setElementSequence",
                domain=self._ctx_domain,
                config=self._ctx_configuration,
                paths=" ".join(paths)))

    def _doSetRule(self, rule):
        self._script.append(
                "{cmd} {domain} {config} {rule}".format(
                cmd="setRule",
                domain=self._ctx_domain,
                config=self._ctx_configuration,
                rule=rule))

    def _doSetParameter(self, path, value):
        self._script.append(
                "{cmd} {domain} {config} {path} '{value}'".format(
                cmd="setConfigurationParameter",
                domain=self._ctx_domain,
                config=self._ctx_configuration,
                path=path,
                value=value))

class ArgparseArgumentParser(object) :
    """class that parse command line arguments with argparse library

    result of parsing are the class atributs"""
    def __init__(self) :

        myArgParser = argparse.ArgumentParser(description='Process domain scripts.')

        myArgParser.add_argument('input', nargs='?',
                type=argparse.FileType('r'), default=sys.stdin,
                help="the domain script file, default stdin")

        myArgParser.add_argument('-o', '--output',
                type=argparse.FileType('w'), default=sys.stdout,
                help="the output file, default stdout")

        myArgParser.add_argument('-d', '--debug',
                action='store_true',
                help="print debug warnings")

        myArgParser.add_argument('--output-kind',
                choices=['pfw', 'raw'],
                default='pfw',
                help="output kind; can be either 'raw' (debug only) or 'pfw' (pfw commands; default choice)")


        # process command line arguments
        options = myArgParser.parse_args()

        # maping to atributs
        self.input = options.input
        self.output = options.output

        self.debug = options.debug

        self.output_kind = options.output_kind


# ==============
# main function
# ==============

def printE(s):
    """print in stderr"""
    sys.stderr.write(str(s))

def main ():

    options = ArgparseArgumentParser()

    myparser = EddParser.Parser()
    try:
        myroot = myparser.parse(options.input, options.debug)

    except EddParser.MySyntaxError as ex:
        printE(ex)
        printE("EXIT ON FAILURE")
        exit(2)

    if options.output_kind == 'raw':
        options.output.write(str(myroot))
    else:
        try:
            myroot.propagate()

        except EddParser.MyPropagationError, ex :
            printE(ex)
            printE("EXIT ON FAILURE")
            exit(1)

        if options.output_kind == 'pfw':
            translator = PfwScriptTranslator()
            myroot.translate(translator)
            options.output.write("\n".join(translator.getScript()))

# execute main function if the python interpreter is running this module as the main program
if __name__ == "__main__" :
    main()

