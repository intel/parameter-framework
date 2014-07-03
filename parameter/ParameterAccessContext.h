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

#include <stdint.h>
#include "ErrorContext.h"
#include <string>

class CParameterBlackboard;

class CParameterAccessContext : public CErrorContext
{
public:
    CParameterAccessContext(std::string& strError,
                            CParameterBlackboard* pParameterBlackboard,
                            bool bValueSpaceIsRaw,
                            bool bOutputRawFormatIsHex = false,
                            uint32_t uiOffsetBase = 0);
    CParameterAccessContext(std::string& strError,
                            bool bBigEndianSubsystem,
                            CParameterBlackboard* pParameterBlackboard,
                            uint32_t uiOffsetBase = 0);
    CParameterAccessContext(std::string& strError);

    // ParameterBlackboard
    CParameterBlackboard* getParameterBlackboard();
    void setParameterBlackboard(CParameterBlackboard* pBlackboard);

    // Value interpretation as Real or Raw
    void setValueSpaceRaw(bool bIsRaw);
    bool valueSpaceIsRaw() const;

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

    // Endianness
    void setBigEndianSubsystem(bool bBigEndian);
    bool isBigEndianSubsystem() const;

    // Automatic synchronization to HW
    void setAutoSync(bool bAutoSync);
    bool getAutoSync() const;

    // Base offset for blackboard access
    void setBaseOffset(uint32_t uiBaseOffset);
    uint32_t getBaseOffset() const;

private:
    // Blackboard
    CParameterBlackboard* _pParameterBlackboard;
    // Value space
    bool _bValueSpaceIsRaw;
    // Output Raw Format
    bool _bOutputRawFormatIsHex;
    // Subsystem Endianness
    bool _bBigEndianSubsystem;
    // Automatic synchronization to HW
    bool _bAutoSync;
    // Base offset where parameters are stored in configuration blackboards
    uint32_t _uiBaseOffset;
};

