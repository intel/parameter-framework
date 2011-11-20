/* <auto_header>
 * <FILENAME>
 * 
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
 *  AUTHOR: Patrick Benavoli (patrickx.benavoli@intel.com)
 * CREATED: 2011-06-01
 * UPDATED: 2011-07-27
 * 
 * 
 * </auto_header>
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

#define base CConfigurableElement

// A plugin file name is of the form:
// lib<type>-subsystem.so
// The plugin symbol is of the form:
// get<TYPE>SusbystemBuilder

// Plugin file naming
const char* gpcPluginPattern = "-subsystem.so";
const char* gpcLibraryPrefix = "lib";

// Plugin symbol naming
const char* gpcPluginSymbolPrefix = "get";
const char* gpcPluginSymbolSuffix = "SusbystemBuilder";

// Used by subsystem plugins
typedef void (*GetSusbystemBuilder)(CSubsystemLibrary*);

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

bool CSystemClass::loadSubsystems(string& strError, const vector<string>& astrPluginFolderPaths)
{
    CAutoLog autoLlog(this, "Loading subsystem plugins");

    // Plugin list
    list<string> lstrPluginFiles;

    uint32_t uiFolderLocation;

    for (uiFolderLocation = 0; uiFolderLocation < astrPluginFolderPaths.size(); uiFolderLocation++) {

        // Folder for current SystemClass
        string strPluginPath = astrPluginFolderPaths[uiFolderLocation] + "/";

        // Get plugin list
        getPluginFiles(strPluginPath, lstrPluginFiles);
    }
    // Check at least one subsystem plugin available
    if (!lstrPluginFiles.size()) {

        // No plugin found?
        log("No subsystem plugin found");

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
    log("All subsystem plugins successfully loaded");

    // Add virtual subsystem builder
    _pSubsystemLibrary->addElementBuilder(new TNamedElementBuilderTemplate<CVirtualSubsystem>("Virtual"));

    return true;
}

// Subsystem plugins
bool CSystemClass::getPluginFiles(const string& strPluginPath, list<string>& lstrPluginFiles) const
{
    log("Seeking subsystem plugins from folder \"%s\"", strPluginPath.c_str());

    DIR *dirp;
    struct dirent *dp;

    if ((dirp = opendir(strPluginPath.c_str())) == NULL) {

        return false;
    }

    const string strPluginPattern(gpcPluginPattern);

    // Parse it and load plugins
    while ((dp = readdir(dirp)) != NULL) {

        string strFileName(dp->d_name);

        // Check file name ends with pattern
        size_t uiPatternPos = strFileName.rfind(strPluginPattern, -1);

        if (uiPatternPos != (size_t)-1 && uiPatternPos == strFileName.size() - strPluginPattern.size()) {
            // Found plugin
            lstrPluginFiles.push_back(strPluginPath + strFileName);
        }
    }

    // Close plugin folder
    closedir(dirp);

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

        log("Attempting to load subsystem plugin path \"%s\"", strPluginFileName.c_str());

        // Load attempt
        void* lib_handle = dlopen(strPluginFileName.c_str(), RTLD_LAZY);

        if (!lib_handle) {

            // Failed
            log("Plugin load failed, proceeding on with remaining ones");

            // Next plugin
            ++it;

            continue;
        }

        // Get plugin symbol
        string strPluginSymbol = getPluginSymbol(strPluginFileName);

        // Load symbol from library
        GetSusbystemBuilder pfnGetSusbystemBuilder = (GetSusbystemBuilder)dlsym(lib_handle, strPluginSymbol.c_str());

        if (!pfnGetSusbystemBuilder) {

            strError = "Subsystem plugin " + strPluginFileName + " does not contain " + strPluginSymbol + " symbol.";

            return false;
        }

        // Fill library
        pfnGetSusbystemBuilder(_pSubsystemLibrary);

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

bool CSystemClass::init(string& strError)
{
    return base::init(strError);
}

