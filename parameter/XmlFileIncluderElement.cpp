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
#include "XmlFileIncluderElement.h"
#include "XmlDocSource.h"
#include "XmlMemoryDocSink.h"
#include "XmlElementSerializingContext.h"
#include "ElementLibrary.h"
#include <assert.h>
#include <fstream>

#define base CKindElement
CXmlFileIncluderElement::CXmlFileIncluderElement(const std::string &strName,
                                                 const std::string &strKind,
                                                 bool bValidateWithSchemas,
                                                 const std::string &schemaBaseUri)
    : base(strName, strKind), _bValidateSchemasOnStart(bValidateWithSchemas),
      _schemaBaseUri(schemaBaseUri)
{
}

// From IXmlSink
bool CXmlFileIncluderElement::fromXml(const CXmlElement &xmlElement,
                                      CXmlSerializingContext &serializingContext)
{
    // Context
    CXmlElementSerializingContext &elementSerializingContext =
        static_cast<CXmlElementSerializingContext &>(serializingContext);

    // Parse included document
    std::string strPath;
    xmlElement.getAttribute("Path", strPath);
    strPath = CXmlDocSource::mkUri(elementSerializingContext.getXmlUri(), strPath);

    // Instantiate parser
    std::string strIncludedElementType = getIncludedElementType();
    {
        _xmlDoc *doc = CXmlDocSource::mkXmlDoc(strPath, true, true, elementSerializingContext);

        CXmlDocSource docSource(doc, _bValidateSchemasOnStart, strIncludedElementType);

        if (not _schemaBaseUri.empty()) {
            docSource.setSchemaBaseUri(_schemaBaseUri);
        }

        if (!docSource.isParsable()) {

            elementSerializingContext.setError("Could not parse document \"" + strPath + "\"");

            return false;
        }

        // Get top level element
        CXmlElement childElement;

        docSource.getRootElement(childElement);

        // Create child element
        CElement *pChild =
            elementSerializingContext.getElementLibrary()->createElement(childElement);

        if (pChild) {

            // Store created child!
            getParent()->addChild(pChild);
        } else {

            elementSerializingContext.setError("Unable to create XML element " +
                                               childElement.getPath());

            return false;
        }

        // Use a doc sink that instantiate the structure from the doc source
        CXmlMemoryDocSink memorySink(pChild);

        if (!memorySink.process(docSource, elementSerializingContext)) {

            return false;
        }
    }
    // Detach from parent
    getParent()->removeChild(this);

    // Self destroy
    delete this;

    return true;
}

// Element type
std::string CXmlFileIncluderElement::getIncludedElementType() const
{
    std::string strKind = getKind();

    std::string::size_type pos = strKind.rfind("Include", std::string::npos);

    assert(pos != std::string::npos);

    return strKind.substr(0, pos);
}
