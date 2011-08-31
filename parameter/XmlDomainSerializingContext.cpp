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
#include "XmlDomainSerializingContext.h"

#define base CXmlElementSerializingContext

CXmlDomainSerializingContext::CXmlDomainSerializingContext(string& strError, bool bWithSettings)
    : base(strError), _bWithSettings(bWithSettings), _bValueSpaceIsRaw(false), _bOutputRawFormatIsHex(false), _pSelectionCriteriaDefinition(NULL)
{
}

// Settings to be loaded or not
bool CXmlDomainSerializingContext::withSettings() const
{
    return _bWithSettings;
}

// Value Space
void CXmlDomainSerializingContext::setValueSpaceRaw(bool bIsRaw)
{
    _bValueSpaceIsRaw = bIsRaw;
}

bool CXmlDomainSerializingContext::valueSpaceIsRaw() const
{
    return _bValueSpaceIsRaw;
}

// Output Raw Format for user get value interpretation
void CXmlDomainSerializingContext::setOutputRawFormat(bool bIsHex)
{
    _bOutputRawFormatIsHex = bIsHex;
}

bool CXmlDomainSerializingContext::outputRawFormatIsHex()
{
    return _bOutputRawFormatIsHex;
}

// Criteria defintion
void CXmlDomainSerializingContext::setSelectionCriteriaDefinition(const CSelectionCriteriaDefinition* pSelectionCriteriaDefinition)
{
    _pSelectionCriteriaDefinition = pSelectionCriteriaDefinition;
}

const CSelectionCriteriaDefinition* CXmlDomainSerializingContext::getSelectionCriteriaDefinition() const
{
    return _pSelectionCriteriaDefinition;
}
