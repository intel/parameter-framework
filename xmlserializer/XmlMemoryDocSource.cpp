/*
 * Copyright (c) 2011-2014, Intel Corporation
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

#include "XmlMemoryDocSource.h"
#include <libxml/parser.h>
#include <libxml/tree.h>

#define base CXmlDocSource

CXmlMemoryDocSource::CXmlMemoryDocSource(const IXmlSource* pXmlSource, bool bValidateWithSchema,
                                         const std::string& strRootElementType,
                                         const std::string& strXmlSchemaFile,
                                         const std::string& strProduct,
                                         const std::string& strVersion):
     base(xmlNewDoc(BAD_CAST "1.0"), bValidateWithSchema,
          xmlNewNode(NULL, BAD_CAST strRootElementType.c_str())),
     _pXmlSource(pXmlSource),
     _strXmlSchemaFile(strXmlSchemaFile),
     _strProduct(strProduct),
     _strVersion(strVersion)
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
    serializingContext.setError("XML file exporting feature unsupported on this image. " +
                                "This easiest way to activate it is to do a global " +
                                "recompilation with LIBXML_TREE_ENABLED compiler switch set");

    return false;
#endif

    // Create Xml element with the Doc
     CXmlElement docElement(_pRootNode);

    if (!_strXmlSchemaFile.empty()) {

        // Schema namespace
        docElement.setAttributeString("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");

        // Schema location
        docElement.setAttributeString("xsi:noNamespaceSchemaLocation", _strXmlSchemaFile);
    }

    // Compose the xml document
    _pXmlSource->toXml(docElement, serializingContext);

    return true;
}
