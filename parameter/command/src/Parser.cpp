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
#include "command/Parser.h"
#include "ParameterMgr.h"
#include "ConfigurableDomains.h"
#include "ElementLocator.h"
#include "ErrorContext.h"
#include "ParameterAccessContext.h"
#include "SystemClass.h"
#include "Utility.h"
#include <algorithm>
#include <ctype.h>
#include <list>

namespace core
{
namespace command
{

Parser::Parser(CParameterMgr& parameterMgr) :
    mParameterMgr(parameterMgr), mCommandHandler(CommandHandler(*this, getRemoteParserItems()))
{
}

Parser::CommandHandler::RemoteCommandParserItems &Parser::getRemoteParserItems() const
{
    static CommandHandler::RemoteCommandParserItems remoteCommandParserItems = {
        { "version",
          { &Parser::version, 0, "",
            "Show version" } },
        { "status",
          { &Parser::status, 0, "",
            "Show current status" } },
        { "setTuningMode",
          { &Parser::setTuningMode, 1, "on|off*",
            "Turn on or off Tuning Mode" } },
        { "getTuningMode",
          { &Parser::getTuningMode, 0, "",
            "Show Tuning Mode" } },
        { "setValueSpace",
          { &Parser::setValueSpace, 1, "raw|real*",
            "Assigns Value Space used for parameter value interpretation" } },
        { "getValueSpace",
          { &Parser::getValueSpace, 0, "",
            "Show Value Space" } },
        { "setOutputRawFormat",
          { &Parser::setOutputRawFormat, 1, "dec*|hex",
            "Assigns format used to output parameter values when in raw Value Space" } },
        { "getOutputRawFormat",
          { &Parser::getOutputRawFormat, 0, "",
            "Show Output Raw Format" } },
        { "setAutoSync",
          { &Parser::setAutoSync, 1, "on*|off",
            "Turn on or off automatic synchronization to hardware while in Tuning Mode" } },
        { "getAutoSync",
          { &Parser::getAutoSync, 0, "",
            "Show Auto Sync state" } },
        { "sync",
          { &Parser::sync, 0, "",
            "Synchronize current settings to hardware while in Tuning Mode and Auto Sync off" } },
        { "listCriteria",
          { &Parser::listCriteria, 0, "[CSV|XML]",
            "List selection criteria" } },
        { "listDomains",
          { &Parser::listDomains, 0, "",
            "List configurable domains" } },
        { "dumpDomains",
          { &Parser::dumpDomains, 0, "",
            "Show all domains and configurations, including applicability conditions" } },
        { "createDomain",
          { &Parser::createDomain, 1, "<domain>",
            "Create new configurable domain" } },
        { "deleteDomain",
          { &Parser::deleteDomain, 1, "<domain>",
            "Delete configurable domain" } },
        { "deleteAllDomains",
          { &Parser::deleteAllDomains, 0, "",
            "Delete all configurable domains" } },
        { "renameDomain",
          { &Parser::renameDomain, 2, "<domain> <new name>",
            "Rename configurable domain" } },
        { "setSequenceAwareness",
          { &Parser::setSequenceAwareness, 1, "<domain> true|false*",
            "Set configurable domain sequence awareness" } },
        { "getSequenceAwareness",
          { &Parser::getSequenceAwareness, 1, "<domain>",
            "Get configurable domain sequence awareness" } },
        { "listDomainElements",
          { &Parser::listDomainElements, 1, "<domain>",
            "List elements associated to configurable domain" } },
        { "addElement",
          { &Parser::addElement, 2, "<domain> <elem path>",
            "Associate element at given path to configurable domain" } },
        { "removeElement",
          { &Parser::removeElement, 2, "<domain> <elem path>",
            "Dissociate element at given path from configurable domain" } },
        { "splitDomain",
          { &Parser::splitDomain, 2, "<domain> <elem path>",
            "Split configurable domain at given associated element path" } },
        { "listConfigurations",
          { &Parser::listConfigurations, 1, "<domain>",
            "List domain configurations" } },
        { "createConfiguration",
          { &Parser::createConfiguration, 2, "<domain> <configuration>",
            "Create new domain configuration" } },
        { "deleteConfiguration",
          { &Parser::deleteConfiguration, 2, "<domain> <configuration>",
            "Delete domain configuration" } },
        { "renameConfiguration",
          { &Parser::renameConfiguration, 3, "<domain> <configuration> <new name>",
            "Rename domain configuration" } },
        { "saveConfiguration",
          { &Parser::saveConfiguration, 2, "<domain> <configuration>",
            "Save current settings into configuration" } },
        { "restoreConfiguration",
          { &Parser::restoreConfiguration, 2, "<domain> <configuration>",
            "Restore current settings from configuration" } },
        { "setElementSequence",
          { &Parser::setElementSequence, 3, "<domain> <configuration> <elem path list>",
            "Set element application order for configuration" } },
        { "getElementSequence",
          { &Parser::getElementSequence, 2, "<domain> <configuration>",
            "Get element application order for configuration" } },
        { "setRule",
          { &Parser::setRule, 3, "<domain> <configuration> <rule>",
            "Set configuration application rule" } },
        { "clearRule",
          { &Parser::clearRule, 2, "<domain> <configuration>",
            "Clear configuration application rule" } },
        { "getRule",
          { &Parser::getRule, 2, "<domain> <configuration>",
            "Get configuration application rule" } },
        { "listElements",
          { &Parser::listElements, 1, "<elem path>|/",
            "List elements under element at given path or root" } },
        { "listParameters",
          { &Parser::listParameters, 1, "<elem path>|/",
            "List parameters under element at given path or root" } },
        { "dumpElement",
          { &Parser::dumpElement, 1, "<elem path>",
            "Dump structure and content of element at given path" } },
        { "getElementSize",
          { &Parser::getElementSize, 1, "<elem path>",
            "Show size of element at given path" } },
        { "showProperties",
          { &Parser::showProperties, 1, "<elem path>",
            "Show properties of element at given path" } },
        { "getParameter",
          { &Parser::getParameter, 1, "<param path>",
            "Get value for parameter at given path" } },
        { "setParameter",
          { &Parser::setParameter, 2, "<param path> <value>",
            "Set value for parameter at given path" } },
        { "listBelongingDomains",
          { &Parser::listBelongingDomains, 1, "<elem path>",
            "List domain(s) element at given path belongs to" } },
        { "listAssociatedDomains",
          { &Parser::listAssociatedDomains, 1, "<elem path>",
            "List domain(s) element at given path is associated to" } },
        { "getConfigurationParameter",
          { &Parser::getConfigurationParameter, 3, "<domain> <configuration> <param path>",
            "Get value for parameter at given path from configuration" } },
        { "setConfigurationParameter",
          { &Parser::setConfigurationParameter, 4, "<domain> <configuration> <param path> <value>",
            "Set value for parameter at given path to configuration" } },
        { "showMapping",
          { &Parser::showMapping, 1, "<elem path>",
            "Show mapping for an element at given path" } },
        { "listAssociatedElements",
          { &Parser::listAssociatedElements, 0, "",
            "List element sub-trees associated to at least one configurable domain" } },
        { "listConflictingElements",
          { &Parser::listConflictingElements, 0, "",
            "List element sub-trees contained in more than one configurable domain" } },
        { "listRogueElements",
          { &Parser::listRogueElements, 0, "",
            "List element sub-trees owned by no configurable domain" } },
        { "exportDomainsXML",
          { &Parser::exportConfigurableDomainsToXML, 1, "<file path> ",
            "Export domains to XML file (provide an absolute path or relative to the client's working"
            " directory)" } },
        { "importDomainsXML",
          { &Parser::importConfigurableDomainsFromXML, 1, "<file path>",
            "Import domains from XML file (provide an absolute path or relative to the client's "
            "working directory)" } },
        { "exportDomainsWithSettingsXML",
          { &Parser::exportConfigurableDomainsWithSettingsToXML, 1, "<file path> ",
            "Export domains including settings to XML file (provide an absolute path or relative to"
            " the client's working directory)" } },
        { "importDomainsWithSettingsXML",
          { &Parser::importConfigurableDomainsWithSettingsFromXML, 1, "<file path>",
            "Import domains including settings from XML file (provide an absolute path or relative to"
            " the client's working directory)" } },
        { "exportDomainWithSettingsXML",
          { &Parser::exportConfigurableDomainWithSettingsToXML, 1, "<domain> <file path>",
            "Export a single given domain including settings to an XML file (provide an absolute path"
            " or relative to the client's working directory)" } },
        { "importDomainWithSettingsXML",
          { &Parser::importConfigurableDomainWithSettingsFromXML, 1, "<file path> [overwrite]",
            "Import a single domain including settings from XML file."
            " Does not overwrite an existing domain unless 'overwrite' is passed as second"
            " argument (provide an absolute path or relative to the client's working directory)" } },
        { "exportSettings",
          { &Parser::exportSettings, 1, "<file path>",
            "Export settings to binary file (provide an absolute path or relative to the client's "
            "working directory)" } },
        { "importSettings",
          { &Parser::importSettings, 1, "<file path>",
            "Import settings from binary file (provide an absolute path or relative to the client's "
            "working directory)" } },
        { "getDomainsWithSettingsXML",
          { &Parser::getConfigurableDomainsWithSettingsXML, 0, "",
            "Print domains including settings as XML" } },
        { "getDomainWithSettingsXML",
          { &Parser::getConfigurableDomainWithSettingsXML, 1, "<domain>",
            "Print the given domain including settings as XML" } },
        { "setDomainsWithSettingsXML",
          { &Parser::setConfigurableDomainsWithSettingsXML, 1, "<xml configurable domains>",
            "Import domains including settings from XML string" } },
        { "setDomainWithSettingsXML",
          { &Parser::setConfigurableDomainWithSettingsXML, 1, "<xml configurable domain> [overwrite]",
            "Import a single domain including settings from an XML string. Does not overwrite an"
            " existing domain unless 'overwrite' is passed as second argument" } },
        { "getSystemClassXML",
          { &Parser::getSystemClassXML, 0, "",
            "Print parameter structure as XML" } }
    };

    return remoteCommandParserItems;
}

Parser::CommandHandler* Parser::getCommandHandler()
{
    return &mCommandHandler;
}

Parser::CommandReturn Parser::version(const IRemoteCommand&, std::string& result)
{
    // Show version
    result = mParameterMgr.getVersion();

    return CommandHandler::ESucceeded;
}

Parser::CommandReturn Parser::status(const IRemoteCommand&, std::string& result)
{
    // System class
    const CSystemClass &systemClass = mParameterMgr._systemClass;

    // Show status
    /// General section
    CUtility::appendTitle(result, "General:");
    // System class
    result += "System Class: ";
    result += systemClass.getName();
    result += "\n";

    // Tuning mode
    result += "Tuning Mode: ";
    result += mParameterMgr.tuningModeOn() ? "on" : "off";
    result += "\n";

    // Value space
    result += "Value Space: ";
    result += mParameterMgr.valueSpaceIsRaw() ? "raw" : "real";
    result += "\n";

    // Output raw format
    result += "Output Raw Format: ";
    result += mParameterMgr.outputRawFormatIsHex() ? "hex" : "dec";
    result += "\n";

    // Auto Sync
    result += "Auto Sync: ";
    result += mParameterMgr.autoSyncOn() ? "on" : "off";
    result += "\n";

    /// Subsystem list
    CUtility::appendTitle(result, "Subsystems:");
    std::string strSubsystemList;
    systemClass.listChildrenPaths(strSubsystemList);
    result += strSubsystemList;

    /// Last applied configurations
    CUtility::appendTitle(result, "Last Applied [Pending] Configurations:");
    std::string strLastAppliedConfigurations;
    mParameterMgr._domains.listLastAppliedConfigurations(
        strLastAppliedConfigurations);
    result += strLastAppliedConfigurations;

    /// Criteria states
    CUtility::appendTitle(result, "Selection Criteria:");
    std::list<std::string> lstrSelectionCriteria;
    mParameterMgr._criteria.listCriteria(lstrSelectionCriteria, false, true);
    // Concatenate the criterion list as the command result
    std::string strCriteriaStates;
    CUtility::asString(lstrSelectionCriteria, strCriteriaStates);
    result += strCriteriaStates;

    return CommandHandler::ESucceeded;
}

Parser::CommandReturn Parser::setTuningMode(const IRemoteCommand& remoteCommand,
                                            std::string& result)
{
    if (remoteCommand.getArgument(0) == "on") {

        if (mParameterMgr.setTuningMode(true, result)) {

            return CommandHandler::EDone;
        }
    } else if (remoteCommand.getArgument(0) == "off") {

        if (mParameterMgr.setTuningMode(false, result)) {

            return CommandHandler::EDone;
        }
    } else {
        // Show usage
        return CommandHandler::EShowUsage;
    }
    return CommandHandler::EFailed;
}

Parser::CommandReturn Parser::getTuningMode(const IRemoteCommand&, std::string& result)
{
    result = mParameterMgr.tuningModeOn() ? "on" : "off";
    return CommandHandler::ESucceeded;
}

Parser::CommandReturn Parser::setValueSpace(const IRemoteCommand& remoteCommand, std::string&)
{
    if (remoteCommand.getArgument(0) == "raw") {

        mParameterMgr.setValueSpace(true);

        return CommandHandler::EDone;

    } else if (remoteCommand.getArgument(0) == "real") {

        mParameterMgr.setValueSpace(false);

        return CommandHandler::EDone;

    } else {
        // Show usage
        return CommandHandler::EShowUsage;
    }
    return CommandHandler::EFailed;
}

Parser::CommandReturn Parser::getValueSpace(const IRemoteCommand&, std::string& result)
{
    result = mParameterMgr.valueSpaceIsRaw() ? "raw" : "real";
    return CommandHandler::ESucceeded;
}

Parser::CommandReturn Parser::setOutputRawFormat(const IRemoteCommand& remoteCommand, std::string&)
{
    if (remoteCommand.getArgument(0) == "hex") {

        mParameterMgr.setOutputRawFormat(true);

        return CommandHandler::EDone;

    } else if (remoteCommand.getArgument(0) == "dec") {

        mParameterMgr.setOutputRawFormat(false);

        return CommandHandler::EDone;

    } else {
        // Show usage
        return CommandHandler::EShowUsage;
    }
    return CommandHandler::EFailed;
}

Parser::CommandReturn Parser::getOutputRawFormat(const IRemoteCommand&, std::string& result)
{
    result = mParameterMgr.outputRawFormatIsHex() ? "hex" : "dec";
    return CommandHandler::ESucceeded;
}

Parser::CommandReturn Parser::setAutoSync(const IRemoteCommand& remoteCommand,
                                          std::string& result)
{
    if (remoteCommand.getArgument(0) == "on") {

        if (mParameterMgr.setAutoSync(true, result)) {

            return CommandHandler::EDone;
        }
    } else if (remoteCommand.getArgument(0) == "off") {

        if (mParameterMgr.setAutoSync(false, result)) {

            return CommandHandler::EDone;
        }
    } else {
        // Show usage
        return CommandHandler::EShowUsage;
    }
    return CommandHandler::EFailed;
}

Parser::CommandReturn Parser::getAutoSync(const IRemoteCommand&, std::string& result)
{
    result = mParameterMgr.autoSyncOn() ? "on" : "off";
    return CommandHandler::ESucceeded;
}

Parser::CommandReturn Parser::sync(const IRemoteCommand&, std::string& result)
{
    return mParameterMgr.sync(result) ? CommandHandler::EDone : CommandHandler::EFailed;
}

Parser::CommandReturn Parser::listCriteria(const IRemoteCommand& remoteCommand,
                                           std::string& result)
{
    if (remoteCommand.getArgumentCount() > 1) {

        return CommandHandler::EShowUsage;
    }

    std::string format;
    // Look for optional arguments
    if (remoteCommand.getArgumentCount() == 1) {

        // Get requested format
        format = remoteCommand.getArgument(0);
        // Capitalize
        std::transform(format.begin(), format.end(), format.begin(), ::toupper);
        if (format != "XML" && format != "CSV") {
            return CommandHandler::EShowUsage;
        }
    }

    if (format == "XML") {
        if (!mParameterMgr.exportElementToXMLString(
                &mParameterMgr._criteria, "SelectionCriteria", result)) {

            return CommandHandler::EFailed;
        }
        // Succeeded
        return CommandHandler::ESucceeded;
    } else {

        // Requested format will be either CSV or human readable based on format content
        bool bHumanReadable = format.empty();

        std::list<std::string> results;
        mParameterMgr._criteria.listCriteria(results, true, bHumanReadable);

        // Concatenate the criterion list as the command result
        CUtility::asString(results, result);
        return CommandHandler::ESucceeded;
    }
}

Parser::CommandReturn Parser::listDomains(const IRemoteCommand&, std::string& result)
{
    mParameterMgr._domains.listDomains(result);
    return CommandHandler::ESucceeded;
}

Parser::CommandReturn Parser::createDomain(const IRemoteCommand& remoteCommand,
                                           std::string& result)
{
    return mParameterMgr.createDomain(remoteCommand.getArgument(0),
                                      result) ? CommandHandler::EDone : CommandHandler::EFailed;
}

Parser::CommandReturn Parser::deleteDomain(const IRemoteCommand& remoteCommand,
                                           std::string& result)
{
    return mParameterMgr.deleteDomain(remoteCommand.getArgument(0),
                                      result) ? CommandHandler::EDone : CommandHandler::EFailed;
}

Parser::CommandReturn Parser::deleteAllDomains(const IRemoteCommand&, std::string& result)
{
    return mParameterMgr.deleteAllDomains(result) ? CommandHandler::EDone : CommandHandler::EFailed;
}

Parser::CommandReturn Parser::renameDomain(const IRemoteCommand& remoteCommand,
                                           std::string& result)
{
    return mParameterMgr.renameDomain(remoteCommand.getArgument(0),
                                      remoteCommand.getArgument(1),
                                      result) ? CommandHandler::EDone : CommandHandler::EFailed;
}

Parser::CommandReturn
Parser::setSequenceAwareness(const IRemoteCommand& remoteCommand, std::string& result)
{
    // Set property
    bool bSequenceAware;

    if (remoteCommand.getArgument(1) == "true") {

        bSequenceAware = true;

    } else if (remoteCommand.getArgument(1) == "false") {

        bSequenceAware = false;

    } else {
        // Show usage
        return CommandHandler::EShowUsage;
    }

    return mParameterMgr.setSequenceAwareness(remoteCommand.getArgument(0),
                                              bSequenceAware,
                                              result) ?
           CommandHandler::EDone : CommandHandler::EFailed;
}

Parser::CommandReturn
Parser::getSequenceAwareness(const IRemoteCommand& remoteCommand, std::string& result)
{
    // Get property
    bool bSequenceAware;

    if (!mParameterMgr.getSequenceAwareness(remoteCommand.getArgument(0), bSequenceAware, result)) {
        return CommandHandler::EFailed;
    }
    result = bSequenceAware ? "true" : "false";
    return CommandHandler::ESucceeded;
}

Parser::CommandReturn Parser::listDomainElements(const IRemoteCommand& remoteCommand,
                                                 std::string& result)
{
    return mParameterMgr._domains.listDomainElements(remoteCommand.getArgument(0),
                                                                      result) ?
           CommandHandler::ESucceeded : CommandHandler::EFailed;
}

Parser::CommandReturn Parser::addElement(const IRemoteCommand& remoteCommand,
                                         std::string& result)
{
    return mParameterMgr.addConfigurableElementToDomain(remoteCommand.getArgument(0),
                                                        remoteCommand.getArgument(1),
                                                        result) ?
           CommandHandler::EDone : CommandHandler::EFailed;
}

Parser::CommandReturn Parser::removeElement(const IRemoteCommand& remoteCommand,
                                            std::string& result)
{
    return mParameterMgr.removeConfigurableElementFromDomain(remoteCommand.getArgument(0),
                                                             remoteCommand.getArgument(1),
                                                             result) ?
           CommandHandler::EDone : CommandHandler::EFailed;
}

Parser::CommandReturn Parser::splitDomain(const IRemoteCommand& remoteCommand,
                                          std::string& result)
{
    return mParameterMgr.split(remoteCommand.getArgument(0),
                               remoteCommand.getArgument(1),
                               result) ? CommandHandler::EDone : CommandHandler::EFailed;
}

Parser::CommandReturn Parser::listConfigurations(const IRemoteCommand& remoteCommand,
                                                 std::string& result)
{
    return mParameterMgr._domains.listConfigurations(
               remoteCommand.getArgument(0), result) ?
           CommandHandler::ESucceeded : CommandHandler::EFailed;
}

Parser::CommandReturn Parser::dumpDomains(const IRemoteCommand&, std::string& result)
{
    // Dummy error context
    std::string strError;
    CErrorContext errorContext(strError);
    // Dump
    mParameterMgr._domains.dumpContent(result, errorContext);
    return CommandHandler::ESucceeded;
}

Parser::CommandReturn Parser::createConfiguration(const IRemoteCommand& remoteCommand,
                                                  std::string& result)
{
    return mParameterMgr.createConfiguration(remoteCommand.getArgument(0),
                                             remoteCommand.getArgument(1),
                                             result) ?
           CommandHandler::EDone : CommandHandler::EFailed;
}

Parser::CommandReturn Parser::deleteConfiguration(const IRemoteCommand& remoteCommand,
                                                  std::string& result)
{
    return mParameterMgr.deleteConfiguration(remoteCommand.getArgument(0),
                                             remoteCommand.getArgument(1),
                                             result) ?
           CommandHandler::EDone : CommandHandler::EFailed;
}

Parser::CommandReturn Parser::renameConfiguration(const IRemoteCommand& remoteCommand,
                                                  std::string& result)
{
    return mParameterMgr.renameConfiguration(remoteCommand.getArgument(0),
                                             remoteCommand.getArgument(1),
                                             remoteCommand.getArgument(2),
                                             result) ?
           CommandHandler::EDone : CommandHandler::EFailed;
}

Parser::CommandReturn Parser::saveConfiguration(const IRemoteCommand& remoteCommand,
                                                std::string& result)
{
    return mParameterMgr.saveConfiguration(remoteCommand.getArgument(0),
                                           remoteCommand.getArgument(1),
                                           result) ?
           CommandHandler::EDone : CommandHandler::EFailed;
}

Parser::CommandReturn
Parser::restoreConfiguration(const IRemoteCommand& remoteCommand, std::string& result)
{
    core::Results results;
    if (!mParameterMgr.restoreConfiguration(
            remoteCommand.getArgument(0), remoteCommand.getArgument(1), results)) {
        // Concatenate the error list as the command result
        CUtility::asString(results, result);
        return CommandHandler::EFailed;
    }
    return CommandHandler::EDone;
}

Parser::CommandReturn Parser::setElementSequence(const IRemoteCommand& remoteCommand,
                                                 std::string& result)
{
    // Build configurable element path list
    std::vector<std::string> astrNewElementSequence;

    uint32_t uiArgument;

    for (uiArgument = 2; uiArgument < remoteCommand.getArgumentCount(); uiArgument++) {

        astrNewElementSequence.push_back(remoteCommand.getArgument(uiArgument));
    }

    // Delegate to configurable domains
    return mParameterMgr.setElementSequence(remoteCommand.getArgument(0),
                                            remoteCommand.getArgument(1),
                                            astrNewElementSequence,
                                            result) ?
           CommandHandler::EDone : CommandHandler::EFailed;
}

Parser::CommandReturn Parser::getElementSequence(const IRemoteCommand& remoteCommand,
                                                 std::string& result)
{
    // Delegate to configurable domains
    return mParameterMgr._domains.getElementSequence(remoteCommand.getArgument(0),
                                                                      remoteCommand.getArgument(1),
                                                                      result) ?
           CommandHandler::ESucceeded : CommandHandler::EFailed;
}

Parser::CommandReturn Parser::setRule(const IRemoteCommand& remoteCommand,
                                      std::string& result)
{
    // Delegate to configurable domains
    return mParameterMgr.setApplicationRule(remoteCommand.getArgument(0),
                                            remoteCommand.getArgument(1),
                                            remoteCommand.packArguments(
                                                2, remoteCommand.getArgumentCount() - 2),
                                            result) ?
           CommandHandler::EDone : CommandHandler::EFailed;
}

Parser::CommandReturn Parser::clearRule(const IRemoteCommand& remoteCommand,
                                        std::string& result)
{
    // Delegate to configurable domains
    return mParameterMgr.clearApplicationRule(remoteCommand.getArgument(0),
                                              remoteCommand.getArgument(1),
                                              result) ?
           CommandHandler::EDone : CommandHandler::EFailed;
}

Parser::CommandReturn Parser::getRule(const IRemoteCommand& remoteCommand,
                                      std::string& result)
{
    // Delegate to configurable domains
    return mParameterMgr.getApplicationRule(remoteCommand.getArgument(0),
                                            remoteCommand.getArgument(1),
                                            result) ?
           CommandHandler::ESucceeded : CommandHandler::EFailed;
}

Parser::CommandReturn Parser::listElements(const IRemoteCommand& remoteCommand,
                                           std::string& result)
{
    CElementLocator elementLocator(&mParameterMgr._systemClass, false);
    CElement* pLocatedElement = NULL;

    if (!elementLocator.locate(remoteCommand.getArgument(0), &pLocatedElement, result)) {
        return CommandHandler::EFailed;
    }

    result = std::string("\n");
    if (!pLocatedElement) {
        // List from root folder
        // Return system class qualified name
        pLocatedElement = &mParameterMgr._systemClass;
    }
    // Return sub-elements
    result += pLocatedElement->listQualifiedPaths(false);
    return CommandHandler::ESucceeded;
}

Parser::CommandReturn Parser::listParameters(const IRemoteCommand& remoteCommand,
                                             std::string& result)
{
    CElementLocator elementLocator(&mParameterMgr._systemClass, false);
    CElement* pLocatedElement = NULL;

    if (!elementLocator.locate(remoteCommand.getArgument(0), &pLocatedElement, result)) {
        return CommandHandler::EFailed;
    }

    result = std::string("\n");
    if (!pLocatedElement) {
        // List from root folder
        // Return system class qualified name
        pLocatedElement = &mParameterMgr._systemClass;
    }
    // Return sub-elements
    result += pLocatedElement->listQualifiedPaths(true);
    return CommandHandler::ESucceeded;
}

Parser::CommandReturn Parser::dumpElement(const IRemoteCommand& remoteCommand,
                                          std::string& result)
{
    CElementLocator elementLocator(&mParameterMgr._systemClass);
    CElement* pLocatedElement = NULL;

    if (!elementLocator.locate(remoteCommand.getArgument(0), &pLocatedElement, result)) {
        return CommandHandler::EFailed;
    }

    std::string strError;
    CParameterAccessContext parameterAccessContext(strError,
                                                   mParameterMgr._pMainParameterBlackboard,
                                                   mParameterMgr._bValueSpaceIsRaw,
                                                   mParameterMgr._bOutputRawFormatIsHex);
    // Dump elements
    pLocatedElement->dumpContent(result, parameterAccessContext);
    return CommandHandler::ESucceeded;
}

Parser::CommandReturn Parser::getElementSize(const IRemoteCommand& remoteCommand,
                                             std::string& result)
{
    CElementLocator elementLocator(&mParameterMgr._systemClass);
    CElement* pLocatedElement = NULL;

    if (!elementLocator.locate(remoteCommand.getArgument(0), &pLocatedElement, result)) {
        return CommandHandler::EFailed;
    }

    // Converted to actual sizable element
    const CConfigurableElement* pConfigurableElement =
        static_cast<const CConfigurableElement*>(pLocatedElement);
    // Get size as string
    result = pConfigurableElement->getFootprintAsString();
    return CommandHandler::ESucceeded;
}

Parser::CommandReturn Parser::showProperties(const IRemoteCommand& remoteCommand,
                                             std::string& result)
{
    CElementLocator elementLocator(&mParameterMgr._systemClass);
    CElement* pLocatedElement = NULL;

    if (!elementLocator.locate(remoteCommand.getArgument(0), &pLocatedElement, result)) {
        return CommandHandler::EFailed;
    }

    // Convert element
    const CConfigurableElement* pConfigurableElement =
        static_cast<const CConfigurableElement*>(pLocatedElement);
    // Return element properties
    pConfigurableElement->showProperties(result);
    return CommandHandler::ESucceeded;
}

Parser::CommandReturn Parser::getParameter(const IRemoteCommand& remoteCommand,
                                           std::string& result)
{
    if (!mParameterMgr.accessParameterValue(remoteCommand.getArgument(0), result, false, result)) {
        return CommandHandler::EFailed;
    }
    return CommandHandler::ESucceeded;
}

Parser::CommandReturn Parser::setParameter(const IRemoteCommand& remoteCommand,
                                           std::string& result)
{
    // Get value to set
    std::string strValue = remoteCommand.packArguments(1, remoteCommand.getArgumentCount() - 1);

    return mParameterMgr.accessParameterValue(remoteCommand.getArgument(0),
                                              strValue,
                                              true,
                                              result) ?
           CommandHandler::EDone : CommandHandler::EFailed;
}

Parser::CommandReturn
Parser::listBelongingDomains(const IRemoteCommand& remoteCommand, std::string& result)
{
    CElementLocator elementLocator(&mParameterMgr._systemClass);
    CElement* pLocatedElement = NULL;

    if (!elementLocator.locate(remoteCommand.getArgument(0), &pLocatedElement, result)) {
        return CommandHandler::EFailed;
    }

    // Convert element
    const CConfigurableElement* pConfigurableElement =
        static_cast<const CConfigurableElement*>(pLocatedElement);
    // Return element belonging domains
    pConfigurableElement->listBelongingDomains(result);
    return CommandHandler::ESucceeded;
}

Parser::CommandReturn
Parser::listAssociatedDomains(const IRemoteCommand& remoteCommand, std::string& result)
{
    CElementLocator elementLocator(&mParameterMgr._systemClass);
    CElement* pLocatedElement = NULL;

    if (!elementLocator.locate(remoteCommand.getArgument(0), &pLocatedElement, result)) {
        return CommandHandler::EFailed;
    }
    // Convert element
    const CConfigurableElement* pConfigurableElement =
        static_cast<const CConfigurableElement*>(pLocatedElement);
    // Return element belonging domains
    pConfigurableElement->listAssociatedDomains(result);
    return CommandHandler::ESucceeded;
}

Parser::CommandReturn Parser::listAssociatedElements(const IRemoteCommand&, std::string& result)
{
    mParameterMgr._domains.listAssociatedElements(result);
    return CommandHandler::ESucceeded;
}

Parser::CommandReturn Parser::listConflictingElements(const IRemoteCommand&, std::string& result)
{
    mParameterMgr._domains.listConflictingElements(result);
    return CommandHandler::ESucceeded;
}

Parser::CommandReturn Parser::listRogueElements(const IRemoteCommand&, std::string& result)
{
    mParameterMgr._systemClass.listRogueElements(result);
    return CommandHandler::ESucceeded;
}

Parser::CommandReturn Parser::getConfigurationParameter(const IRemoteCommand& remoteCommand,
                                                        std::string& result)
{
    std::string output;
    if (!mParameterMgr.accessConfigurationValue(
            remoteCommand.getArgument(0), remoteCommand.getArgument(1),
            remoteCommand.getArgument(2), output, false, result)) {
        return CommandHandler::EFailed;
    }
    // Succeeded
    result = output;
    return CommandHandler::ESucceeded;
}

Parser::CommandReturn
Parser::setConfigurationParameter(const IRemoteCommand& remoteCommand, std::string& result)
{
    // Get value to set
    std::string strValue = remoteCommand.packArguments(3, remoteCommand.getArgumentCount() - 3);

    bool bSuccess = mParameterMgr.accessConfigurationValue(remoteCommand.getArgument(0),
                                                           remoteCommand.getArgument(1),
                                                           remoteCommand.getArgument(2),
                                                           strValue, true, result);

    return bSuccess ? CommandHandler::EDone : CommandHandler::EFailed;
}

Parser::CommandReturn Parser::showMapping(const IRemoteCommand& remoteCommand, std::string& result)
{
    if (!mParameterMgr.getParameterMapping(remoteCommand.getArgument(0), result)) {
        return CommandHandler::EFailed;
    }
    return CommandHandler::ESucceeded;
}

Parser::CommandReturn
Parser::exportConfigurableDomainsToXML(const IRemoteCommand& remoteCommand, std::string& result)
{
    std::string strFileName = remoteCommand.getArgument(0);
    return mParameterMgr.exportDomainsXml(strFileName, false, true, result) ?
           CommandHandler::EDone : CommandHandler::EFailed;
}

Parser::CommandReturn
Parser::importConfigurableDomainsFromXML(const IRemoteCommand& remoteCommand, std::string& result)
{
    return mParameterMgr.importDomainsXml(remoteCommand.getArgument(0), false, true, result) ?
           CommandHandler::EDone : CommandHandler::EFailed;
}

Parser::CommandReturn
Parser::exportConfigurableDomainsWithSettingsToXML(const IRemoteCommand& remoteCommand,
                                                   std::string& result)
{
    std::string strFileName = remoteCommand.getArgument(0);
    return mParameterMgr.exportDomainsXml(strFileName, true, true, result) ?
           CommandHandler::EDone : CommandHandler::EFailed;
}

Parser::CommandReturn
Parser::exportConfigurableDomainWithSettingsToXML(const IRemoteCommand& remoteCommand,
                                                  std::string& result)
{
    std::string domainName = remoteCommand.getArgument(0);
    std::string fileName = remoteCommand.getArgument(1);
    return mParameterMgr.exportSingleDomainXml(fileName, domainName, true, true, result) ?
           CommandHandler::EDone : CommandHandler::EFailed;
}

Parser::CommandReturn
Parser::importConfigurableDomainsWithSettingsFromXML(const IRemoteCommand& remoteCommand,
                                                     std::string& result)
{
    return mParameterMgr.importDomainsXml(remoteCommand.getArgument(0), true, true, result) ?
           CommandHandler::EDone : CommandHandler::EFailed;
}

Parser::CommandReturn
Parser::importConfigurableDomainWithSettingsFromXML(const IRemoteCommand& remoteCommand,
                                                    std::string& result)
{
    std::string domainName = remoteCommand.getArgument(0);
    bool overwrite = false;

    // Look for optional arguments
    if (remoteCommand.getArgumentCount() > 1) {

        if (remoteCommand.getArgument(1) == "overwrite") {

            overwrite = true;
        } else {
            // Show usage
            return CommandHandler::EShowUsage;
        }
    }

    return mParameterMgr.importSingleDomainXml(domainName, overwrite, true, true, result) ?
           CommandHandler::EDone : CommandHandler::EFailed;
}

Parser::CommandReturn Parser::exportSettings(const IRemoteCommand& remoteCommand,
                                             std::string& result)
{
    return mParameterMgr.exportDomainsBinary(remoteCommand.getArgument(0), result) ?
           CommandHandler::EDone : CommandHandler::EFailed;
}

Parser::CommandReturn Parser::importSettings(const IRemoteCommand& remoteCommand,
                                             std::string& result)
{
    return mParameterMgr.importDomainsBinary(remoteCommand.getArgument(0), result) ?
           CommandHandler::EDone : CommandHandler::EFailed;
}

Parser::CommandReturn Parser::getConfigurableDomainsWithSettingsXML(const IRemoteCommand&,
                                                                    std::string& result)
{
    if (!mParameterMgr.exportDomainsXml(result, true, false, result)) {

        return CommandHandler::EFailed;
    }
    // Succeeded
    return CommandHandler::ESucceeded;
}

Parser::CommandReturn
Parser::getConfigurableDomainWithSettingsXML(const IRemoteCommand& remoteCommand,
                                             std::string& result)
{
    std::string strDomainName = remoteCommand.getArgument(0);

    return mParameterMgr.exportSingleDomainXml(result, strDomainName, true, false, result) ?
           CommandHandler::ESucceeded : CommandHandler::EFailed;
}

Parser::CommandReturn
Parser::setConfigurableDomainsWithSettingsXML(const IRemoteCommand& remoteCommand,
                                              std::string& result)
{
    return mParameterMgr.importDomainsXml(remoteCommand.getArgument(0), true, false, result) ?
           CommandHandler::EDone : CommandHandler::EFailed;
}

Parser::CommandReturn
Parser::setConfigurableDomainWithSettingsXML(const IRemoteCommand& remoteCommand,
                                             std::string& result)
{
    std::string domainName = remoteCommand.getArgument(0);
    bool overwrite = false;

    // Look for optional arguments
    if (remoteCommand.getArgumentCount() > 1) {

        if (remoteCommand.getArgument(1) == "overwrite") {

            overwrite = true;
        } else {
            // Show usage
            return CommandHandler::EShowUsage;
        }
    }

    return mParameterMgr.importSingleDomainXml(domainName, overwrite, true, false, result) ?
           CommandHandler::EDone : CommandHandler::EFailed;
}

Parser::CommandReturn Parser::getSystemClassXML(const IRemoteCommand&, std::string& result)
{
    // Get Root element where to export from
    const CSystemClass &systemClass = mParameterMgr._systemClass;

    if (!mParameterMgr.exportElementToXMLString(
            &systemClass, systemClass.getKind(), result)) {

        return CommandHandler::EFailed;
    }
    // Succeeded
    return CommandHandler::ESucceeded;
}

} /** command namespace */
} /** core namespace */
