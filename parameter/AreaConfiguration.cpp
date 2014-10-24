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
#include "AreaConfiguration.h"
#include "ConfigurableElement.h"
#include "ConfigurationAccessContext.h"
#include "BinaryStream.h"
#include <assert.h>

CAreaConfiguration::CAreaConfiguration(const CConfigurableElement* pConfigurableElement, const CSyncerSet* pSyncerSet)
    : _pConfigurableElement(pConfigurableElement), _pSyncerSet(pSyncerSet), _bValid(false)
{
    // Size blackboard
    _blackboard.setSize(_pConfigurableElement->getFootPrint());
}

CAreaConfiguration::CAreaConfiguration(const CConfigurableElement* pConfigurableElement, const CSyncerSet* pSyncerSet, uint32_t uiSize)
    : _pConfigurableElement(pConfigurableElement), _pSyncerSet(pSyncerSet), _bValid(false)
{
    // Size blackboard
    _blackboard.setSize(uiSize);
}

// Save data from current
void CAreaConfiguration::save(const CParameterBlackboard* pMainBlackboard)
{
    copyFrom(pMainBlackboard, _pConfigurableElement->getOffset());
}

// Apply data to current
bool CAreaConfiguration::restore(CParameterBlackboard* pMainBlackboard, bool bSync, std::list<std::string>* plstrError) const
{
    assert(_bValid);

    copyTo(pMainBlackboard, _pConfigurableElement->getOffset());

    // Synchronize if required
    return !bSync || _pSyncerSet->sync(*pMainBlackboard, false, plstrError);
}

// Ensure validity
void CAreaConfiguration::validate(const CParameterBlackboard* pMainBlackboard)
{
    if (!_bValid) {

        // Saving from blackboard make area configuration valid
        save(pMainBlackboard);

        _bValid = true;
    }
}

// Return validity
bool CAreaConfiguration::isValid() const
{
    return _bValid;
}

// Ensure validity against given valid area configuration
void CAreaConfiguration::validateAgainst(const CAreaConfiguration* pValidAreaConfiguration)
{
    // Should be called on purpose
    assert(!_bValid);

    // Check proper against area given
    assert(pValidAreaConfiguration->isValid());

    // Check compatibility
    assert(_pConfigurableElement == pValidAreaConfiguration->_pConfigurableElement);

    // Copy
    _blackboard.restoreFrom(&pValidAreaConfiguration->_blackboard, 0);

    // Set as valid
    _bValid = true;
}

// XML configuration settings parsing
bool CAreaConfiguration::serializeXmlSettings(CXmlElement& xmlConfigurableElementSettingsElementContent, CConfigurationAccessContext& configurationAccessContext)
{
    // Assign blackboard to configuration context
    configurationAccessContext.setParameterBlackboard(&_blackboard);

    // Assign base offset to configuration context
    configurationAccessContext.setBaseOffset(_pConfigurableElement->getOffset());

    // Parse configuration settings (element contents)
    if (_pConfigurableElement->serializeXmlSettings(xmlConfigurableElementSettingsElementContent, configurationAccessContext)) {

        if (!configurationAccessContext.serializeOut()) {

            // Serialized-in areas are valid
            _bValid = true;
        }
        return true;
    }
    return false;
}

// Compound handling
const CConfigurableElement* CAreaConfiguration::getConfigurableElement() const
{
    return _pConfigurableElement;
}

void CAreaConfiguration::copyToOuter(CAreaConfiguration* pToAreaConfiguration) const
{
    assert(_pConfigurableElement->isDescendantOf(pToAreaConfiguration->getConfigurableElement()));

    copyTo(&pToAreaConfiguration->_blackboard, _pConfigurableElement->getOffset() - pToAreaConfiguration->getConfigurableElement()->getOffset());
}

void CAreaConfiguration::copyFromOuter(const CAreaConfiguration* pFromAreaConfiguration)
{
    assert(_pConfigurableElement->isDescendantOf(pFromAreaConfiguration->getConfigurableElement()));

    copyFrom(&pFromAreaConfiguration->_blackboard, _pConfigurableElement->getOffset() - pFromAreaConfiguration->getConfigurableElement()->getOffset());

    // Inner becomes valid
    setValid(true);
}

// Serialization
void CAreaConfiguration::serialize(CBinaryStream& binaryStream)
{
    // Delegate to blackboard
    _blackboard.serialize(binaryStream);

    if (!binaryStream.isOut()) {

        // Serialized in areas are valid
        _bValid = true;
    }
}

// Data size
uint32_t CAreaConfiguration::getSize() const
{
    return _blackboard.getSize();
}

CParameterBlackboard& CAreaConfiguration::getBlackboard()
{
    return _blackboard;
}

const CParameterBlackboard& CAreaConfiguration::getBlackboard() const
{
     return _blackboard;
}

// Store validity
void CAreaConfiguration::setValid(bool bValid)
{
    _bValid = bValid;
}

// Blackboard copies
void CAreaConfiguration::copyTo(CParameterBlackboard* pToBlackboard, uint32_t uiOffset) const
{
    pToBlackboard->restoreFrom(&_blackboard, uiOffset);
}

void CAreaConfiguration::copyFrom(const CParameterBlackboard* pFromBlackboard, uint32_t uiOffset)
{
   pFromBlackboard->saveTo(&_blackboard, uiOffset);
}

