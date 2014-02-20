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
#include "ParameterFrameworkConfiguration.h"

#define base CElement

CParameterFrameworkConfiguration::CParameterFrameworkConfiguration()
    : _bTuningAllowed(false), _uiServerPort(0)
{
}

std::string CParameterFrameworkConfiguration::getKind() const
{
    return "ParameterFrameworkConfiguration";
}

bool CParameterFrameworkConfiguration::childrenAreDynamic() const
{
    return true;
}

// System class name
const std::string& CParameterFrameworkConfiguration::getSystemClassName() const
{
    return _strSystemClassName;
}

// Tuning allowed
bool CParameterFrameworkConfiguration::isTuningAllowed() const
{
    return _bTuningAllowed;
}

// Server port
uint16_t CParameterFrameworkConfiguration::getServerPort() const
{
    return _uiServerPort;
}

// From IXmlSink
bool CParameterFrameworkConfiguration::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    // System class name
    _strSystemClassName = xmlElement.getAttributeString("SystemClassName");

    // Tuning allowed
    _bTuningAllowed = xmlElement.getAttributeBoolean("TuningAllowed");

    // Server port
    _uiServerPort = (uint16_t)xmlElement.getAttributeInteger("ServerPort");

    // Base
    return base::fromXml(xmlElement, serializingContext);
}
