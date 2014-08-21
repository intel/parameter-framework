#
# INTEL CONFIDENTIAL
# Copyright  2014 Intel
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
#

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
                self.name+"STDOUT")
        stdErrLogger = StreamLoggerThread(
                self.__consoleLogger,
                logging.ERROR,
                self.name+"STDERR")

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
                if self.__subProc.poll() != None:
                    break
            except KeyboardInterrupt:
                continue

        # Close pipes
        if stdOutLogger.is_alive(): stdOutLogger.close()
        if stdErrLogger.is_alive(): stdErrLogger.close()

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
        return [t for t in threading.enumerate() if type(t) is cls]
