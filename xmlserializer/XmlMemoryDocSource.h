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
 *
 */

#pragma once
#include <string>
#include "XmlDocSource.h"
#include "XmlSource.h"

using std::string;

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
      */
    CXmlMemoryDocSource(const IXmlSource* pXmlSource, const string& strRootElementType,
                        const string& strXmlSchemaFile, const string& strProduct,
                        const string& strVersion);

    /**
      * Constructor
      *
      * @param[in] pXmlSource a pointer to a parameter-framework structure that can generate
      * an xml description of itself
      * @param[in] strRootElementType a string containing the root element type
      */
    CXmlMemoryDocSource(const IXmlSource* pXmlSource, const string& strRootElementType);

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
    string _strXmlSchemaFile;

    /**
      * Boolean used to specify if a header should be added in the Xml Doc
      */
    bool _bWithHeader;

    // Product and version info
    string _strProduct;
    string _strVersion;
};
