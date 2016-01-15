/*
 * Copyright (c) 2015, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once

#include <string>
#include <vector>

/** Class used to send commands to a parameter framework.
 * @see the help command for more information on which command can be sent.
 * @see ParameterMgrFullConnector::createCommandHandler to create an instance.
 *
 * This interface is primary designed to send commands without using a
 * tcp socket for test purposes.
 *
 * Note: the fact that this class must be deleted by the client is because the
 *       PF interface is not c++11.
 * TODO: When the interface will transition to C++11, return directly the
 *       CommandHandlerWrapper as this base class only use is to hide the
 *       move semantic that is not supported in C++03.
 */
class CommandHandlerInterface
{
public:
    /** Send a command synchronously and receive it's result.
     *
     * @see CParameterMgr::gastRemoteCommandParserItems for the list of possible
     *      command and their description.
     *
     * @param[in] command the command to execute.
     * @param[in] arguments the command arguments.
     * @param[out] output the result of the command.
     *
     * return true in the command executed succesfuly,
     *        false otherwise.
     */
    virtual bool process(const std::string &command, const std::vector<std::string> &arguments,
                         std::string &output) = 0;

    virtual ~CommandHandlerInterface(){};
};
