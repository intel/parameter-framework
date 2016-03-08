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

#include "IntrospectionSubsystemObject.h"
#include <InstanceConfigurableElement.h>
#include <ParameterType.h>

namespace parameterFramework
{
namespace introspectionSubsystem
{

const SubsystemObject *SubsystemObject::mSingletonInstance = nullptr;

/* Helper function */
const CParameterType *geParameterType(CInstanceConfigurableElement *element)
{
    return static_cast<const CParameterType *>(element->getTypeElement());
}

SubsystemObject::SubsystemObject(const std::string & /*mappingValue*/,
                                 CInstanceConfigurableElement *instanceConfigurableElement,
                                 const CMappingContext & /*context*/, core::log::Logger &logger)
    : base(instanceConfigurableElement, logger), mParameter(false)
{
    /* Checking that structure matches the internal parameter */
    ALWAYS_ASSERT(geParameterType(instanceConfigurableElement)->getSize() == parameterSize,
                  "Wrong parameter size");
    ALWAYS_ASSERT((instanceConfigurableElement->getFootPrint() / parameterSize) == 1,
                  "Parameter shall not be an array");
    ALWAYS_ASSERT(geParameterType(instanceConfigurableElement)->isScalar(),
                  "Parameter shall be scalar");

    /* Registering the instance into a singleton */
    registerInstance(*this);
}

SubsystemObject::~SubsystemObject()
{
    /* Unregistering the instance from the singleton */
    unregisterInstance(*this);
}

bool SubsystemObject::sendToHW(std::string & /*error*/)
{
    blackboardRead(&mParameter, parameterSize);
    return true;
}

bool SubsystemObject::receiveFromHW(std::string & /*error*/)
{
    blackboardRead(&mParameter, parameterSize);
    return true;
}
} // namespace introspectionSubsystem
} // namespace parameterFramework
