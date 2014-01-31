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
#include <libxml/xinclude.h>

#define base CXmlDocSource

CXmlFileDocSource::CXmlFileDocSource(const string& strXmlInstanceFile,
                                     const string& strXmlSchemaFile,
                                     const string& strRootElementType,
                                     const string& strRootElementName,
                                     const string& strNameAttrituteName) :
        base(readFile(strXmlInstanceFile),
             strXmlSchemaFile,
             strRootElementType,
             strRootElementName,
             strNameAttrituteName),
        _strXmlInstanceFile(strXmlInstanceFile)
{
}

CXmlFileDocSource::CXmlFileDocSource(const string& strXmlInstanceFile,
                                     const string& strXmlSchemaFile,
                                     const string& strRootElementType) :
        base(readFile(strXmlInstanceFile),
             strXmlSchemaFile,
             strRootElementType),
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

_xmlDoc* CXmlFileDocSource::readFile(const string& strFileName)
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
