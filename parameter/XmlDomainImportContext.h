/*
 * Copyright (c) 2014, Intel Corporation
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

#include "XmlDomainSerializingContext.h"
#include "SystemClass.h"
#include <criterion/Criteria.h>

#include <string>

class CXmlDomainImportContext : public CXmlDomainSerializingContext
{
public:
    CXmlDomainImportContext(std::string& strError,
                            bool bWithSettings,
                            CSystemClass& systemClass,
                            const core::criterion::internal::Criteria& criteria)
        : base(strError, bWithSettings), _systemClass(systemClass), mCriteria(criteria),
          _bAutoValidationRequired(true)
    {}

    // System Class
    CSystemClass& getSystemClass() const
    {
        return _systemClass;
    }

    const core::criterion::internal::Criteria& getCriteria() const
    {
        return mCriteria;
    }

    // Auto validation of configurations
    void setAutoValidationRequired(bool bAutoValidationRequired)
    {
        _bAutoValidationRequired = bAutoValidationRequired;
    }

    bool autoValidationRequired() const
    {
        return _bAutoValidationRequired;
    }

private:
    typedef CXmlDomainSerializingContext base;

    // System Class
    CSystemClass& _systemClass;

    /** Selection criteria definition for rule creation */
    const core::criterion::internal::Criteria& mCriteria;

    // Auto validation of configurations
    bool _bAutoValidationRequired;
};

