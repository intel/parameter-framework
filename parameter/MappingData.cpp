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
#include "MappingData.h"
#include "Tokenizer.h"
#include <assert.h>

CMappingData::CMappingData()
{
}

bool CMappingData::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    assert(xmlElement.hasAttribute("Mapping"));

    string strMapping = xmlElement.getAttributeString("Mapping");

    Tokenizer mappingTok(strMapping, ", ");

    string strMappingElement;

    while (!(strMappingElement = mappingTok.next()).empty()) {

        Tokenizer keyValueTok(strMappingElement, ":");

        string strKey = keyValueTok.next();
        string strValue = keyValueTok.next();

        if (!addValue(strKey, strValue)) {

            serializingContext.setError("Duplicate Mapping data: Unable to process Mapping element key = " + strKey + ", value = " + strValue + " from XML element " + xmlElement.getPath());

            return false;
        }
    }
    return true;
}

bool CMappingData::getValue(const string& strkey, string& strValue) const
{
    KeyToValueMapConstIterator it = _keyToValueMap.find(strkey);

    if (it != _keyToValueMap.end()) {

        strValue = it->second;

        return true;
    }
    return false;
}

bool CMappingData::addValue(const string& strkey, const string& strValue)
{
    if (_keyToValueMap.find(strkey) != _keyToValueMap.end()) {

        return false;
    }
    _keyToValueMap[strkey] = strValue;

    return true;
}

