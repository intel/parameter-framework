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
#include "XmlDocSource.h"
#include "XmlSerializingContext.h"

/**
  * The CXmlDocSink class defines how to use a CXmlDocSource.
  * The interaction between the xml source and xml sink is defined
  * in the process method of CXmlDocSink. One can subclass CXmlDocSink
  * for different purpose by implementing the doProcess method and then
  * use it with any existing implementation of CXmlDocSource.
  */
class CXmlDocSink
{
public:
    CXmlDocSink();

    /**
      * Method to be called to use an xmlDocSource.
      * Any subclass of XmlDocSink must implement the doProcess
      * method that will define how to use the xmlDocSource.
      *
      * @param[in] xmlDocSource a CXmlDocSource reference
      * @param[in] serializingContext a CXmlSerializing Context reference
      *
      * @return true is there was no error during the processing of xmlDocSource
      */
    bool process(CXmlDocSource& xmlDocSource, CXmlSerializingContext& serializingContext);
    virtual ~CXmlDocSink() {}

private:
    /**
      * Handle for subclasses to process the source.
      * This method will define what to do with the xmlDocSource.
      *
      * @param[in] xmlDocSource a CXmlDocSource reference
      * @param[in] serializingContext a CXmlSerializing Context reference
      *
      * @return true is there was no error during the processing of xmlDocSource
      */
    virtual bool doProcess(CXmlDocSource& xmlDocSource, CXmlSerializingContext& serializingContext) = 0;
};
