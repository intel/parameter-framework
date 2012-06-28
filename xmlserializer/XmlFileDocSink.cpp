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

#include "XmlFileDocSink.h"
#include <libxml/parser.h>

#define base CXmlDocSink

CXmlFileDocSink::CXmlFileDocSink(const string& strXmlInstanceFile):
     _strXmlInstanceFile(strXmlInstanceFile)
{
}

bool CXmlFileDocSink::doProcess(CXmlDocSource& xmlDocSource, CXmlSerializingContext& serializingContext)
{
    // Write file (formatted)
    if (xmlSaveFormatFileEnc(_strXmlInstanceFile.c_str(), xmlDocSource.getDoc(), "UTF-8", 1) == -1) {

        serializingContext.setError("Could not write file " + _strXmlInstanceFile);

        return false;
    }
    return true;
}

