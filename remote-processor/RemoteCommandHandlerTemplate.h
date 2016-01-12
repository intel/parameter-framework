/*
 * Copyright (c) 2011-2014, Intel Corporation
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

#include <vector>
#include "RemoteCommandHandler.h"

template <class CCommandParser>
class TRemoteCommandHandlerTemplate : public IRemoteCommandHandler
{
public:
    /** Remote command parser execution return status */
    enum CommandStatus
    {
        EDone,      /**< Command succeded, return "Done" */
        ESucceeded, /**< Command succeeded */
        EFailed,    /**< Command failed */
        EShowUsage  /**< Command failed, show usage */
    };

    /** Type of the remote command callbacks
     *
     * @param[in] remoteCommand contains the arguments of the received command.
     * @param[out] strResult a string containing the result of the command.
     *
     * @return the command execution status, @see CommandStatus
     */
    typedef CommandStatus (CCommandParser::*RemoteCommandParser)(
        const IRemoteCommand &remoteCommand, std::string &strResult);

private:
    // Parser descriptions
    class CRemoteCommandParserItem
    {
    public:
        CRemoteCommandParserItem(const std::string &strCommandName, RemoteCommandParser pfnParser,
                                 size_t minArgumentCount, const std::string &strHelp,
                                 const std::string &strDescription)
            : _strCommandName(strCommandName), _pfnParser(pfnParser),
              _minArgumentCount(minArgumentCount), _strHelp(strHelp),
              _strDescription(strDescription)
        {
        }

        const std::string &getCommandName() const { return _strCommandName; }

        const std::string &getDescription() const { return _strDescription; }

        // Usage
        std::string usage() const { return _strCommandName + " " + _strHelp; }

        bool parse(CCommandParser *pCommandParser, const IRemoteCommand &remoteCommand,
                   std::string &strResult) const
        {
            // Check enough arguments supplied
            if (remoteCommand.getArgumentCount() < _minArgumentCount) {

                strResult = std::string("Not enough arguments supplied\nUsage:\n") + usage();

                return false;
            }

            switch ((pCommandParser->*_pfnParser)(remoteCommand, strResult)) {
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
        std::string _strCommandName;
        RemoteCommandParser _pfnParser;
        size_t _minArgumentCount;
        std::string _strHelp;
        std::string _strDescription;
    };

public:
    TRemoteCommandHandlerTemplate(CCommandParser *pCommandParser)
        : _pCommandParser(pCommandParser), _maxCommandUsageLength(0)
    {
        // Help Command
        addCommandParser("help", NULL, 0, "", "Show commands description and usage");
    }
    ~TRemoteCommandHandlerTemplate()
    {
        // FIXME use unique_ptr
        for (auto *parser : _remoteCommandParserVector) {

            delete parser;
        }
    }

    // Parsers
    bool addCommandParser(const std::string &strCommandName, RemoteCommandParser pfnParser,
                          size_t minArgumentCount, const std::string &strHelp,
                          const std::string &strDescription)
    {
        if (findCommandParserItem(strCommandName)) {

            // Already exists
            return false;
        }

        // Add command
        _remoteCommandParserVector.push_back(new CRemoteCommandParserItem(
            strCommandName, pfnParser, minArgumentCount, strHelp, strDescription));

        return true;
    }

private:
    // Command processing
    bool remoteCommandProcess(const IRemoteCommand &remoteCommand, std::string &strResult)
    {
        // Dispatch
        const CRemoteCommandParserItem *pRemoteCommandParserItem =
            findCommandParserItem(remoteCommand.getCommand());

        if (!pRemoteCommandParserItem) {

            // Not found
            strResult = "Command not found!\nUse \"help\" to show available commands";

            return false;
        }

        if (remoteCommand.getCommand() == "help") {

            helpCommandProcess(strResult);

            return true;
        }

        return pRemoteCommandParserItem->parse(_pCommandParser, remoteCommand, strResult);
    }

    // Max command usage length, use for formatting
    void initMaxCommandUsageLength()
    {
        if (!_maxCommandUsageLength) {
            // Show usages
            for (const auto *pRemoteCommandParserItem : _remoteCommandParserVector) {

                size_t remoteCommandUsageLength = pRemoteCommandParserItem->usage().length();

                if (remoteCommandUsageLength > _maxCommandUsageLength) {

                    _maxCommandUsageLength = remoteCommandUsageLength;
                }
            }
        }
    }

    /////////////////// Remote command parsers
    /// Help
    void helpCommandProcess(std::string &strResult)
    {
        initMaxCommandUsageLength();

        // Show usages
        for (const auto *pRemoteCommandParserItem : _remoteCommandParserVector) {

            std::string strUsage = pRemoteCommandParserItem->usage();

            // Align
            size_t spacesToAdd = _maxCommandUsageLength + 5 - strUsage.length();

            strResult += strUsage + std::string(spacesToAdd, ' ') + "=> " +
                         pRemoteCommandParserItem->getDescription() + '\n';
        }
    }

    const CRemoteCommandParserItem *findCommandParserItem(const std::string &strCommandName) const
    {
        for (const auto *pRemoteCommandParserItem : _remoteCommandParserVector) {

            if (pRemoteCommandParserItem->getCommandName() == strCommandName) {

                return pRemoteCommandParserItem;
            }
        }
        return NULL;
    }

private:
    CCommandParser *_pCommandParser;
    std::vector<CRemoteCommandParserItem *> _remoteCommandParserVector;
    size_t _maxCommandUsageLength;
};
