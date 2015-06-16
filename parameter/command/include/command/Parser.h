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

#include "RemoteCommandHandlerTemplate.h"
#include <string>

class CParameterMgr;

namespace core
{
namespace command
{

/** Parse remote commands and delegate actions to Parameter Manager */
class Parser
{

public:

    /** Remote command Handler type */
    typedef TRemoteCommandHandlerTemplate<Parser> CommandHandler;

    /** Class constructor
     *
     * @param parameterMgr pointer
     */
    Parser(CParameterMgr& parameterMgr);

    /** Internal command handler getter
     *
     * @return pointer on the internal command handler
     */
    CommandHandler* getCommandHandler();

private:

    /** Command handler method return type */
    using CommandReturn = CommandHandler::CommandStatus;

    //@{
    /** Command handler methods for remote commands.
     *
     * @param[in] remoteCommand contains the arguments of the received command.
     * @param[out] result a std::string containing the result of the command
     *
     * @return CommandHandler::ESucceeded if command succeeded or CommandHandler::EFailed
     * in the other case
     */
    CommandReturn version(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn status(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn setTuningMode(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn getTuningMode(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn setValueSpace(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn getValueSpace(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn setOutputRawFormat(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn getOutputRawFormat(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn setAutoSync(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn getAutoSync(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn sync(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn listCriteria(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn listDomains(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn createDomain(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn deleteDomain(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn deleteAllDomains(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn renameDomain(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn setSequenceAwareness(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn getSequenceAwareness(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn listDomainElements(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn addElement(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn removeElement(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn splitDomain(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn listConfigurations(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn dumpDomains(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn createConfiguration(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn deleteConfiguration(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn renameConfiguration(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn saveConfiguration(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn restoreConfiguration(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn setElementSequence(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn getElementSequence(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn setRule(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn clearRule(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn getRule(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn listElements(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn listParameters(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn dumpElement(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn getElementSize(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn showProperties(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn getParameter(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn setParameter(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn getConfigurationParameter(const IRemoteCommand& remoteCommand,
                                            std::string& result);

    CommandReturn setConfigurationParameter(const IRemoteCommand& remoteCommand,
                                            std::string& result);

    CommandReturn listBelongingDomains(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn listAssociatedDomains(const IRemoteCommand& remoteCommand,
                                        std::string& result);

    CommandReturn showMapping(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn listAssociatedElements(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn listConflictingElements(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn listRogueElements(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn exportConfigurableDomainsToXML(const IRemoteCommand& remoteCommand,
                                                 std::string& result);

    CommandReturn importConfigurableDomainsFromXML(const IRemoteCommand& remoteCommand,
                                                   std::string& result);

    CommandReturn exportConfigurableDomainsWithSettingsToXML(const IRemoteCommand& remoteCommand,
                                                             std::string& result);

    CommandReturn importConfigurableDomainsWithSettingsFromXML(const IRemoteCommand& remoteCommand,
                                                               std::string& result);

    CommandReturn exportConfigurableDomainWithSettingsToXML(const IRemoteCommand& remoteCommand,
                                                            std::string& result);

    CommandReturn importConfigurableDomainWithSettingsFromXML(const IRemoteCommand& remoteCommand,
                                                              std::string& result);

    CommandReturn exportSettings(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn importSettings(const IRemoteCommand& remoteCommand, std::string& result);

    CommandReturn getConfigurableDomainsWithSettingsXML(const IRemoteCommand& remoteCommand,
                                                        std::string& result);

    CommandReturn getConfigurableDomainWithSettingsXML(const IRemoteCommand& remoteCommand,
                                                       std::string& result);

    CommandReturn setConfigurableDomainsWithSettingsXML(const IRemoteCommand& remoteCommand,
                                                        std::string& result);

    CommandReturn setConfigurableDomainWithSettingsXML(const IRemoteCommand& remoteCommand,
                                                       std::string& result);

    CommandReturn getSystemClassXML(const IRemoteCommand& remoteCommand, std::string& result);
    //@}

    /** Parser items getter */
    const CommandHandler::RemoteCommandParserItems &getRemoteParserItems() const;

    /** Parameter Manager used to delegate parsed commands */
    CParameterMgr& mParameterMgr;

    /** Command Handler */
    CommandHandler mCommandHandler;
};

} /** command namespace */
} /** core namespace */
