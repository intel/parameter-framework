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
#include <string>
#include "XmlDocSource.h"
#include "XmlSource.h"

/**
  * Source class that uses parameter-framework's structures to create an xml document
  */
class CXmlMemoryDocSource : public CXmlDocSource
{
public:
    /**
      * Constructor
      *
      * @param[in] pXmlSource a pointer to a parameter-framework structure that can generate
      * an xml description of itself
      * @param[in] strRootElementType a string containing the root element type
      * @param[in] strXmlSchemaFile a string containing the path to the schema file
      * @param[in] strProduct a string containing the product name
      * @param[in] strVersion a string containing the version number
      * @param[in] bValidateWithSchema a boolean that toggles schema validation
      */
    CXmlMemoryDocSource(const IXmlSource* pXmlSource, bool bValidateWithSchema,
                        const std::string& strRootElementType,
                        const std::string& strXmlSchemaFile = "",
                        const std::string& strProduct = "",
                        const std::string& strVersion = "");

    /**
      * Implementation of CXmlDocSource::populate() method.
      * Method that popuplates the Xml document using the IXmlSource given in the constructor.
      *
      * @param[out] serializingContext is used as error output
      *
      * @return false if any error occurs
      */
    virtual bool populate(CXmlSerializingContext& serializingContext);
private:

    /**
      * Initialize root element
      */
    void init();

    /**
      * Xml Source
      */
    const IXmlSource* _pXmlSource;

    /**
      * Schema file
      */
    std::string _strXmlSchemaFile;

    // Product and version info
    std::string _strProduct;
    std::string _strVersion;
};
