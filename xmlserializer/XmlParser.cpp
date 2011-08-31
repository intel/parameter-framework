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
#include "XmlParser.h"
#include <stdio.h>
#include <stdarg.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlschemas.h>

#define base CXmlSerializer


CXmlParser::CXmlParser(const string& strXmlInstanceFile, const string& strXmlSchemaFile, const string& strRootElementType, CXmlSerializingContext& serializingContext) :
    base(strXmlInstanceFile, strXmlSchemaFile, strRootElementType, serializingContext)
{
}

CXmlParser::~CXmlParser()
{
}

void CXmlParser::schemaValidityStructuredErrorFunc(void* pUserData, _xmlError* pError)
{
    (void)pUserData;

#ifdef LIBXML_SCHEMAS_ENABLED
    // Display message
    puts(pError->message);
#endif
}

bool CXmlParser::isInstanceDocumentValid()
{
#ifdef LIBXML_SCHEMAS_ENABLED
    xmlDocPtr pSchemaDoc = xmlReadFile(_strXmlSchemaFile.c_str(), NULL, XML_PARSE_NONET);

    if (!pSchemaDoc) {
        // Unable to load Schema
        return false;
    }

    xmlSchemaParserCtxtPtr pParserCtxt = xmlSchemaNewDocParserCtxt(pSchemaDoc);

    if (!pParserCtxt) {

        // Unable to create schema context
        xmlFreeDoc(pSchemaDoc);
        return false;
    }

    // Get Schema
    xmlSchemaPtr pSchema = xmlSchemaParse(pParserCtxt);

    if (!pSchema) {

        // Invalid Schema
        xmlSchemaFreeParserCtxt(pParserCtxt);
        xmlFreeDoc(pSchemaDoc);
        return false;
    }
    xmlSchemaValidCtxtPtr pValidationCtxt = xmlSchemaNewValidCtxt(pSchema);

    if (!pValidationCtxt) {

        // Unable to create validation context
        xmlSchemaFree(pSchema);
        xmlSchemaFreeParserCtxt(pParserCtxt);
        xmlFreeDoc(pSchemaDoc);
        return false;
    }

    xmlSetStructuredErrorFunc(this, schemaValidityStructuredErrorFunc);
    //xmlSchemaSetValidErrors(pValidationCtxt, schemaValidityErrorFunc, schemaValidityWarningFunc, NULL);

    bool isDocValid = xmlSchemaValidateDoc(pValidationCtxt, _pDoc) == 0;

    xmlSchemaFreeValidCtxt(pValidationCtxt);
    xmlSchemaFree(pSchema);
    xmlSchemaFreeParserCtxt(pParserCtxt);
    xmlFreeDoc(pSchemaDoc);

    return isDocValid;
#else
    return true;
#endif
}

bool CXmlParser::open()
{
    // Parse the file and get the DOM
    _pDoc = xmlReadFile(_strXmlInstanceFile.c_str(), NULL, 0);

    if (!_pDoc) {

        _serializingContext.setError("Could not parse file " + _strXmlInstanceFile);

        return false;
    }
    // Validate
    if (!isInstanceDocumentValid()) {

        _serializingContext.setError("Document " + _strXmlInstanceFile + " is not valid");

        // Free XML doc
        xmlFreeDoc(_pDoc);

        _pDoc = NULL;

        return false;
    }

    // Get the root element node
    _pRootNode = xmlDocGetRootElement(_pDoc);

    // Check Root element type
    if (getRootElementName() != _strRootElementType) {

        _serializingContext.setError("Error: Wrong XML structure file " + _strXmlInstanceFile);
        _serializingContext.appendLineToError("Root Element " + getRootElementName() + " mismatches expected type " + _strRootElementType);

        return false;
    }

    return base::open();
}

bool CXmlParser::parse(IXmlSink* pXmlSink)
{
    // Parse document
    CXmlElement topMostElement(_pRootNode);

    return pXmlSink->fromXml(topMostElement, _serializingContext);
}

