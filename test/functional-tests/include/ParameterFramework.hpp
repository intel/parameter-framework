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
#include "ConfigFiles.hpp"
#include "FailureWrapper.hpp"
#include "ParameterHandle.hpp"

#include <ParameterMgrFullConnector.h>

namespace parameterFramework
{

/** Wrapper around the Parameter Framework to throw exceptions on errors and
 *  have more user friendly methods.
 */
class ParameterFramework : private parameterFramework::ConfigFiles,
                           private FailureWrapper<CParameterMgrFullConnector>
{
protected:
    /** Alias to the Platform Connector PF.
     * It should not be usefull as PF is a super set but is useful
     * to disambiguate overloaded method for MS visual compiler.
     */
    using PPF = CParameterMgrPlatformConnector;
    using PF = CParameterMgrFullConnector;

public:
    ParameterFramework(const Config &config = Config()) :
        ConfigFiles(config),
        FailureWrapper(getPath()) {}

    void start() {
        setForceNoRemoteInterface(true);
        mayFailCall(&PF::start);
    }

    /** @name Forwarded methods
     * Forward those methods without modification as there are ergonomic and
     * can not fail (no failure to throw).
     * @{ */
    using PF::applyConfigurations;
    using PF::getFailureOnMissingSubsystem;
    using PF::getFailureOnFailedSettingsLoad;
    using PF::getForceNoRemoteInterface;
    using PF::setForceNoRemoteInterface;
    using PF::getSchemaUri;
    using PF::setSchemaUri;
    using PF::getValidateSchemasOnStart;
    using PF::setValidateSchemasOnStart;
    using PF::isValueSpaceRaw;
    using PF::isOutputRawFormatHex;
    using PF::setOutputRawFormat;
    using PF::isTuningModeOn;
    using PF::isAutoSyncOn;
    using PF::setLogger;
    /** @} */

    /** Wrap PF::setValidateSchemasOnStart to throw an exception on failure. */
    void setValidateSchemasOnStart(bool validate) {
        mayFailCall(&PPF::setValidateSchemasOnStart, validate);
    }

    /** Wrap PF::setFailureOnFailedSettingsLoad to throw an exception on failure. */
    void setFailureOnFailedSettingsLoad(bool fail) {
        mayFailCall(&PPF::setFailureOnFailedSettingsLoad, fail);
    }

    /** Wrap PF::setFailureOnMissingSubsystem to throw an exception on failure. */
    void setFailureOnMissingSubsystem(bool fail) {
        mayFailCall(&PPF::setFailureOnMissingSubsystem, fail);
    }

    /** Renaming for better readability (and coherency with PF::isValueSpaceRaw)
     *  of PF::setValueSpace. */
    void setRawValueSpace(bool enable) { setValueSpace(enable); }

    /** Wrap PF::setTuningMode to throw an exception on failure. */
    void setTuningMode(bool enable) { mayFailCall(&PF::setTuningMode, enable); }

    /** Wrap PF::setAutoSync to throw an exception on failure. */
    void setAutoSync(bool enable) { mayFailCall(&PF::setAutoSync, enable); }

    /** Wrap PF::accessParameterValue in "set" mode (and rename it) to throw an
     * exception on failure
     */
    void setParameter(const std::string& path, std::string& value)
    {
        mayFailCall(&PF::accessParameterValue, path, value, true);
    }
    /** Wrap PF::accessParameterValue in "get" mode (and rename it) to throw an
     * exception on failure
     */
    void getParameter(const std::string& path, std::string& value)
    {
        mayFailCall(&PF::accessParameterValue, path, value, false);
    }

    // Dynamic parameter handling
    ParameterHandle* createParameterHandle(const std::string& path) {
        return new ParameterHandle(mayFailCall(&PF::createParameterHandle, path));
    }

};

} // parameterFramework
