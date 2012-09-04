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
#include <string>
#include "XmlDocSink.h"
#include "XmlSource.h"

using std::string;

/**
  * Sink class that writes the content of any CXmlDocSource into a string.
  * A reference to an empty string is given in the constructor.
  */
class CXmlStringDocSink : public CXmlDocSink
{
public:
    /** Constructor
      *
      * @param[out] strResult a reference to a string that will be filled by the doProcess method
      */
    CXmlStringDocSink(string& strResult);

private:
    /** Implementation of CXmlDocSink::doProcess()
      * Writes the content of the xmlDocSource in strResult
      *
      * @param[in] xmlDocSource is the source containing the Xml document
      * @param[out] serializingContext is used as error output
      *
      * @return false if any error occurs
      */
    virtual bool doProcess(CXmlDocSource& xmlDocSource, CXmlSerializingContext& serializingContext);

    /**
      * Result string containing the XML informations
      */
    string& _strResult;
};

