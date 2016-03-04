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
#include <algorithm>
#include "SystemClass.h"
#include "SubsystemLibrary.h"
#include "VirtualSubsystem.h"
#include "LoggingElementBuilderTemplate.h"
#include <cassert>
#include "PluginLocation.h"
#include "DynamicLibrary.hpp"
#include "Utility.h"
#include "Memory.hpp"

#define base CConfigurableElement

#ifndef PARAMETER_FRAMEWORK_PLUGIN_ENTRYPOINT_V1
#error Missing PARAMETER_FRAMEWORK_PLUGIN_ENTRYPOINT_V1 macro definition
#endif
#define QUOTE(X) #X
#define MACRO_TO_STR(X) QUOTE(X)
const char CSystemClass::entryPointSymbol[] =
    MACRO_TO_STR(PARAMETER_FRAMEWORK_PLUGIN_ENTRYPOINT_V1);
using PluginEntryPointV1 = void (*)(CSubsystemLibrary *, core::log::Logger &);

using std::list;
using std::string;

// FIXME: integrate SystemClass to core namespace
using namespace core;

CSystemClass::CSystemClass(log::Logger &logger)
    : _pSubsystemLibrary(new CSubsystemLibrary()), _logger(logger)
{
}

CSystemClass::~CSystemClass()
{
    delete _pSubsystemLibrary;

    // Destroy child subsystems *before* unloading the libraries (otherwise crashes will occur
    // as unmapped code will be referenced)
    clean();
}

bool CSystemClass::childrenAreDynamic() const
{
    return true;
}

string CSystemClass::getKind() const
{
    return "SystemClass";
}

bool CSystemClass::getMappingData(const std::string & /*strKey*/,
                                  const std::string *& /*pStrValue*/) const
{
    // Although it could make sense to have mapping in the system class,
    // just like at subsystem level, it is currently not supported.
    return false;
}

string CSystemClass::getFormattedMapping() const
{
    return "";
}

bool CSystemClass::loadSubsystems(string &strError, const CSubsystemPlugins *pSubsystemPlugins,
                                  bool bVirtualSubsystemFallback)
{
    // Start clean
    _pSubsystemLibrary->clean();

    typedef TLoggingElementBuilderTemplate<CVirtualSubsystem> VirtualSubsystemBuilder;
    // Add virtual subsystem builder
    _pSubsystemLibrary->addElementBuilder("Virtual", new VirtualSubsystemBuilder(_logger));
    // Set virtual subsytem as builder fallback if required
    if (bVirtualSubsystemFallback) {
        _pSubsystemLibrary->setDefaultBuilder(
            utility::make_unique<VirtualSubsystemBuilder>(_logger));
    }

    // Add subsystem defined in shared libraries
    core::Results errors;
    bool bLoadPluginsSuccess = loadSubsystemsFromSharedLibraries(errors, pSubsystemPlugins);

    // Fill strError for caller, he has to decide if there is a problem depending on
    // bVirtualSubsystemFallback value
    strError = utility::asString(errors);

    return bLoadPluginsSuccess || bVirtualSubsystemFallback;
}

bool CSystemClass::loadSubsystemsFromSharedLibraries(core::Results &errors,
                                                     const CSubsystemPlugins *pSubsystemPlugins)
{
    // Plugin list
    list<string> lstrPluginFiles;

    size_t pluginLocation;

    for (pluginLocation = 0; pluginLocation < pSubsystemPlugins->getNbChildren();
         pluginLocation++) {

        // Get Folder for current Plugin Location
        const CPluginLocation *pPluginLocation =
            static_cast<const CPluginLocation *>(pSubsystemPlugins->getChild(pluginLocation));

        string strFolder(pPluginLocation->getFolder());
        if (!strFolder.empty()) {
            strFolder += "/";
        }
        // Iterator on Plugin List:
        list<string>::const_iterator it;

        const list<string> &pluginList = pPluginLocation->getPluginList();

        for (it = pluginList.begin(); it != pluginList.end(); ++it) {

            // Fill Plugin files list
            lstrPluginFiles.push_back(strFolder + *it);
        }
    }

    // Actually load plugins
    while (!lstrPluginFiles.empty()) {

        // Because plugins might depend on one another, loading will be done
        // as an iteration process that finishes successfully when the remaining
        // list of plugins to load gets empty or unsuccessfully if the loading
        // process failed to load at least one of them

        // Attempt to load the complete list
        if (!loadPlugins(lstrPluginFiles, errors)) {

            // Unable to load at least one plugin
            break;
        }
    }

    if (!lstrPluginFiles.empty()) {
        // Unable to load at least one plugin
        errors.push_back("Unable to load the following plugins: " +
                         utility::asString(lstrPluginFiles, ", ") + ".");
        return false;
    }

    return true;
}

// Plugin loading
bool CSystemClass::loadPlugins(list<string> &lstrPluginFiles, core::Results &errors)
{
    assert(lstrPluginFiles.size());

    bool bAtLeastOneSubsystemPluginSuccessfullyLoaded = false;

    auto it = lstrPluginFiles.begin();

    while (it != lstrPluginFiles.end()) {

        string strPluginFileName = *it;

        // Load attempt
        try {
            auto library = utility::make_unique<DynamicLibrary>(strPluginFileName);

            // Load symbol from library
            auto subSystemBuilder = library->getSymbol<PluginEntryPointV1>(entryPointSymbol);

            // Store libraries handles
            _subsystemLibraryHandleList.push_back(std::move(library));

            // Fill library
            subSystemBuilder(_pSubsystemLibrary, _logger);

        } catch (std::exception &e) {
            errors.push_back(e.what());

            // Next plugin
            ++it;

            continue;
        }

        // Account for this success
        bAtLeastOneSubsystemPluginSuccessfullyLoaded = true;

        // Remove successfully loaded plugin from list and select next
        lstrPluginFiles.erase(it++);
    }

    return bAtLeastOneSubsystemPluginSuccessfullyLoaded;
}

const CSubsystemLibrary *CSystemClass::getSubsystemLibrary() const
{
    return _pSubsystemLibrary;
}

void CSystemClass::checkForSubsystemsToResync(CSyncerSet &syncerSet, core::Results &infos)
{
    size_t uiNbChildren = getNbChildren();
    size_t uiChild;

    for (uiChild = 0; uiChild < uiNbChildren; uiChild++) {

        CSubsystem *pSubsystem = static_cast<CSubsystem *>(getChild(uiChild));

        // Collect and consume the need for a resync
        if (pSubsystem->needResync(true)) {

            infos.push_back("Resynchronizing subsystem: " + pSubsystem->getName());
            // get all subsystem syncers
            pSubsystem->fillSyncerSet(syncerSet);
        }
    }
}

void CSystemClass::cleanSubsystemsNeedToResync()
{
    size_t uiNbChildren = getNbChildren();
    size_t uiChild;

    for (uiChild = 0; uiChild < uiNbChildren; uiChild++) {

        CSubsystem *pSubsystem = static_cast<CSubsystem *>(getChild(uiChild));

        // Consume the need for a resync
        pSubsystem->needResync(true);
    }
}
