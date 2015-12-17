/*
 * Copyright (c) 2011-2015, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "XmlDocSource.h"
#include "AlwaysAssert.hpp"
#include <libxml/tree.h>
#include <libxml/xmlschemas.h>
#include <libxml/parser.h>
#include <libxml/xinclude.h>
#include <libxml/uri.h>
#include <memory>
#include <stdexcept>

using std::string;
using xml_unique_ptr = std::unique_ptr<xmlChar, decltype(xmlFree)>;

CXmlDocSource::CXmlDocSource(_xmlDoc *pDoc, bool bValidateWithSchema, _xmlNode *pRootNode)
    : _pDoc(pDoc), _pRootNode(pRootNode), _strRootElementType(""), _strRootElementName(""),
      _strNameAttributeName(""), _bValidateWithSchema(bValidateWithSchema)
{
}

CXmlDocSource::CXmlDocSource(_xmlDoc *pDoc, bool bValidateWithSchema,
                             const string &strRootElementType, const string &strRootElementName,
                             const string &strNameAttributeName)
    : _pDoc(pDoc), _pRootNode(xmlDocGetRootElement(pDoc)), _strRootElementType(strRootElementType),
      _strRootElementName(strRootElementName), _strNameAttributeName(strNameAttributeName),
      _bValidateWithSchema(bValidateWithSchema)
{
}

CXmlDocSource::~CXmlDocSource()
{
    if (_pDoc) {
        // Free XML doc
        xmlFreeDoc(_pDoc);
        _pDoc = NULL;
    }
}

void CXmlDocSource::getRootElement(CXmlElement &xmlRootElement) const
{
    xmlRootElement.setXmlElement(_pRootNode);
}

string CXmlDocSource::getRootElementName() const
{
    return (const char *)_pRootNode->name;
}

string CXmlDocSource::getRootElementAttributeString(const string &strAttributeName) const
{
    CXmlElement topMostElement(_pRootNode);

    string attribute;
    topMostElement.getAttribute(strAttributeName, attribute);
    return attribute;
}

void CXmlDocSource::setSchemaBaseUri(const string &uri)
{
    _schemaBaseUri = uri;
}

string CXmlDocSource::getSchemaBaseUri()
{
    return _schemaBaseUri;
}

string CXmlDocSource::getSchemaUri() const
{
    // Adding a trailing '/' is a bit dirty but works fine on both Linux and
    // Windows in order to make sure that libxml2's URI handling methods
    // interpret the base URI as a folder.
    return mkUri(_schemaBaseUri + "/", getRootElementName() + ".xsd");
}

_xmlDoc *CXmlDocSource::getDoc() const
{
    return _pDoc;
}

bool CXmlDocSource::isParsable() const
{
    // Check that the doc has been created
    return _pDoc != NULL;
}

bool CXmlDocSource::populate(CXmlSerializingContext &serializingContext)
{
    // Check that the doc has been created
    if (!_pDoc) {

        serializingContext.setError("Could not parse document ");

        return false;
    }

    // Validate if necessary
    if (_bValidateWithSchema) {
        if (!isInstanceDocumentValid()) {

            serializingContext.setError("Document is not valid");

            return false;
        }
    }

    // Check Root element type
    if (getRootElementName() != _strRootElementType) {

        serializingContext.setError("Error: Wrong XML structure document ");
        serializingContext.appendLineToError("Root Element " + getRootElementName() +
                                             " mismatches expected type " + _strRootElementType);

        return false;
    }

    if (!_strNameAttributeName.empty()) {

        string strRootElementNameCheck = getRootElementAttributeString(_strNameAttributeName);

        // Check Root element name attribute (if any)
        if (!_strRootElementName.empty() && strRootElementNameCheck != _strRootElementName) {

            serializingContext.setError("Error: Wrong XML structure document ");
            serializingContext.appendLineToError(
                _strRootElementType + " element " + _strRootElementName + " mismatches expected " +
                _strRootElementType + " type " + strRootElementNameCheck);

            return false;
        }
    }

    return true;
}

bool CXmlDocSource::isInstanceDocumentValid()
{
#ifdef LIBXML_SCHEMAS_ENABLED
    string schemaUri = getSchemaUri();

    xmlDocPtr pSchemaDoc = xmlReadFile(schemaUri.c_str(), NULL, XML_PARSE_NONET);

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

std::string CXmlDocSource::mkUri(const std::string &base, const std::string &relative)
{
    xml_unique_ptr baseUri(xmlPathToURI((const xmlChar *)base.c_str()), xmlFree);
    xml_unique_ptr relativeUri(xmlPathToURI((const xmlChar *)relative.c_str()), xmlFree);
    /* return null pointer if baseUri or relativeUri are null pointer  */
    xml_unique_ptr xmlUri(xmlBuildURI(relativeUri.get(), baseUri.get()), xmlFree);

    ALWAYS_ASSERT(xmlUri != nullptr, "unable to make URI from: \"" << base << "\" and \""
                                                                   << relative << "\"");

    return (const char *)xmlUri.get();
}

_xmlDoc *CXmlDocSource::mkXmlDoc(const string &source, bool fromFile, bool xincludes,
                                 CXmlSerializingContext &serializingContext)
{
    _xmlDoc *doc = NULL;
    if (fromFile) {
        doc = xmlReadFile(source.c_str(), NULL, 0);
    } else {
        doc = xmlReadMemory(source.c_str(), (int)source.size(), "", NULL, 0);
    }

    if (doc == NULL) {
        string errorMsg = "libxml failed to read";
        if (fromFile) {
            errorMsg += " \"" + source + "\"";
        }
        serializingContext.appendLineToError(errorMsg);

        return NULL;
    }

    if (xincludes and (xmlXIncludeProcess(doc) < 0)) {
        serializingContext.appendLineToError("libxml failed to resolve XIncludes");

        xmlFreeDoc(doc);
        doc = NULL;
    }

    return doc;
}
