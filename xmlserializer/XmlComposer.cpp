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
#include "XmlComposer.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <time.h>

#define base CXmlSerializer

#ifndef LIBXML_TREE_ENABLED
#warning "LIBXML_TREE_ENABLED undefined. XML file exporting feature won't be supported!"
#endif

CXmlComposer::CXmlComposer(const string& strXmlInstanceFile, const string& strXmlSchemaFile, const string& strRootElementType, CXmlSerializingContext& serializingContext) :
    base(strXmlInstanceFile, strXmlSchemaFile, strRootElementType, serializingContext)
{
}

CXmlComposer::~CXmlComposer()
{
}

// open / close
bool CXmlComposer::open()
{
#ifdef LIBXML_TREE_ENABLED
    // Create document from scratch
    _pDoc = xmlNewDoc(BAD_CAST "1.0");

    // Create root node
    _pRootNode = xmlNewNode(NULL, BAD_CAST _strRootElementType.c_str());

    // Assign it to document
    xmlDocSetRootElement(_pDoc, _pRootNode);
#else
    _serializingContext.setError("XML file exporting feature unsupported on this image. This easiest way to activate it is to do a global recompilation with LIBXML_TREE_ENABLED compiler switch set");
#endif
    return base::open();
}

bool CXmlComposer::close()
{
    // Write file (formatted)
    if (xmlSaveFormatFileEnc(_strXmlInstanceFile.c_str(), _pDoc, "UTF-8", 1) == -1) {

        _serializingContext.setError("Could not write file " + _strXmlInstanceFile);

        return false;
    }

    return base::close();
}

// Composing contents
void CXmlComposer::compose(const IXmlSource* pXmlSource, const string& strProduct, const string& strVersion)
{
    // Compose document
    CXmlElement docElement(_pRootNode);

    // Schema namespace
    docElement.setAttributeString("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");

    // Schema location
    docElement.setAttributeString("xsi:noNamespaceSchemaLocation", _strXmlSchemaFile);

    // Comment for date/time
    docElement.setComment(string(" Exported on ") + getTimeAsString() + " from " + strProduct + " version " + strVersion + " ");

    pXmlSource->toXml(docElement, _serializingContext);
}

string CXmlComposer::getTimeAsString()
{
    char acBuf[200];
    time_t t;
    struct tm *tmp;
    t = time(NULL);
    tmp = localtime(&t);

    strftime(acBuf, sizeof(acBuf), "%F, %T", tmp);

    return acBuf;
}
