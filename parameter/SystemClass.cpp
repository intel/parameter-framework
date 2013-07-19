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

#define base CConfigurableElement

// A plugin file name is of the form:
// lib<type>-subsystem.so
// The plugin symbol is of the form:
// get<TYPE>SubsystemBuilder

// Plugin file naming
const char* gpcPluginPattern = "-subsystem.so";
const char* gpcLibraryPrefix = "lib";

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

bool CSystemClass::loadSubsystems(string& strError, const CSubsystemPlugins* pSubsystemPlugins)
{
    CAutoLog autoLlog_info(this, "Loading subsystem plugins");

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
    // Check at least one subsystem plugin available
    if (!lstrPluginFiles.size()) {

        // No plugin found?
        log_warning("No subsystem plugin found");

        // Don't bail out now that we have virtual subsystems
    }

    // Start clean
    _pSubsystemLibrary->clean();

    // Actually load plugins
    while (lstrPluginFiles.size()) {

        // Because plugins might depend on one another, loading will be done
        // as an iteration process that finishes successfully when the remaining
        // list of plugins to load gets empty or unsuccessfully if the loading
        // process failed to load at least one of them

        // Attempt to load the complete list
        if (!loadPlugins(lstrPluginFiles, strError)) {

            // Display the list of plugins we were unable to load

            // Leave clean
            _pSubsystemLibrary->clean();

            return false;
        }
    }
    log_info("All subsystem plugins successfully loaded");

    // Add virtual subsystem builder
    _pSubsystemLibrary->addElementBuilder(new TNamedElementBuilderTemplate<CVirtualSubsystem>("Virtual"));

    return true;
}

// Plugin symbol computation
string CSystemClass::getPluginSymbol(const string& strPluginPath)
{
    // Extract plugin type out of file name
    string strPluginPattern = gpcPluginPattern;
    string strLibraryPrefix = gpcLibraryPrefix;

    // Remove folder
    int32_t iSlashPos = strPluginPath.rfind('/') + 1 + strLibraryPrefix.length();

    // Get type
    string strPluginType = strPluginPath.substr(iSlashPos, strPluginPath.length() - iSlashPos - strPluginPattern.length());

    // Make it upper case
    std::transform(strPluginType.begin(), strPluginType.end(), strPluginType.begin(), ::toupper);

    // Get plugin symbol
    return gpcPluginSymbolPrefix + strPluginType + gpcPluginSymbolSuffix;
}

// Plugin loading
bool CSystemClass::loadPlugins(list<string>& lstrPluginFiles, string& strError)
{
    assert(lstrPluginFiles.size());

    bool bAtLeastOneSybsystemPluginSuccessfullyLoaded = false;

    list<string>::iterator it = lstrPluginFiles.begin();

    while (it != lstrPluginFiles.end()) {

        string strPluginFileName = *it;

        log_info("Attempting to load subsystem plugin path \"%s\"", strPluginFileName.c_str());

        // Load attempt
        void* lib_handle = dlopen(strPluginFileName.c_str(), RTLD_LAZY);

        if (!lib_handle) {

            // Failed
            log_warning("Plugin load failed: %s, proceeding on with remaining ones", dlerror());

            // Next plugin
            ++it;

            continue;
        }

        // Get plugin symbol
        string strPluginSymbol = getPluginSymbol(strPluginFileName);

        // Load symbol from library
        GetSubsystemBuilder pfnGetSubsystemBuilder = (GetSubsystemBuilder)dlsym(lib_handle, strPluginSymbol.c_str());

        if (!pfnGetSubsystemBuilder) {

            strError = "Subsystem plugin " + strPluginFileName + " does not contain " + strPluginSymbol + " symbol.";

            return false;
        }

        // Fill library
        pfnGetSubsystemBuilder(_pSubsystemLibrary);

        // Account for this success
        bAtLeastOneSybsystemPluginSuccessfullyLoaded = true;

        // Remove successfully loaded plugin from list and select next
        lstrPluginFiles.erase(it++);
    }

    // Check for success
    if (!bAtLeastOneSybsystemPluginSuccessfullyLoaded) {

        // Return list of plugins we were unable to load
        strError = "Unable to load the following plugins:\n";

        for (it = lstrPluginFiles.begin(); it != lstrPluginFiles.end(); ++it) {

            strError += *it + "\n";
        }

        return false;
    }

    return true;
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
