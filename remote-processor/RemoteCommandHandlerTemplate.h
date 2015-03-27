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
#include <functional>

/** Handle command of server side application
 *
 * @tparam CommandParser type of the parser used to handle remote commands
 */
template <class CommandParser>
class RemoteCommandHandlerTemplate : public IRemoteCommandHandler
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
    using RemoteCommandParser = std::function<CommandStatus(CommandParser&,
                                                            const IRemoteCommand&,
                                                            std::string&)>;

    /** Parser item definition */
    class RemoteCommandParserItem
    {
    public:

        /**
         * @param parser command handler callback
         * @param minArgumentCount command required arguments number
         * @param help string containing command help
         * @param description string containing command description
         */
        RemoteCommandParserItem(RemoteCommandParser parser,
                                uint32_t minArgumentCount,
                                const std::string& help,
                                const std::string& description) :
            mParser(parser),
            mMinArgumentCount(minArgumentCount),
            mHelp(help),
            mDescription(description)
        {
        }

        /** Command description string getter
         *
         * @return the command description string
         */
        const std::string& getDescription() const
        {
            return mDescription;
        }

        /** Command help string getter
         *
         * @return the command help string
         */
        const std::string getHelp() const
        {
            return mHelp;
        }

        /** Parse and launch a remote command
         *
         * @param[in] commandParser the parser used to handle the command
         * @param[in] remoteCommand the command received from client
         * @param[out] result the command result
         * @result true on success, false otherwise
         */
        bool parse(CommandParser& commandParser,
                   const IRemoteCommand& remoteCommand,
                   std::string& result) const
        {
            // Check enough arguments supplied
            if (remoteCommand.getArgumentCount() < mMinArgumentCount) {

                result = std::string("Not enough arguments supplied\nUsage:\n") + getHelp();

                return false;
            }

            switch (mParser(commandParser, remoteCommand, result)) {
            case EDone:
                result = "Done";
            // Fall through intentionally
            case ESucceeded:
                return true;
            case EShowUsage:
                result = getHelp();
            // Fall through intentionally
            case EFailed:
                return false;
            }

            return false;
        }

    private:

        /** Command Handler callback */
        RemoteCommandParser mParser;

        /** Needed arguments number */
        uint32_t mMinArgumentCount;

        /** Command help string */
        std::string mHelp;

        /** Command description string */
        std::string mDescription;
    };

    /** Remote command parser container type */
    typedef std::map<std::string, RemoteCommandParserItem> RemoteCommandParserItems;

    /**
     * @param commandParser command parser used for command handling
     * @param remoteCommandParserItems supported command parser items
     */
    RemoteCommandHandlerTemplate(CommandParser& commandParser,
                                 RemoteCommandParserItems& remoteCommandParserItems) :
        _commandParser(commandParser), _remoteCommandParserItems(remoteCommandParserItems)
    {
        /* Add the help command and specialize the help function
         * to match RemoteCommandParser prototype
         */
        _remoteCommandParserItems.emplace(
                "help",
                RemoteCommandParserItem(
                    [this](CommandParser&,
                           const IRemoteCommand&, std::string& result){ return help(result); },
                    0, "", "Show commands description and usage"));
    }

private:

    /** Handle a remote command
     *
     * @param[in] remoteCommand the remote command to handle
     * @param[out] result string containing the result of the handled command
     * @result true on success, false otherwise
     */
    bool remoteCommandProcess(const IRemoteCommand& remoteCommand, std::string& result)
    {
        try {
            // Dispatch
            const RemoteCommandParserItem& remoteCommandParserItem =
                _remoteCommandParserItems.at(remoteCommand.getCommand());

            return remoteCommandParserItem.parse(_commandParser, remoteCommand, result);
        }
        catch (const std::out_of_range&) {
            // Not found
            result = "Command not found!\nUse \"help\" to show available commands";

            return false;
        }
    }

    /** Format help display
     *
     * @param result the formatted help string
     *
     * @return ESucceeded command status
     */
    CommandStatus help(std::string& result)
    {
        struct Help { std::string usage; std::string description; };
        std::vector<Help> helps;
        size_t maxUsage = 0;

        for (auto& item : _remoteCommandParserItems) {
            std::string usage = item.first + ' ' + item.second.getHelp();
            helps.push_back({ usage, item.second.getDescription() });
            maxUsage = std::max(maxUsage, usage.length());
        }

        for (auto& help : helps) {
            help.usage.resize(maxUsage, ' ');
            result += help.usage + " => " + help.description + '\n';
        }
        return CommandStatus::ESucceeded;
    }

    /** Command parser used during command during command handling */
    CommandParser& _commandParser;

    /** Remote command parser map */
    RemoteCommandParserItems& _remoteCommandParserItems;
};
