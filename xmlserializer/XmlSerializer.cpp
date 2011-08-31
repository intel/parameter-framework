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
#include "XmlSerializer.h"
#include <libxml/tree.h>
#include <stdlib.h>

// Schedule for libxml2 library
bool CXmlSerializer::_bLibXml2CleanupScheduled;

CXmlSerializer::CXmlSerializer(const string& strXmlInstanceFile, const string& strXmlSchemaFile, const string& strRootElementType, CXmlSerializingContext& serializingContext) :
    _strXmlInstanceFile(strXmlInstanceFile), _strXmlSchemaFile(strXmlSchemaFile), _strRootElementType(strRootElementType), _serializingContext(serializingContext), _pDoc(NULL), _pRootNode(NULL)
{
    if (_bLibXml2CleanupScheduled) {

        // Schedule cleanup
        atexit(xmlCleanupParser);

        _bLibXml2CleanupScheduled = true;
    }
}

CXmlSerializer::~CXmlSerializer()
{
    // Free XML doc
    xmlFreeDoc(_pDoc);
}

bool CXmlSerializer::close()
{
    // Free XML doc
    xmlFreeDoc(_pDoc);

    _pDoc = NULL;

    return true;
}

bool CXmlSerializer::open()
{
    return _pDoc != NULL;
}

// Root element
void CXmlSerializer::getRootElement(CXmlElement& xmlRootElement) const
{
    xmlRootElement.setXmlElement(_pRootNode);
}

string CXmlSerializer::getRootElementName() const
{
    return (const char*)_pRootNode->name;
}

string CXmlSerializer::getRootElementAttributeString(const string& strAttributeName) const
{
    CXmlElement topMostElement(_pRootNode);

    return topMostElement.getAttributeString(strAttributeName);
}


