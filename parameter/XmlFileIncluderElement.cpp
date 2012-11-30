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
 * CREATED: 2011-06-01
 * UPDATED: 2011-07-27
 */
#include "XmlFileIncluderElement.h"
#include "XmlFileDocSource.h"
#include "XmlMemoryDocSink.h"
#include "XmlElementSerializingContext.h"
#include "ElementLibrary.h"
#include "AutoLog.h"
#include <assert.h>

#define base CKindElement

CXmlFileIncluderElement::CXmlFileIncluderElement(const string& strName, const string& strKind) : base(strName, strKind)
{
}

// From IXmlSink
bool CXmlFileIncluderElement::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    // Context
    CXmlElementSerializingContext& elementSerializingContext = static_cast<CXmlElementSerializingContext&>(serializingContext);

    // Parse included document
    string strPath = xmlElement.getAttributeString("Path");

    // Relative path?
    if (strPath[0] != '/') {

        strPath = elementSerializingContext.getXmlFolder() + "/" + strPath;
    }

    // Instantiate parser
    string strIncludedElementType = getIncludedElementType();

    {
        // Open a log section titled with loading file path
        CAutoLog autolog(this, "Loading " + strPath);

        // Use a doc source that load data from a file
        CXmlFileDocSource fileDocSource(strPath, elementSerializingContext.getXmlSchemaPathFolder() + "/" + strIncludedElementType + ".xsd", strIncludedElementType);

        if (!fileDocSource.isParsable(elementSerializingContext)) {

            return false;
        }

        // Get top level element
        CXmlElement childElement;

        fileDocSource.getRootElement(childElement);

        // Create child element
        CElement* pChild = elementSerializingContext.getElementLibrary()->createElement(childElement);

        if (pChild) {

            // Store created child!
            getParent()->addChild(pChild);
        } else {

            elementSerializingContext.setError("Unable to create XML element " + childElement.getPath());

            return false;
        }

        // Use a doc sink that instantiate the structure from the doc source
        CXmlMemoryDocSink memorySink(pChild);

        if (!memorySink.process(fileDocSource, elementSerializingContext)) {

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
string CXmlFileIncluderElement::getIncludedElementType() const
{
    string strKind = getKind();

    int iPosToRemoveFrom = strKind.rfind("Include", -1);

    assert(iPosToRemoveFrom != -1);

    return strKind.substr(0, iPosToRemoveFrom);
}
