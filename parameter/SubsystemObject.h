/*
 * INTEL CONFIDENTIAL
 * Copyright © 2011 Intel 
 * Corporation All Rights Reserved.
 * 
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its suppliers
 * or licensors. Title to the Material remains with Intel Corporation or its
 * suppliers and licensors. The Material contains trade secrets and proprietary
 * and confidential information of Intel or its suppliers and licensors. The
 * Material is protected by worldwide copyright and trade secret laws and
 * treaty provisions. No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel’s prior express written permission.
 * 
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Intel in writing.
 * 
 * CREATED: 2011-06-01
 * UPDATED: 2011-07-27
 */
#pragma once

#include "Syncer.h"
#include <stdint.h>

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
     * @return A string containing the mapping value
     */
    virtual string getFormattedMappingValue() const;

    // Configurable element retrieval
    const CInstanceConfigurableElement* getConfigurableElement() const;

protected:
    // Blackboard data location
    uint8_t* getBlackboardLocation() const;
    // Size
    uint32_t getSize() const;
    // Conversion utility
    static uint32_t asInteger(const string& strValue);
    static string asString(uint32_t uiValue);

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
    virtual bool sendToHW(string& strError);
    virtual bool receiveFromHW(string& strError);
    // Fall back HW access
    virtual bool accessHW(bool bReceive, string& strError);
    // Blackboard access from subsystems
    void blackboardRead(void* pvData, uint32_t uiSize);
    void blackboardWrite(const void* pvData, uint32_t uiSize);
    // Logging
    void log_info(const string& strMessage, ...) const;
    void log_warning(const string& strMessage, ...) const;
    // Belonging Subsystem retrieval
    const CSubsystem* getSubsystem() const;

private:
    // from ISyncer
    virtual bool sync(CParameterBlackboard& parameterBlackboard, bool bBack, string& strError);

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

