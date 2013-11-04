/*
 * INTEL CONFIDENTIAL
 * Copyright © 2011 Intel
 * Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its suppliers
 * or licensors. Title to the Material remains with Intel Corporation or its
 * suppliers and licensors. The Material contains trade secrets and proprietary
 * and confidential information of Intel or its suppliers and licensors. The
 * Material is protected by worldwide copyright and trade secret laws and
 * treaty provisions. No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel’s prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Intel in writing.
 *
 * CREATED: 2011-06-01
 * UPDATED: 2011-07-27
 */
#include <dlfcn.h>
#include <dirent.h>
#include <algorithm>
#include <ctype.h>
#include "SystemClass.h"
#include "SubsystemLibrary.h"
#include "AutoLog.h"
#include "VirtualSubsystem.h"
#include "NamedElementBuilderTemplate.h"
#include <assert.h>
#include "PluginLocation.h"
#include "Utility.h"

#define base CConfigurableElement

/**
 * A plugin file name is of the form:
 * lib<type>-subsystem.so or lib<type>-subsystem._host.so
 *
 * The plugin symbol is of the form:
 * get<TYPE>SubsystemBuilder
*/
// Plugin file naming
const char* gpcPluginSuffix = "-subsystem";
const char* gpcPluginPrefix = "lib";

// Plugin symbol naming
const char* gpcPluginSymbolPrefix = "get";
const char* gpcPluginSymbolSuffix = "SubsystemBuilder";

// Used by subsystem plugins
typedef void (*GetSubsystemBuilder)(CSubsystemLibrary*);

CSystemClass::CSystemClass() : _pSubsystemLibrary(new CSubsystemLibrary)
{
}

CSystemClass::~CSystemClass()
{
    delete _pSubsystemLibrary;
}

bool CSystemClass::childrenAreDynamic() const
{
    return true;
}

string CSystemClass::getKind() const
{
    return "SystemClass";
}

bool CSystemClass::loadSubsystems(string& strError,
                                  const CSubsystemPlugins* pSubsystemPlugins,
                                  bool bVirtualSubsystemFallback)
{
    CAutoLog autoLog_info(this, "Loading subsystem plugins");

    // Start clean
    _pSubsystemLibrary->clean();

    // Add virtual subsystem builder
    _pSubsystemLibrary->addElementBuilder("Virtual",
                                          new TNamedElementBuilderTemplate<CVirtualSubsystem>());
    // Set virtual subsytem as builder fallback if required
    _pSubsystemLibrary->enableDefaultMechanism(bVirtualSubsystemFallback);

    // Add subsystem defined in shared libraries
    list<string> lstrError;
    bool bLoadPluginsSuccess = loadSubsystemsFromSharedLibraries(lstrError, pSubsystemPlugins);

    if (bLoadPluginsSuccess) {
        log_info("All subsystem plugins successfully loaded");
    } else {
        // Log plugin as warning if no fallback available
        log_table(!bVirtualSubsystemFallback, lstrError);
    }

    if (!bVirtualSubsystemFallback) {
        // Any problem reported is an error as there is no fallback.
        // Fill strError for caller.
        CUtility::asString(lstrError, strError);
    }

    return bLoadPluginsSuccess || bVirtualSubsystemFallback;
}

bool CSystemClass::loadSubsystemsFromSharedLibraries(list<string>& lstrError,
                                                     const CSubsystemPlugins* pSubsystemPlugins)
{
    // Plugin list
    list<string> lstrPluginFiles;

    uint32_t uiPluginLocation;

    for (uiPluginLocation = 0; uiPluginLocation <  pSubsystemPlugins->getNbChildren(); uiPluginLocation++) {

        // Get Folder for current Plugin Location
        const CPluginLocation* pPluginLocation = static_cast<const CPluginLocation*>(pSubsystemPlugins->getChild(uiPluginLocation));

        const string& strFolder = pPluginLocation->getFolder();

        // Iterator on Plugin List:
        list<string>::const_iterator it;

        const list<string>& pluginList = pPluginLocation->getPluginList();

        for (it = pluginList.begin(); it != pluginList.end(); ++it) {

            // Fill Plugin files list
            lstrPluginFiles.push_back(strFolder + "/" + *it);
        }
    }

    // Actually load plugins
    while (!lstrPluginFiles.empty()) {

        // Because plugins might depend on one another, loading will be done
        // as an iteration process that finishes successfully when the remaining
        // list of plugins to load gets empty or unsuccessfully if the loading
        // process failed to load at least one of them

        // Attempt to load the complete list
        if (!loadPlugins(lstrPluginFiles, lstrError)) {

            // Unable to load at least one plugin
            break;
        }
    }

    if (!lstrPluginFiles.empty()) {
        // Unable to load at least one plugin
        string strPluginUnloaded;
        CUtility::asString(lstrPluginFiles, strPluginUnloaded, ", ");

        lstrError.push_back("Unable to load the following plugins: " + strPluginUnloaded + ".");
        return false;
    }

    return true;
}

// Plugin symbol computation
string CSystemClass::getPluginSymbol(const string& strPluginPath)
{
    // Extract plugin type out of file name
    string strPluginSuffix = gpcPluginSuffix;
    string strPluginPrefix = gpcPluginPrefix;

    // Remove folder and library prefix
    size_t iPluginTypePos = strPluginPath.rfind('/') + 1 + strPluginPrefix.length();

    // Get index of -subsystem.so or -subsystem_host.so suffix
    size_t iSubsystemPos = strPluginPath.find(strPluginSuffix, iPluginTypePos);

    // Get type (between iPluginTypePos and iSubsystemPos)
    string strPluginType = strPluginPath.substr(iPluginTypePos, iSubsystemPos - iPluginTypePos);

    // Make it upper case
    std::transform(strPluginType.begin(), strPluginType.end(), strPluginType.begin(), ::toupper);

    // Get plugin symbol
    return gpcPluginSymbolPrefix + strPluginType + gpcPluginSymbolSuffix;
}

// Plugin loading
bool CSystemClass::loadPlugins(list<string>& lstrPluginFiles, list<string>& lstrError)
{
    assert(lstrPluginFiles.size());

    bool bAtLeastOneSubsystemPluginSuccessfullyLoaded = false;

    list<string>::iterator it = lstrPluginFiles.begin();

    while (it != lstrPluginFiles.end()) {

        string strPluginFileName = *it;

        log_info("Attempting to load subsystem plugin path \"%s\"", strPluginFileName.c_str());

        // Load attempt
        void* lib_handle = dlopen(strPluginFileName.c_str(), RTLD_LAZY);

        if (!lib_handle) {

            // Failed
            lstrError.push_back("Plugin load failed: " + string(dlerror()));

            // Next plugin
            ++it;

            continue;
        }

        // Get plugin symbol
        string strPluginSymbol = getPluginSymbol(strPluginFileName);

        // Load symbol from library
        GetSubsystemBuilder pfnGetSubsystemBuilder = (GetSubsystemBuilder)dlsym(lib_handle, strPluginSymbol.c_str());

        if (!pfnGetSubsystemBuilder) {

            lstrError.push_back("Subsystem plugin " + strPluginFileName +
                                " does not contain " + strPluginSymbol + " symbol.");

            continue;
        }

        // Account for this success
        bAtLeastOneSubsystemPluginSuccessfullyLoaded = true;

        // Fill library
        pfnGetSubsystemBuilder(_pSubsystemLibrary);

        // Remove successfully loaded plugin from list and select next
        lstrPluginFiles.erase(it++);
    }

    return bAtLeastOneSubsystemPluginSuccessfullyLoaded;
}

const CSubsystemLibrary* CSystemClass::getSubsystemLibrary() const
{
    return _pSubsystemLibrary;
}

void CSystemClass::checkForSubsystemsToResync(CSyncerSet& syncerSet)
{
    uint32_t uiNbChildren = getNbChildren();
    uint32_t uiChild;

    for (uiChild = 0; uiChild < uiNbChildren; uiChild++) {

        CSubsystem* pSubsystem = static_cast<CSubsystem*>(getChild(uiChild));

        // Collect and consume the need for a resync
        if (pSubsystem->needResync(true)) {

            log_info("Resynchronizing subsystem: %s", pSubsystem->getName().c_str());
            // get all subsystem syncers
            pSubsystem->fillSyncerSet(syncerSet);
        }
    }
}

void CSystemClass::cleanSubsystemsNeedToResync()
{
    uint32_t uiNbChildren = getNbChildren();
    uint32_t uiChild;

    for (uiChild = 0; uiChild < uiNbChildren; uiChild++) {

        CSubsystem* pSubsystem = static_cast<CSubsystem*>(getChild(uiChild));

        // Consume the need for a resync
        pSubsystem->needResync(true);
    }
}

bool CSystemClass::init(string& strError)
{
    return base::init(strError);
}

// From IXmlSource
void CSystemClass::toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const
{
    // Set the name of system class
    xmlElement.setNameAttribute(getName());

    base::toXml(xmlElement, serializingContext);
}
