/*
 * Copyright (c) 2011-2015, Intel Corporation
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
#include "SubsystemObject.h"
#include "Subsystem.h"
#include "InstanceConfigurableElement.h"
#include "ParameterBlackboard.h"
#include "ParameterAccessContext.h"
#include "MappingContext.h"
#include "ParameterType.h"
#include "convert.hpp"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <stdarg.h>

using std::string;

CSubsystemObject::CSubsystemObject(CInstanceConfigurableElement *pInstanceConfigurableElement,
                                   core::log::Logger &logger)
    : _logger(logger), _pInstanceConfigurableElement(pInstanceConfigurableElement),
      _dataSize(pInstanceConfigurableElement->getFootPrint())
{
    // Syncer
    _pInstanceConfigurableElement->setSyncer(this);
}

CSubsystemObject::~CSubsystemObject()
{
    _pInstanceConfigurableElement->unsetSyncer();
}

string CSubsystemObject::getFormattedMappingValue() const
{
    // Default formatted mapping value is empty
    return "";
}

// Blackboard data location
uint8_t *CSubsystemObject::getBlackboardLocation() const
{
    return _blackboard->getLocation(getOffset());
}

// Size
size_t CSubsystemObject::getSize() const
{
    return _dataSize;
}

int CSubsystemObject::toPlainInteger(
    const CInstanceConfigurableElement *instanceConfigurableElement, int sizeOptimizedData)
{
    if (instanceConfigurableElement) {

        // Get actual element type
        const CTypeElement *typeElement =
            static_cast<const CParameterType *>(instanceConfigurableElement->getTypeElement());

        // Do the extension
        return typeElement->toPlainInteger(sizeOptimizedData);
    }

    return sizeOptimizedData;
}

// Default back synchronization
void CSubsystemObject::setDefaultValues(CParameterBlackboard &parameterBlackboard) const
{
    string strError;

    // Create access context
    CParameterAccessContext parameterAccessContext(strError, &parameterBlackboard);

    // Just implement back synchronization with default values
    _pInstanceConfigurableElement->setDefaultValues(parameterAccessContext);
}

// Synchronization
bool CSubsystemObject::sync(CParameterBlackboard &parameterBlackboard, bool bBack, string &strError)
{
    // Get blackboard location
    _blackboard = &parameterBlackboard;
    // Access index init
    _accessedIndex = 0;

#ifdef SIMULATION
    return true;
#endif

    // Retrieve subsystem
    const CSubsystem *pSubsystem = _pInstanceConfigurableElement->getBelongingSubsystem();

    // Get it's health insdicator
    bool bIsSubsystemAlive = pSubsystem->isAlive();

    // Check subsystem health
    if (!bIsSubsystemAlive) {

        strError = "Susbsystem not alive";
    }

    // Synchronize to/from HW
    if (!bIsSubsystemAlive || !accessHW(bBack, strError)) {

        // Fall back to parameter default initialization
        if (bBack) {

            setDefaultValues(parameterBlackboard);
        }
        return false;
    }

    return true;
}

// Sync to/from HW
bool CSubsystemObject::sendToHW(string &strError)
{
    strError = "Send to HW interface not implemented at subsystem level";

    return false;
}

bool CSubsystemObject::receiveFromHW(string & /*strError*/)
{
    // Back synchronization is not supported at subsystem level.
    // Rely on blackboard content

    return true;
}

// Fall back HW access
bool CSubsystemObject::accessHW(bool bReceive, string &strError)
{
    // Default access fall back
    if (bReceive) {

        return receiveFromHW(strError);
    } else {

        return sendToHW(strError);
    }
}

// Blackboard access from subsystems
void CSubsystemObject::blackboardRead(void *pvData, size_t size)
{
    _blackboard->readBuffer(pvData, size, getOffset() + _accessedIndex);

    _accessedIndex += size;
}

void CSubsystemObject::blackboardWrite(const void *pvData, size_t size)
{
    _blackboard->writeBuffer(pvData, size, getOffset() + _accessedIndex);

    _accessedIndex += size;
}

// Configurable element retrieval
const CInstanceConfigurableElement *CSubsystemObject::getConfigurableElement() const
{
    return _pInstanceConfigurableElement;
}
// Belonging Subsystem retrieval
const CSubsystem *CSubsystemObject::getSubsystem() const
{
    return _pInstanceConfigurableElement->getBelongingSubsystem();
}

size_t CSubsystemObject::getOffset() const
{
    return _pInstanceConfigurableElement->getOffset();
}
