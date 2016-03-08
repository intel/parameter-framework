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
#pragma once

#include <SubsystemObject.h>
#include <AlwaysAssert.hpp>
#include <string>

class CMappingContext;

namespace parameterFramework
{
namespace introspectionSubsystem
{

/** This subsystem object exposes a boolean parameter. The value of this parameter
 * can be retrieved by an external code that calls the getSingletonInstanceValue()
 * static method.
 */
class SubsystemObject final : public CSubsystemObject
{
public:
    SubsystemObject(const std::string &mappingValue,
                    CInstanceConfigurableElement *instanceConfigurableElement,
                    const CMappingContext &context, core::log::Logger &logger);
    ~SubsystemObject();

    static bool getSingletonInstanceValue()
    {
        ALWAYS_ASSERT(mSingletonInstance != nullptr, "Singleton value has not been registered");
        return mSingletonInstance->mParameter;
    }

private:
    using base = CSubsystemObject;

    virtual bool sendToHW(std::string &error) override;
    virtual bool receiveFromHW(std::string &error) override;

    static void registerInstance(const SubsystemObject &instance)
    {
        ALWAYS_ASSERT(mSingletonInstance == nullptr, "An instance is already registered");
        mSingletonInstance = &instance;
    }

    static void unregisterInstance(const SubsystemObject &instance)
    {
        // instance parameter is only used by assertion, so unused in release mode
        (void)instance;

        ALWAYS_ASSERT(mSingletonInstance == &instance, "This instance was not registered.");
        mSingletonInstance = nullptr;
    }

    static const std::size_t parameterSize = sizeof(bool);

    static const SubsystemObject *mSingletonInstance;

    bool mParameter;
};
} // namespace introspectionSubsystem
} // namespace parameterFramework
