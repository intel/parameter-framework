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

#include "XmlFileDocSource.h"
#include <libxml/parser.h>
#include <libxml/xmlschemas.h>

#define base CXmlDocSource



CXmlFileDocSource::CXmlFileDocSource(const string& strXmlInstanceFile, const string& strXmlSchemaFile, const string& strRootElementType, const string& strRootElementName, const string& strNameAttrituteName) :
        base(xmlReadFile(strXmlInstanceFile.c_str(), NULL, 0)), _strXmlInstanceFile(strXmlInstanceFile), _strXmlSchemaFile(strXmlSchemaFile), _strRootElementType(strRootElementType), _strRootElementName(strRootElementName), _strNameAttrituteName(strNameAttrituteName), _bNameCheck(true)
{
}

CXmlFileDocSource::CXmlFileDocSource(const string& strXmlInstanceFile, const string& strXmlSchemaFile, const string& strRootElementType) :
        base(xmlReadFile(strXmlInstanceFile.c_str(), NULL, 0)), _strXmlInstanceFile(strXmlInstanceFile), _strXmlSchemaFile(strXmlSchemaFile), _strRootElementType(strRootElementType), _strRootElementName(""), _strNameAttrituteName(""), _bNameCheck(false)
{
}

bool CXmlFileDocSource::populate(CXmlSerializingContext& serializingContext)
{
    // Check that the doc has been created
    if (!isParsable(serializingContext)) {

        return false;
    }

    // Validate
    if (!isInstanceDocumentValid()) {

        serializingContext.setError("Document " + _strXmlInstanceFile + " is not valid");

        return false;
    }

    // Check Root element type
    if (getRootElementName() != _strRootElementType) {

        serializingContext.setError("Error: Wrong XML structure file " + _strXmlInstanceFile);
        serializingContext.appendLineToError("Root Element " + getRootElementName() + " mismatches expected type " + _strRootElementType);

        return false;
    }

    // Check Root element name attribute (if any)
    if (_bNameCheck) {

        string strRootElementNameCheck = getRootElementAttributeString(_strNameAttrituteName);

        if (!_strRootElementName.empty() && strRootElementNameCheck != _strRootElementName) {

            serializingContext.setError("Error: Wrong XML structure file " + _strXmlInstanceFile);
            serializingContext.appendLineToError(_strRootElementType + " element " + _strRootElementName + " mismatches expected " + _strRootElementType + " type " + strRootElementNameCheck);

            return false;
        }
    }

    return true;
}

bool CXmlFileDocSource::isParsable(CXmlSerializingContext& serializingContext) const
{
    // Check that the doc has been created
    if (!_pDoc) {

        serializingContext.setError("Could not parse file " + _strXmlInstanceFile);

        return false;
    }

    return true;
}

bool CXmlFileDocSource::isInstanceDocumentValid()
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

void CXmlFileDocSource::schemaValidityStructuredErrorFunc(void* pUserData, _xmlError* pError)
{
    (void)pUserData;

#ifdef LIBXML_SCHEMAS_ENABLED
    // Display message
    puts(pError->message);
#endif
}
