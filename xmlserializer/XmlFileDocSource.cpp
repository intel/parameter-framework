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

#include "XmlFileDocSource.h"
#include <libxml/parser.h>
#include <libxml/xinclude.h>

#define base CXmlDocSource

CXmlFileDocSource::CXmlFileDocSource(const std::string& strXmlInstanceFile,
                                     const std::string& strXmlSchemaFile,
                                     const std::string& strRootElementType,
                                     const std::string& strRootElementName,
                                     const std::string& strNameAttributeName,
                                     bool bValidateWithSchema) :
        base(readFile(strXmlInstanceFile),
             strXmlSchemaFile,
             strRootElementType,
             strRootElementName,
             strNameAttributeName,
             bValidateWithSchema),
        _strXmlInstanceFile(strXmlInstanceFile)
{
}

CXmlFileDocSource::CXmlFileDocSource(const std::string& strXmlInstanceFile,
                                     const std::string& strXmlSchemaFile,
                                     const std::string& strRootElementType,
                                     bool bValidateWithSchema) :
        base(readFile(strXmlInstanceFile),
             strXmlSchemaFile,
             strRootElementType,
             bValidateWithSchema),
        _strXmlInstanceFile(strXmlInstanceFile)
{
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

bool CXmlFileDocSource::populate(CXmlSerializingContext& serializingContext)
{
    if (!validate(serializingContext)) {

        // Add the file's name in the error message
        serializingContext.appendLineToError("File : " + _strXmlInstanceFile);

        return false;
    }

    return true;
}

_xmlDoc* CXmlFileDocSource::readFile(const std::string& strFileName)
{
    // Read xml file
    xmlDocPtr pDoc = xmlReadFile(strFileName.c_str(), NULL, 0);

    if (!pDoc) {

        return NULL;
    }
    // Process file inclusion
    // WARNING: this symbol is available if libxml2 has been compiled with LIBXML_XINCLUDE_ENABLED
    if (xmlXIncludeProcess(pDoc) < 0) {

        xmlFreeDoc(pDoc);
        return NULL;
    }

    return pDoc;
}
