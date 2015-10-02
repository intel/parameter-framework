/*
 * Copyright (c) 2015, Intel Corporation
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
#include "ConfigurableElementAccessor.h"
#include "ConfigurableElement.h"
#include "ConfigurationAccessContext.h"
#include "Subsystem.h"
#include <string>

using std::string;

ConfigurableElementAccessor::ConfigurableElementAccessor(
        const CConfigurableElement *configurableElement, CParameterBlackboard *pParameterBlackboard,
        bool valueSpaceIsRaw, bool outputRawFormatIsHex)
    : mConfigurableElement(configurableElement),
      mParameterBlackboard(pParameterBlackboard),
      mValueSpaceIsRaw(valueSpaceIsRaw),
      mOutputRawFormatIsHex(outputRawFormatIsHex)
{
}

bool ConfigurableElementAccessor::fromXml(const CXmlElement &xmlElement, CXmlSerializingContext &serializingContext)
{
    return serializeXmlSettings(const_cast<CXmlElement &>(xmlElement), serializingContext, false);
}

void ConfigurableElementAccessor::toXml(CXmlElement &xmlElement, CXmlSerializingContext &serializingContext) const
{
    serializeXmlSettings(xmlElement, serializingContext, true);
}

bool ConfigurableElementAccessor::serializeXmlSettings(CXmlElement &xmlElement, CXmlSerializingContext &serializingContext, bool bOut) const
{
    string strError;

    // Create configuration access context
    CConfigurationAccessContext configurationAccessContext(strError, bOut);

    // Provide current value space
    configurationAccessContext.setValueSpaceRaw(mValueSpaceIsRaw);

    // Provide current output raw format
    configurationAccessContext.setOutputRawFormat(mOutputRawFormatIsHex);

    // Deal with Endianness
    const CSubsystem *subsystem = mConfigurableElement->getBelongingSubsystem();

    if (subsystem && subsystem != mConfigurableElement) {

        // Element is a descendant of subsystem

        // Retrieve subsystem Endianness
        configurationAccessContext.setBigEndianSubsystem(subsystem->isBigEndian());
    }

    // Assign blackboard to configuration context
    configurationAccessContext.setParameterBlackboard(mParameterBlackboard);

    // Assign base offset to configuration context
    // Put 0 for main blackboard access
    configurationAccessContext.setBaseOffset(0);

    // Serialize values as XML (element contents)
    if (!mConfigurableElement->serializeXmlSettings(xmlElement, configurationAccessContext)) {

        serializingContext.setError(strError);

        return false;
    }

    // Synchronize (or defer synchronization) in case of element update (XML import)
    return bOut || mConfigurableElement->sync(configurationAccessContext);
}
