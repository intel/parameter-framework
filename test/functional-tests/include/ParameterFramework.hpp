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

#include "Config.hpp"

#include <ParameterMgrFullConnector.h>

/** Wrapper around the Parameter Framework to throw exceptions on errors and
 *  have more user friendly methods.
 */
class ParameterFramework : private CParameterMgrFullConnector
{
private:
    using PF = CParameterMgrFullConnector;

    template <class Type>
    using Getter = Type (PF::*)() const;
    template <class Type>
    using Setter = void (PF::*)(Type);

    template <class PType, Getter<PType> getter, Setter<PType> setter>
    struct Prop {
        using Type = PType;
        static Type get(const PF &pf) {
            return (pf.*getter)();
        }
        static void set(PF &pf, Type value) {
            (pf.*setter)(value);
        }
    };

    template <class Type>
    using MayFailSetter = bool (PF::*)(Type, std::string &);

    template <class PType, Getter<PType> getter, MayFailSetter<PType> setter>
    struct MayFailProp {
        using Type = PType;
        static Type get(const PF &pf) {
            return (pf.*getter)();
        }
        static void set(PF &pf, Type value) {
            mayFailCall(pf, setter, value);
        }
    };

public:
    using FailureOnMissingSubsystem =
        Prop<bool, &PF::getFailureOnMissingSubsystem, &PF::setFailureOnMissingSubsystem>;
    using FailureOnFailedSettingsLoad =
        Prop<bool, &PF::getFailureOnFailedSettingsLoad, &PF::setFailureOnFailedSettingsLoad>;
    using ForceNoRemoteInterface =
        Prop<bool, &PF::getForceNoRemoteInterface, &PF::setForceNoRemoteInterface>;
    using SchemaFolderLocation =
        Prop<const std::string &, &PF::getSchemaFolderLocation, &PF::setSchemaFolderLocation>;
    using Validate =
        Prop<bool, &PF::getValidateSchemasOnStart, &PF::setValidateSchemasOnStart>;
    using RawValueSpace =
        Prop<bool, &PF::isValueSpaceRaw, &PF::setValueSpace>;
    using HexOutputFormat =
        Prop<bool, &PF::isOutputRawFormatHex, &PF::setOutputRawFormat>;
    using TuningMode =
        MayFailProp<bool,  &PF::isTuningModeOn, &PF::setTuningMode>;
    using Autosync =
        MayFailProp<bool, &PF::isAutoSyncOn, &PF::setAutoSync>;
    using Logger =
        Prop<ILogger *, nullptr, &PF::setLogger>;



    void start() {
        mayFailCall(&PF::start);
    }

    /** @name Forwarded methods
     * Forward those methods without modification as they are no failure to
     * forward as exception and are already ergonomic.
     * @{ */
    using PF::applyConfigurations;
    /** @} */

    template <class Property>
    typename Property::Type get() const { return Property::get(*this); };
    template <class Property>
    void set(typename Property::Type value) { Property::set(*this, value); }

private:
    /** Wrap a method that may fail to throw an Exception instead of retuning a boolean.
     * @param[in] method that return a boolean to indicate failure.
     * @param[in] args parameters to call method call with. */
    template <class... MArgs, class... Args>
    static void mayFailCall(PF &pf, bool (PF::*method)(MArgs...), Args&&... args) {
        std::string error;
        if (not (pf.*method)(std::forward<Args>(args)..., error)) {
            throw Exception(std::move(error));
        }
    }
    /** mayFailCall version that implicitly call the method on mPf. */
    template <class Method, class... Args>
    void mayFailCall(Method method, Args... args) {
        mayFailCall(static_cast<PF&>(*this), method, std::forward<Args>(args)...);
    }
};

