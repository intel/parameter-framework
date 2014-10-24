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

#include "ParameterBlackboard.h"
#include "BinaryStream.h"
#include "SyncerSet.h"

class CConfigurableElement;
class CXmlElement;
class CConfigurationAccessContext;

class CAreaConfiguration
{
public:
    CAreaConfiguration(const CConfigurableElement* pConfigurableElement, const CSyncerSet* pSyncerSet);

    /* FIXME this was missing and probably buggy*/
    virtual ~CAreaConfiguration() {}

    // Save data from current
    void save(const CParameterBlackboard* pMainBlackboard);

    // Apply data to current
    bool restore(CParameterBlackboard* pMainBlackboard, bool bSync, std::list<std::string>* plstrError) const;

    // Ensure validity
    void validate(const CParameterBlackboard* pMainBlackboard);

    // Return validity
    bool isValid() const;

    // Ensure validity against given valid area configuration
    void validateAgainst(const CAreaConfiguration* pValidAreaConfiguration);

    // Compound handling
    const CConfigurableElement* getConfigurableElement() const;

    // Configuration merging
    virtual void copyToOuter(CAreaConfiguration* pToAreaConfiguration) const;

    // Configuration splitting
    virtual void copyFromOuter(const CAreaConfiguration* pFromAreaConfiguration);

    // XML configuration settings parsing/composing
    bool serializeXmlSettings(CXmlElement& xmlConfigurableElementSettingsElementContent, CConfigurationAccessContext& configurationAccessContext);

    // Serialization
    void serialize(CBinaryStream& binaryStream);

    // Data size
    uint32_t getSize() const;

    // Fetch the Configuration Blackboard
    CParameterBlackboard& getBlackboard();
    const CParameterBlackboard& getBlackboard() const;

protected:
    CAreaConfiguration(const CConfigurableElement* pConfigurableElement, const CSyncerSet* pSyncerSet, uint32_t uiSize);

private:
    // Blackboard copies
    virtual void copyTo(CParameterBlackboard* pToBlackboard, uint32_t uiOffset) const;
    virtual void copyFrom(const CParameterBlackboard* pFromBlackboard, uint32_t uiOffset);

    // Store validity
    void setValid(bool bValid);

protected:
    // Associated configurable element
    const CConfigurableElement* _pConfigurableElement;

    // Configurable element settings
    CParameterBlackboard _blackboard;

private:
    // Syncer set (required for immediate synchronization)
    const CSyncerSet* _pSyncerSet;

    // Area configuration validity (invalid area configurations can't be restored)
    bool _bValid;
};

