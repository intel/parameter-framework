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

class PfwException(Exception):
    pass

class PfwBaseTranslator(object):
    """Abstract Pfw Translator class

    The protocol presented by this class allows the creation of
    parameter-framework settings: domains, configurations rules, etc.

    Some methods must be called within a context; e.g. when 'addElement' is
    called, the element is added to the domain that was last created through
    'createDomain'

    Derived classed should only implemented the backend methods, prefixed by an
    underscore."""

    def __init__(self):
        self._ctx_domain = ''
        self._ctx_configuration = ''
        self._ctx_sequence_aware = False
        self._ctx_command = ''

        self._domain_valid = False
        self._configuration_valid = False

    def _getContext(self):
        return {
                'domain': self._ctx_domain,
                'configuration': self._ctx_configuration,
                'sequence_aware': self._ctx_sequence_aware,
                'command': self._ctx_command}

    def _check(self, func):
        """Check and handles exceptions

        Returns False if an exception occured and was properly caught,
        True otherwise"""
        def wrapped(*args, **kwargs):
            try:
                func(*args, **kwargs)
            except PfwException as ex:
                self._handleException(ex)
                return False

            return True

        return wrapped

    def createDomain(self, name, sequence_aware=False):
        """Create a domain with a given name and optionally set its sequence
        awareness"""

        self._ctx_command = "createDomain"
        self._ctx_domain = name
        self._ctx_configuration = ''
        self._ctx_sequence_aware = sequence_aware
        self._domain_valid = True

        if not self._check(self._doCreateDomain)(name):
            self._domain_valid = False
        elif sequence_aware:
            self._check(self._doSetSequenceAware)()

    def addElement(self, path):
        """Add a configurable element to the current domain"""

        self._ctx_command = "addElement"

        if not self._domain_valid:
            return

        self._check(self._doAddElement)(path)

    def createConfiguration(self, name):
        """Create a configuration for the current domain"""

        self._ctx_command = "createConfiguration"
        self._ctx_configuration = name
        self._configuration_valid = True

        if not self._domain_valid:
            self._configuration_valid = False
            return

        if not self._check(self._doCreateConfiguration)(name):
            self._configuration_valid = False

    def setElementSequence(self, paths):
        """Set the element sequence (if applicable, e.g. if the domain is
        sequence-aware) of the current configuration"""

        self._ctx_command = "setElementSequence"

        if not self._configuration_valid:
            return

        if not self._ctx_sequence_aware:
            return

        self._check(self._doSetElementSequence)(paths)

    def setRule(self, rule):
        """Set the current configuration's applicability rule"""

        self._ctx_command = "setRule"

        if not self._configuration_valid:
            return

        self._doSetRule(rule)

    def setParameter(self, path, value):
        """Set a parameter value for the current configuration"""

        self._ctx_command = "setParameter"

        if not self._configuration_valid:
            return

        self._check(self._doSetParameter)(path, value)

    def _handleException(self, exception):
        raise exception

    def _notImplemented(self):
        raise NotImplementedError(
            "{} is an abstract class".format(self.__class__))

    # Implementation methods
    def _doCreateDomain(self, name):
        self._notImplemented()

    def _doSetSequenceAware(self):
        self._notImplemented()

    def _doAddElement(self, path):
        self._notImplemented()

    def _doCreateConfiguration(self, name):
        self._notImplemented()

    def _doSetElementSequence(self, paths):
        self._notImplemented()

    def _doSetRule(self, rule):
        self._notImplemented()

    def _doSetParameter(self, path, value):
        self._notImplemented()
