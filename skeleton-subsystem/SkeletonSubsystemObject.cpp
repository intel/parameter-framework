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
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include "ParameterType.h"
#include "MappingContext.h"
#include "SkeletonMappingKeys.h"
#include "InstanceConfigurableElement.h"
#include "SkeletonSubsystemObject.h"

#define STR_FORMAT_LENGTH 32

#define base CFormattedSubsystemObject

using std::string;

CSkeletonSubsystemObject::CSkeletonSubsystemObject(
            const string& strMappingValue,
            CInstanceConfigurableElement* pInstanceConfigurableElement,
            const CMappingContext& context
        ) :
        base(pInstanceConfigurableElement,
             strMappingValue,
             EAmend1,
             EAmendEnd - EAmend1 + 1,
             context),
        _bWrongElementTypeError(false)
{
    // Get actual element type
    const CParameterType* pParameterType = static_cast<const CParameterType*>(pInstanceConfigurableElement->getTypeElement());

    // Retrieve sizes
    _uiScalarSize = pParameterType->getSize();
    _uiArraySize = pInstanceConfigurableElement->getFootPrint() / _uiScalarSize;

    // Construct message
    _strMessage = context.getItem(ESkeletonOwner) + ":" + strMappingValue ;

    // Handle types
    // Check we are able to handle elements (no exception support, defer the error)
    switch(pInstanceConfigurableElement->getType()) {

        case CInstanceConfigurableElement::EParameter:
            break;
        default:
            _bWrongElementTypeError = true;
            break;
    }

}
// Sync to/from HW
bool CSkeletonSubsystemObject::accessHW(bool bReceive, string& strError)
{
    // Check parameter type is ok (deferred error, no exceptions available :-()
    if (_bWrongElementTypeError) {

        strError = "Unsupported parameter type";

        return false;
    }

    return base::accessHW(bReceive, strError);
}

bool CSkeletonSubsystemObject::sendToHW(string& strError)
{
    (void) strError;

    uint32_t uiIndex;

    void* pvValue = alloca(_uiScalarSize);

    for (uiIndex = 0 ; uiIndex < _uiArraySize ; uiIndex++) {

        // Read Value in BlackBoard
        blackboardRead(pvValue, _uiScalarSize);

        // Send here the value
        std::cout << "Sending to HW: " << _strMessage << std::endl;
    }

    return true;
}

bool CSkeletonSubsystemObject::receiveFromHW(string& strError)
{
    (void) strError;

    uint32_t uiIndex;

    void* pvValue = alloca(_uiScalarSize);

    for (uiIndex = 0 ; uiIndex < _uiArraySize ; uiIndex++) {

        // Retreive here the value
        std::cout << "Retreive from HW: " << _strMessage << std::endl;

        // Write Value in Blackboard
        blackboardWrite(pvValue, _uiScalarSize);
    }

    return true;
}
