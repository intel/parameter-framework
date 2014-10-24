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
    bool process(CXmlDocSource& xmlDocSource, CXmlSerializingContext& serializingContext)
    {
        if (!xmlDocSource.populate(serializingContext)) {
            return false;
        }

        return doProcess(xmlDocSource, serializingContext);
    }

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
