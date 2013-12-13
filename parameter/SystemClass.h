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
#pragma once

#include "ConfigurableElement.h"
#include "SubsystemPlugins.h"
#include <list>

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
    bool loadSubsystems(string& strError, const CSubsystemPlugins* pSubsystemPlugins,
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
    virtual bool init(string& strError);
    virtual string getKind() const;

    // From IXmlSource
    virtual void toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const;

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
    bool loadSubsystemsFromSharedLibraries(list<string>& lstrError,
                                           const CSubsystemPlugins* pSubsystemPlugins);

    // Plugin symbol computation
    static string getPluginSymbol(const string& strPluginPath);

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
    bool loadPlugins(list<string>& lstrPluginFiles, list<string>& lstrError);

    // Subsystem factory
    CSubsystemLibrary* _pSubsystemLibrary;
    list<void*> _subsystemLibraries; /**< Contains the list of all open plugin libs. */
};

