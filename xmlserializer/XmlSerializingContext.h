/*
 * Copyright (c) 2011-2015, Intel Corporation
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

#include "ErrorContext.hpp"

#include <string>

/** Forward declare libxml2 handler structure. */
struct _xmlError;

/** Class that gather errors during serialization.
 *
 * Provided with an initial empty buffer (strError), an instance of this class
 * can describe an error.
 *
 * This error will be accessible formated in the aforementioned buffer
 * _after_ destruction.
 * Ie. the provided buffer (strError) is in an undefined state between
 * construction and destruction and should not be accessed in between.
 */
class CXmlSerializingContext : public utility::ErrorContext
{
public:
    CXmlSerializingContext(std::string &strError);
    ~CXmlSerializingContext();

    // Error
    void appendLineToError(const std::string &strAppend);

    /** XML error handler
      *
      * @param[in] userData pointer to the serializing context
      * @param[in] error the xml error output format
      *
      */
    static void structuredErrorHandler(void *userData, _xmlError *error);

private:
    std::string _strXmlError;
};
