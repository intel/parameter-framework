/* <auto_header>
 * <FILENAME>
 * 
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
 *  AUTHOR: Patrick Benavoli (patrickx.benavoli@intel.com)
 * CREATED: 2011-06-01
 * UPDATED: 2011-07-27
 * 
 * 
 * </auto_header>
 */
#pragma once

#include "XmlElement.h"
#include "XmlSerializingContext.h"

struct _xmlDoc;
struct _xmlNode;

class CXmlSerializer
{
public:
    CXmlSerializer(const string& strXmlInstanceFile, const string& strXmlSchemaFile, const string& strRootElementType, CXmlSerializingContext& serializingContext);
    virtual ~CXmlSerializer();

    // Open/Close
    virtual bool open();
    virtual bool close();

    // Root element
    void getRootElement(CXmlElement& xmlRootElement) const;
    string getRootElementName() const;
    string getRootElementAttributeString(const string& strAttributeName) const;
protected:
    // Instance file
    string _strXmlInstanceFile;

    // Schema file
    string _strXmlSchemaFile;

    // Root element type
    string _strRootElementType;

    // Serializing context
    CXmlSerializingContext& _serializingContext;

    // XML document
    _xmlDoc* _pDoc;

    // Root node
    _xmlNode* _pRootNode;

    // libxml2 library cleanup
    static bool _bLibXml2CleanupScheduled;
};
