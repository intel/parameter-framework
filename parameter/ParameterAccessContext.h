/*
 * Copyright (c) 2011-2016, Intel Corporation
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

#include <stdint.h>
#include "ErrorContext.hpp"
#include <string>

class CParameterBlackboard;

class CParameterAccessContext : public utility::ErrorContext
{
public:
    CParameterAccessContext(std::string &strError, CParameterBlackboard *pParameterBlackboard,
                            bool bValueSpaceIsRaw, bool bOutputRawFormatIsHex,
                            size_t offsetBase = 0);
    CParameterAccessContext(std::string &strError, CParameterBlackboard *pParameterBlackboard,
                            size_t offsetBase = 0);
    CParameterAccessContext(std::string &strError);
    virtual ~CParameterAccessContext() = default;

    // ParameterBlackboard
    CParameterBlackboard *getParameterBlackboard();
    void setParameterBlackboard(CParameterBlackboard *pBlackboard);

    // Value interpretation as Real or Raw
    void setValueSpaceRaw(bool bIsRaw);
    bool valueSpaceIsRaw() const;

    /** @return true if setting serialization is requested,
     *          false if structure serialization
     */
    virtual bool serializeSettings() const { return false; }

    /**
     * Assigns Output Raw Format for user get value interpretation.
     *
     * @param[in] bIsHex "true" for hexadecimal, "false" for decimal
     *
     */
    void setOutputRawFormat(bool bIsHex);
    /**
     * Returns Output Raw Format for user get value interpretation.
     *
     * @return "true" for hexadecimal, "false" for decimal
     *
     */
    bool outputRawFormatIsHex() const;

    // Automatic synchronization to HW
    void setAutoSync(bool bAutoSync);
    bool getAutoSync() const;

    // Base offset for blackboard access
    void setBaseOffset(size_t baseOffset);
    size_t getBaseOffset() const;

private:
    // Blackboard
    CParameterBlackboard *_pParameterBlackboard{nullptr};
    // Value space
    bool _bValueSpaceIsRaw{false};
    // Output Raw Format
    bool _bOutputRawFormatIsHex{false};
    // Automatic synchronization to HW
    bool _bAutoSync{true};
    // Base offset where parameters are stored in configuration blackboards
    size_t _uiBaseOffset{0};
};
