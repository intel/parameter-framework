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

#include "XmlMemoryDocSource.h"
#include <libxml/parser.h>
#include <libxml/tree.h>

#define base CXmlDocSource

CXmlMemoryDocSource::CXmlMemoryDocSource(const IXmlSource* pXmlSource, const string& strRootElementType, const string& strXmlSchemaFile, const string& strProduct, const string& strVersion):
     base(xmlNewDoc(BAD_CAST "1.0"), xmlNewNode(NULL, BAD_CAST strRootElementType.c_str())), _pXmlSource(pXmlSource), _strXmlSchemaFile(strXmlSchemaFile), _bWithHeader(true), _strProduct(strProduct), _strVersion(strVersion)
{
    init();
}

CXmlMemoryDocSource::CXmlMemoryDocSource(const IXmlSource* pXmlSource, const string& strRootElementType):
    base(xmlNewDoc(BAD_CAST "1.0"), xmlNewNode(NULL, BAD_CAST strRootElementType.c_str())), _pXmlSource(pXmlSource), _bWithHeader(false)
{
    init();
}

void CXmlMemoryDocSource::init()
{
#ifdef LIBXML_TREE_ENABLED

    // Assign it to document
    xmlDocSetRootElement(_pDoc, _pRootNode);
#endif
}

bool CXmlMemoryDocSource::populate(CXmlSerializingContext& serializingContext)
{
#ifndef LIBXML_TREE_ENABLED
    serializingContext.setError("XML file exporting feature unsupported on this image. This easiest way to activate it is to do a global recompilation with LIBXML_TREE_ENABLED compiler switch set");

    return false;
#endif

    // Create Xml element with the Doc
     CXmlElement docElement(_pRootNode);

    if (_bWithHeader) {

        // Schema namespace
        docElement.setAttributeString("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");

        // Schema location
        docElement.setAttributeString("xsi:noNamespaceSchemaLocation", _strXmlSchemaFile);
    }

    // Compose the xml document
    _pXmlSource->toXml(docElement, serializingContext);

    return true;
}
