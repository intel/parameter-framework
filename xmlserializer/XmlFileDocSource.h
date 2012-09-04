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

#pragma once
#include "XmlDocSource.h"
#include <string>

/**
  * Source class that read a file to get an xml document.
  * Its base class will validate the document.
  */
class CXmlFileDocSource : public CXmlDocSource
{
public:
    /**
      * Constructor
      *
      * @param[in] strXmlInstanceFile a string containing the path to the xml file
      * @param[in] strXmlSchemaFile a string containing the path to the schema file
      * @param[in] strRootElementType a string containing the root element type
      * @param[in] strRootElementName a string containing the root element name
      * @param[in] strNameAttributeName a string containing the name of the root name attribute
      */
    CXmlFileDocSource(const string& strXmlInstanceFile,
                      const string& strXmlSchemaFile,
                      const string& strRootElementType,
                      const string& strRootElementName,
                      const string& strNameAttrituteName);
    /**
      * Constructor
      *
      * @param[in] strXmlInstanceFile a string containing the path to the xml file
      * @param[in] strXmlSchemaFile a string containing the path to the schema file
      * @param[in] strRootElementType a string containing the root element type
      */
    CXmlFileDocSource(const string& strXmlInstanceFile, const string& strXmlSchemaFile, const string& strRootElementType);

    /**
      * CXmlDocSource method implementation.
      *
      * @param[out] serializingContext is used as error output
      *
      * @return false if any error occurs
      */
    virtual bool populate(CXmlSerializingContext& serializingContext);

    /**
      * Method that checks that the file exists and is readable.
      *
      * @param[out] serializingContext is used as error output
      *
      * @return false if any error occurs during the parsing
      */
    virtual bool isParsable(CXmlSerializingContext& serializingContext) const;

private:

    /**
      * Instance file
      */
    string _strXmlInstanceFile;
};
