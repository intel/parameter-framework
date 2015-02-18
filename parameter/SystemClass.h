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

#include "ConfigurableElement.h"
#include "SubsystemPlugins.h"
#include <list>
#include <string>

class CSubsystemLibrary;

class CSystemClass : public CConfigurableElement
{
public:
    CSystemClass();
    virtual ~CSystemClass();

    /** Load subsystem plugin and fill the corresponding libraries.
     *
     * @param[out] strError is filled with new line separated errors if the function returns false,
     *                         undefined otherwise.
     * @param[in] pSubsystemPlugins The plugins to load.
     * @param[in] bVirtualSubsystemFallback If a subsystem can not be found, use the virtual one.
     *
     * @return true if the plugins succesfully started or that a fallback is available,
               false otherwise.
     */
    bool loadSubsystems(std::string& strError, const CSubsystemPlugins* pSubsystemPlugins,
                        bool bVirtualSubsystemFallback = false);
    // Subsystem factory
    const CSubsystemLibrary* getSubsystemLibrary() const;

    /**
      * Look for subsystems that need to be resynchronized.
      * Consume the need to be resynchronized
      * and fill a syncer set with all syncers that need to be resynchronized
      *
      * @param[out] syncerSet The syncer set to fill
      */
    void checkForSubsystemsToResync(CSyncerSet& syncerSet);

    /**
      * Reset subsystems need to resync flag.
      */
    void cleanSubsystemsNeedToResync();

    // base
    virtual std::string getKind() const;

private:
    CSystemClass(const CSystemClass&);
    CSystemClass& operator=(const CSystemClass&);
    // base
    virtual bool childrenAreDynamic() const;

    /** Load shared libraries subsystem plugins.
     *
     * @param[out] lstrError is the list of error that occured during loadings.
     * @param[in] pSubsystemPlugins The plugins to load.
     *
     * @return true if all plugins have been succesfully loaded, false otherwises.
     */
    bool loadSubsystemsFromSharedLibraries(std::list<std::string>& lstrError,
                                           const CSubsystemPlugins* pSubsystemPlugins);

    // Plugin symbol computation
    static std::string getPluginSymbol(const std::string& strPluginPath);

    /** Load subsystem plugin shared libraries.
     *
     * @param[in:out] lstrPluginFiles is the path list of the plugins shared libraries to load.
     *                Successfully loaded plugins are removed from the list.
     * @param[out] lstrError is the list of error that occured during loadings.
     *
     * @return true if at least one plugin has been succesfully loaded, false otherwise.
     *         When false is returned, some plugins MIHGT have been loaded
     *         but the lstrPluginFiles is accurate.
     */
    bool loadPlugins(std::list<std::string>& lstrPluginFiles, std::list<std::string>& lstrError);

    // Subsystem factory
    CSubsystemLibrary* _pSubsystemLibrary;
    std::list<void*> _subsystemLibraryHandleList; /**< Contains the list of all open plugin libs. */
};

