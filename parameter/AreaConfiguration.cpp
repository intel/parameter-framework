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
#include "AreaConfiguration.h"
#include "ConfigurableElement.h"
#include "ConfigurationAccessContext.h"
#include "BinaryStream.h"
#include <assert.h>

CAreaConfiguration::CAreaConfiguration(const CConfigurableElement* pConfigurableElement) : _pConfigurableElement(pConfigurableElement), _bValid(false)
{
    _blackboard.setSize(_pConfigurableElement->getFootPrint());
}

// Save data from current
void CAreaConfiguration::save(const CParameterBlackboard* pMainBlackboard)
{
    pMainBlackboard->saveTo(&_blackboard, _pConfigurableElement->getOffset());
}

// Apply data to current
void CAreaConfiguration::restore(CParameterBlackboard* pMainBlackboard) const
{
    assert(_bValid);

    pMainBlackboard->restoreFrom(&_blackboard, _pConfigurableElement->getOffset());
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
bool CAreaConfiguration::serializeXmlSettings(CXmlElement& xmlConfigurationSettingsElementContent, CConfigurationAccessContext& configurationAccessContext)
{
    // Assign blackboard to configuration context
    configurationAccessContext.setParameterBlackboard(&_blackboard);

    // Assign base offset to configuration context
    configurationAccessContext.setBaseOffset(_pConfigurableElement->getOffset());

    // Parse configuration settings (element contents)
    if (_pConfigurableElement->serializeXmlSettings(xmlConfigurationSettingsElementContent, configurationAccessContext)) {

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

// Configuration merging (we assume from element is descendant of this)
void CAreaConfiguration::copyFromInner(const CAreaConfiguration* pFromAreaConfiguration)
{
    assert(pFromAreaConfiguration->getConfigurableElement()->isDescendantOf(_pConfigurableElement));

    _blackboard.restoreFrom(&pFromAreaConfiguration->_blackboard, pFromAreaConfiguration->getConfigurableElement()->getOffset() - _pConfigurableElement->getOffset());
}

// Configuration splitting
void CAreaConfiguration::copyToInner(CAreaConfiguration* pToAreaConfiguration) const
{
    assert(pToAreaConfiguration->getConfigurableElement()->isDescendantOf(_pConfigurableElement));

    _blackboard.saveTo(&pToAreaConfiguration->_blackboard, pToAreaConfiguration->getConfigurableElement()->getOffset() - _pConfigurableElement->getOffset());

    // Inner becomes valid
    pToAreaConfiguration->setValid(true);
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

// Store validity
void CAreaConfiguration::setValid(bool bValid)
{
    _bValid = bValid;
}


