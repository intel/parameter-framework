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
 * CREATED: 2012-08-10
 */
#include "XmlDocSource.h"
#include <libxml/tree.h>
#include <stdlib.h>

// Schedule for libxml2 library
bool CXmlDocSource::_bLibXml2CleanupScheduled;

CXmlDocSource::CXmlDocSource(_xmlDoc *pDoc, _xmlNode *pRootNode):
      _pDoc(pDoc), _pRootNode(pRootNode)
{
    if (!_bLibXml2CleanupScheduled) {

        // Schedule cleanup
        atexit(xmlCleanupParser);

        _bLibXml2CleanupScheduled = true;
    }

    if (!_pRootNode) {

        _pRootNode = xmlDocGetRootElement(_pDoc);
    }
}

CXmlDocSource::~CXmlDocSource()
{
    if (_pDoc) {
        // Free XML doc
        xmlFreeDoc(_pDoc);
        _pDoc = NULL;
    }
}

// Root element
void CXmlDocSource::getRootElement(CXmlElement& xmlRootElement) const
{
    xmlRootElement.setXmlElement(_pRootNode);
}

string CXmlDocSource::getRootElementName() const
{
    return (const char*)_pRootNode->name;
}

string CXmlDocSource::getRootElementAttributeString(const string& strAttributeName) const
{
    CXmlElement topMostElement(_pRootNode);

    return topMostElement.getAttributeString(strAttributeName);
}

_xmlDoc* CXmlDocSource::getDoc() const
{
    return _pDoc;
}

