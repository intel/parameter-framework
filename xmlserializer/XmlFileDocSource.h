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

#pragma once

#include "XmlDocSource.h"
#include "XmlSource.h"

struct _xmlError;

class CXmlFileDocSource : public CXmlDocSource
{
public:
    CXmlFileDocSource(const string& strXmlInstanceFile, const string& strXmlSchemaFile, const string& strRootElementType, const string& strRootElementName, const string& strNameAttrituteName);

    CXmlFileDocSource(const string& strXmlInstanceFile, const string& strXmlSchemaFile, const string& strRootElementType);


    // CXmlDocSource method implemented
    virtual bool populate(CXmlSerializingContext& serializingContext);

    // Check that the file exists and is readable
    virtual bool isParsable(CXmlSerializingContext& serializingContext) const;

private:

    // Validation of the document with the xsd file
    bool isInstanceDocumentValid();

    static void schemaValidityStructuredErrorFunc(void* pUserData, _xmlError* pError);

    // Instance file
    string _strXmlInstanceFile;
    // Schema file
    string _strXmlSchemaFile;
    // Element type info
    string _strRootElementType;
    // Element name info
    string _strRootElementName;
    // Element name attribute info
    string _strNameAttrituteName;

    bool _bNameCheck;

};
