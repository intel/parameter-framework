# Copyright (c) 2014-2015, Intel Corporation
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

import threading
import subprocess
import logging
import signal
import os


class StreamLoggerThread(threading.Thread):

    """ File-like object used to log Popen stdout and stderr streams """

    def __init__(self, consoleLogger, level, name):
        """
            StreamLoggerThread Object initializer

            :param consoleLogger: console log handler
            :type consoleLogger: Handler
            :param level: desired logger level of the stream
            :type level: logging.level
            :param name: Thread name
            :type name: string
        """
        super().__init__()

        # Pipe to interact with subprocess
        self.__readFd, self.__writeFd = os.pipe()

        self.__logger = logging.getLogger(__name__)
        self.__logger.addHandler(consoleLogger)

        self.__level = level

        self.name = name

        # Start stream logging
        self.start()

    def fileno(self):
        """ Give Writing side of internal pipe to receive data """

        return self.__writeFd

    def run(self):
        """ Read the reading side of the pipe until EOF """

        with os.fdopen(self.__readFd) as stream:
            for line in stream:
                self.__logger.log(self.__level, line.strip('\n'))

    def close(self):
        """ Close writing pipe side """

        os.close(self.__writeFd)


class SubprocessLoggerThread(threading.Thread):

    """ This class is here to log long process stdout and stderr """

    # Event used to ask all SubprocessLoggerThread object to die
    __closeEvent = threading.Event()

    def __init__(self, cmd, consoleLogger):
        """
            SubprocessLoggerThread Object initializer

            :param cmd: command to launch
            :type cmd: list
            :param consoleLogger: console log handler
            :type consoleLogger: Handler
        """

        super().__init__()

        self.__cmd = cmd
        self.__subProc = None

        self.name = "Thread : " + ' '.join(cmd)

        self.__consoleLogger = consoleLogger

        # Default logging level
        self._stdOutLogLevel = logging.DEBUG

    @classmethod
    def closeAll(cls):
        """ Set the closeEvent to ask the thread to die  """
        cls.__closeEvent.set()

    def __cleanup(self):
        """
            Close properly the child with SIGINT.

            The signal is sended to all the group to kill
            subprocess launched by Popen
        """
        os.killpg(self.__subProc.pid, signal.SIGINT)

    def __subProcPreExec(self):
        """
            Make Popen object a Group leader.

            Avoid subprocess to receive signal destinated
            to the MainThread.
        """
        os.setpgrp()

    def run(self):
        """ Create Popen object and manage it """

        # Logging threaded file-object
        stdOutLogger = StreamLoggerThread(
            self.__consoleLogger,
            self._stdOutLogLevel,
            self.name + "STDOUT")
        stdErrLogger = StreamLoggerThread(
            self.__consoleLogger,
            logging.ERROR,
            self.name + "STDERR")

        # Logging stdout and stderr through objects
        self.__subProc = subprocess.Popen(
            [os.getenv("SHELL"), "-c", ' '.join(self.__cmd)],
            bufsize=1,
            stdout=stdOutLogger,
            stderr=stdErrLogger,
            preexec_fn=self.__subProcPreExec,
            shell=False)

        # Waiting process close or closing order
        while True:
            try:
                # We end the thread if we are requested to do so
                if SubprocessLoggerThread.__closeEvent.wait(0.01):
                    self.__cleanup()
                    break

                # or if the subprocess is dead
                if self.__subProc.poll() is not None:
                    break
            except KeyboardInterrupt:
                continue

        # Close pipes
        if stdOutLogger.is_alive():
            stdOutLogger.close()
        if stdErrLogger.is_alive():
            stdErrLogger.close()


class ScriptLoggerThread(SubprocessLoggerThread):

    """ This class is used to log script subprocess """

    def __init__(self, cmd, consoleLogger):
        """
            ScriptLoggerThread Object initializer

            :param cmd: command to launch
            :type cmd: list
            :param consoleLogger: console log handler
            :type consoleLogger: Handler
        """
        super().__init__(cmd, consoleLogger)

        # Script logging level
        self._stdOutLogLevel = logging.INFO

    @classmethod
    def getRunningInstances(cls):
        """
            Running ScriptLoggerThread instances getter

            :return: The list of running ScriptLoggerThread instances
            :rtype: list
        """
        return [t for t in threading.enumerate() if isinstance(t, cls)]
