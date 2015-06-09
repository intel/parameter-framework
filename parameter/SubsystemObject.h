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

#include "Syncer.h"
#include <stdint.h>

#include <string>

class CInstanceConfigurableElement;
class CMappingContext;
class CSubsystem;

class CSubsystemObject : private ISyncer
{
public:
    CSubsystemObject(CInstanceConfigurableElement* pInstanceConfigurableElement);
    virtual ~CSubsystemObject();

    /**
     * Return the mapping value of the SubystemObject.
     *
     * @return A std::string containing the mapping value
     */
    virtual std::string getFormattedMappingValue() const;

    // Configurable element retrieval
    const CInstanceConfigurableElement* getConfigurableElement() const;

protected:
    // Blackboard data location
    uint8_t* getBlackboardLocation() const;
    // Size
    uint32_t getSize() const;
    // Conversion utility
    static uint32_t asInteger(const std::string& strValue);
    static std::string asString(uint32_t uiValue);

    /**
     * Conversion of int8, int16, int32 to int (taking care of sign extension)
     *
     * @param[in] instanceConfigurableElement pointer to configurable element instance
     * @param[in] sizeOptimizedData data to convert
     *
     * @return the data converted to int
     */
    int toPlainInteger(const CInstanceConfigurableElement *instanceConfigurableElement,
                       int sizeOptimizedData);

    // Sync to/from HW
    virtual bool sendToHW(std::string& strError);
    virtual bool receiveFromHW(std::string& strError);
    // Fall back HW access
    virtual bool accessHW(bool bReceive, std::string& strError);
    // Blackboard access from subsystems
    void blackboardRead(void* pvData, uint32_t uiSize);
    void blackboardWrite(const void* pvData, uint32_t uiSize);
    // Logging
    // Copy the string format because:
    //  - passing char * would break compatibility
    //  - passing a const std::string & in forbiden by the c++ standard
    //    as va_start second argument must not be a reference.
    void log_info(std::string strMessage, ...) const;
    void log_warning(std::string strMessage, ...) const;
    // Belonging Subsystem retrieval
    const CSubsystem* getSubsystem() const;

private:
    // from ISyncer
    virtual bool sync(CParameterBlackboard& parameterBlackboard, bool bBack, std::string& strError);

    // Default back synchronization
    void setDefaultValues(CParameterBlackboard& parameterBlackboard) const;

    // Prevent unsupported operators
    CSubsystemObject(const CSubsystemObject&);

    // Define affection operator
    const CSubsystemObject& operator=(const CSubsystemObject&);

    // Instance element to sync from/to
    CInstanceConfigurableElement* _pInstanceConfigurableElement;
    // Data size
    uint32_t _uiDataSize;
    // Blackboard data location
    uint8_t* _pucBlackboardLocation;
    // Accessed index for Subsystem read/write from/to blackboard
    uint32_t _uiAccessedIndex;
};

