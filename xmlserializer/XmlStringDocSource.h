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

#pragma once
#include "XmlDocSource.h"
#include <string>

/**
  * Source class that get an xml document from a string.
  * Its base class will check the validity of the document.
  */
class CXmlStringDocSource : public CXmlDocSource
{
public:
    /**
      * Constructor
      *
      * @param[in] strXmlInput a string containing an xml description
      * @param[in] strXmlSchemaFile a string containing the path to the schema file
      * @param[in] strRootElementType a string containing the root element type
      * @param[in] strRootElementName a string containing the root element name
      * @param[in] strNameAttributeName a string containing the name of the root name attribute
      * @param[in] bValidateWithSchema a boolean that toggles schema validation
      */
    CXmlStringDocSource(const string& strXmlInput,
                        const string& strXmlSchemaFile,
                        const string& strRootElementType,
                        const string& strRootElementName,
                        const string& strNameAttrituteName,
                        bool bValidateWithSchema);

    /**
      * CXmlDocSource method implementation.
      *
      * @param[out] serializingContext is used as error output
      *
      * @return false if any error occurs
      */
    virtual bool populate(CXmlSerializingContext& serializingContext);
};


