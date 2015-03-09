/*
 * Copyright (c) 2011-2015, Intel Corporation
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

#include "RemoteCommandHandler.h"
#include <vector>
#include <map>

template <class CCommandParser>
class TRemoteCommandHandlerTemplate : public IRemoteCommandHandler
{
public:
    /** Remote command parser execution return status */
    enum CommandStatus {
        EDone, /**< Command succeded, return "Done" */
        ESucceeded, /**< Command succeeded */
        EFailed, /**< Command failed */
        EShowUsage /**< Command failed, show usage */
    };

    /** Type of the remote command callbacks
     *
     * @param[in] remoteCommand contains the arguments of the received command.
     * @param[out] strResult a string containing the result of the command.
     *
     * @return the command execution status, @see CommandStatus
     */
    typedef CommandStatus (CCommandParser::*RemoteCommandParser)(const IRemoteCommand& remoteCommand, std::string& strResult);

    // Parser descriptions
    class RemoteCommandParserItem
    {
    public:
        RemoteCommandParserItem(RemoteCommandParser pfnParser,
                                uint32_t uiMinArgumentCount,
                                const std::string& strHelp,
                                const std::string& strDescription) :
            _pfnParser(pfnParser),
            _uiMinArgumentCount(uiMinArgumentCount),
            _strHelp(strHelp),
            _strDescription(strDescription) {}

        const std::string& getDescription() const
        {
            return _strDescription;
        }

        // Usage
        std::string usage() const
        {
            return _strHelp;
        }

        /** Parse and launch a remote command
         *
         * @param[in] commandParser the parser used to handle the command
         * @param[in] remoteCommand the command received from client
         * @param[out] strResult the command result
         * @result true on success, false otherwise
         */
        bool parse(CCommandParser& commandParser,
                   const IRemoteCommand& remoteCommand,
                   std::string& strResult) const
        {
            // Check enough arguments supplied
            if (remoteCommand.getArgumentCount() < _uiMinArgumentCount) {

                strResult = std::string("Not enough arguments supplied\nUsage:\n") + usage();

                return false;
            }

            switch ((commandParser.*_pfnParser)(remoteCommand, strResult)) {
            case EDone:
                strResult = "Done";
                // Fall through intentionally
            case ESucceeded:
                return true;
            case EShowUsage:
                strResult = usage();
                // Fall through intentionally
            case EFailed:
                return false;
            }

            return false;
        }

    private:
        RemoteCommandParser _pfnParser;
        uint32_t _uiMinArgumentCount;
        std::string _strHelp;
        std::string _strDescription;
    };

public:

    /** Remote command parser container type */
    typedef std::map<std::string, RemoteCommandParserItem> RemoteCommandParserItems;

    /**
     * @param commandParser command parser used for command handling
     * @param remoteCommandParserItems supported command parser items
     */
    TRemoteCommandHandlerTemplate(CCommandParser& commandParser,
                                  const RemoteCommandParserItems& remoteCommandParserItems) :
        _commandParser(commandParser), _remoteCommandParserItems(remoteCommandParserItems)
    {
    }

private:
    // Command processing
    bool remoteCommandProcess(const IRemoteCommand& remoteCommand, std::string& strResult)
    {
        try {
            // Dispatch
            const RemoteCommandParserItem& remoteCommandParserItem =
                _remoteCommandParserItems.at(remoteCommand.getCommand());

            return remoteCommandParserItem.parse(_commandParser, remoteCommand, strResult);
        }
        catch (const std::out_of_range&) {

            if (remoteCommand.getCommand() == helpCommand) {

                help(strResult);

                return true;
            }

            // Not found
            strResult = "Command not found!\nUse \"help\" to show available commands";

            return false;
        }
    }

    /** Format help display
     *
     * @param result the formatted help string
     */
    void help(std::string& result)
    {
        struct Help { std::string usage; std::string description; };
        std::vector<Help> helps{ { helpCommand, helpCommandDescription } };
        size_t maxUsage = helpCommand.length();

        for (auto& item : _remoteCommandParserItems) {
            std::string usage = item.first + ' ' + item.second.usage();
            helps.push_back({ usage, item.second.getDescription() });
            maxUsage = std::max(maxUsage, usage.length());
        }

        for (auto& help : helps) {
            help.usage.resize(maxUsage, ' ');
            result += help.usage + " => " + help.description + '\n';
        }
    }

    /** Help command name */
    static const std::string helpCommand;

    /** Help command description */
    static const std::string helpCommandDescription;

    /** Command parser used during command during command handling */
    CCommandParser& _commandParser;

    /** Remote command parser map */
    const RemoteCommandParserItems& _remoteCommandParserItems;
};

template <typename CommandParser>
const std::string TRemoteCommandHandlerTemplate<CommandParser>::helpCommand = "help";

template <typename CommandParser>
const std::string TRemoteCommandHandlerTemplate<CommandParser>::helpCommandDescription =
    "Show commands description and usage";
