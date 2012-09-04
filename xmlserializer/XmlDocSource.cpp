/*
 * INTEL CONFIDENTIAL
 * Copyright © 2013 Intel
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
 */

#include "XmlDocSource.h"
#include <libxml/tree.h>
#include <libxml/xmlschemas.h>
#include <stdlib.h>

// Schedule for libxml2 library
bool CXmlDocSource::_bLibXml2CleanupScheduled;

CXmlDocSource::CXmlDocSource(_xmlDoc *pDoc, _xmlNode *pRootNode):
      _pDoc(pDoc),
      _pRootNode(pRootNode),
      _strXmlSchemaFile(""),
      _strRootElementType(""),
      _strRootElementName(""),
      _strNameAttrituteName(""),
      _bNameCheck(false)
{
    init();
}

CXmlDocSource::CXmlDocSource(_xmlDoc *pDoc,
                             const string& strXmlSchemaFile,
                             const string& strRootElementType,
                             const string& strRootElementName,
                             const string& strNameAttrituteName) :
    _pDoc(pDoc),
    _pRootNode(NULL),
    _strXmlSchemaFile(strXmlSchemaFile),
    _strRootElementType(strRootElementType),
    _strRootElementName(strRootElementName),
    _strNameAttrituteName(strNameAttrituteName),
    _bNameCheck(true)
{
    init();
}

CXmlDocSource::CXmlDocSource(_xmlDoc* pDoc,
                             const string& strXmlSchemaFile,
                             const string& strRootElementType) :
    _pDoc(pDoc), _pRootNode(NULL),
    _strXmlSchemaFile(strXmlSchemaFile),
    _strRootElementType(strRootElementType),
    _strRootElementName(""),
    _strNameAttrituteName(""),
    _bNameCheck(false)
{
    init();
}

CXmlDocSource::~CXmlDocSource()
{
    if (_pDoc) {
        // Free XML doc
        xmlFreeDoc(_pDoc);
        _pDoc = NULL;
    }
}

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

bool CXmlDocSource::validate(CXmlSerializingContext& serializingContext)
{
    // Check that the doc has been created
    if (!_pDoc) {

        serializingContext.setError("Could not parse document ");

        return false;
    }

    // Validate
    if (!isInstanceDocumentValid()) {

        serializingContext.setError("Document is not valid");

        return false;
    }

    // Check Root element type
    if (getRootElementName() != _strRootElementType) {

        serializingContext.setError("Error: Wrong XML structure document ");
        serializingContext.appendLineToError("Root Element " + getRootElementName()
                                             + " mismatches expected type " + _strRootElementType);

        return false;
    }

    if (_bNameCheck) {

        string strRootElementNameCheck = getRootElementAttributeString(_strNameAttrituteName);

        // Check Root element name attribute (if any)
        if (!_strRootElementName.empty() && strRootElementNameCheck != _strRootElementName) {

            serializingContext.setError("Error: Wrong XML structure document ");
            serializingContext.appendLineToError(_strRootElementType + " element "
                                                 + _strRootElementName + " mismatches expected "
                                                 + _strRootElementType + " type "
                                                 + strRootElementNameCheck);

            return false;
        }
    }

    return true;
}

void CXmlDocSource::init()
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

bool CXmlDocSource::isInstanceDocumentValid()
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

void CXmlDocSource::schemaValidityStructuredErrorFunc(void* pUserData, _xmlError* pError)
{
    (void)pUserData;

#ifdef LIBXML_SCHEMAS_ENABLED
    // Display message
    puts(pError->message);
#endif
}
