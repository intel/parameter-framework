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
#include <fstream>
#include <alloca.h>
#include "ParameterType.h"
#include "MappingContext.h"
#include "TESTMappingKeys.h"
#include "InstanceConfigurableElement.h"
#include "TESTSubsystemObject.h"

#define base CSubsystemObject

CTESTSubsystemObject::CTESTSubsystemObject(const std::string& strMappingValue, CInstanceConfigurableElement* pInstanceConfigurableElement, const CMappingContext& context)
    : base(pInstanceConfigurableElement)
{
    (void)strMappingValue;
    // Get actual element type
    const CParameterType* pParameterType = static_cast<const CParameterType*>(pInstanceConfigurableElement->getTypeElement());

    _uiScalarSize = pParameterType->getSize();
    _uiArraySize = pInstanceConfigurableElement->getFootPrint() / _uiScalarSize;
    _bIsScalar = pParameterType->isScalar();

    _strFilePath = context.getItem(ETESTDirectory) + "/" + pInstanceConfigurableElement->getName();
    _bLog = context.iSet(ETESTLog) && (context.getItem(ETESTLog) == "yes");
}

bool CTESTSubsystemObject::sendToHW(std::string& strError)
{
    std::ofstream outputFile;

    outputFile.open(_strFilePath.c_str());

    if (!outputFile.is_open()) {

        strError = "Unable to open file: " + _strFilePath;

        return false;
    }

    sendToFile(outputFile);

    outputFile.close();

    return true;
}


bool CTESTSubsystemObject::receiveFromHW(std::string& strError)
{
    (void)strError;
    std::ifstream inputFile;

    inputFile.open(_strFilePath.c_str());

    if (!inputFile.is_open()) {

        return true;
    }

    receiveFromFile(inputFile);

    inputFile.close();
    return true;
}

void CTESTSubsystemObject::sendToFile(std::ofstream& outputFile)
{
    uint32_t uiIndex;

    for (uiIndex = 0 ; uiIndex < _uiArraySize ; uiIndex++) {

        void* pvValue = alloca(_uiScalarSize);

        // Read Value in BlackBoard
        blackboardRead(pvValue, _uiScalarSize);

        std::string strValue = toString(pvValue, _uiScalarSize);

        outputFile << strValue << std::endl;

        if (_bLog) {

            if (_bIsScalar) {

                log_info("TESTSUBSYSTEM: Writing \"%s\" to file %s", strValue.c_str(), _strFilePath.c_str());
            } else {

                log_info("TESTSUBSYSTEM: Writing \"%s\" to file %s[%d]", strValue.c_str(), _strFilePath.c_str(), uiIndex);
            }
        }
    }
}

void CTESTSubsystemObject::receiveFromFile(std::ifstream& inputFile)
{
    uint32_t uiIndex;

    for (uiIndex = 0 ; uiIndex < _uiArraySize ; uiIndex++) {

        void* pvValue = alloca(_uiScalarSize);

        std::string strValue;

        inputFile >> strValue;

        if (_bLog) {

            if (_bIsScalar) {

                log_info("TESTSUBSYSTEM: Writing \"%s\" from file %s", strValue.c_str(), _strFilePath.c_str());
            } else {

                log_info("TESTSUBSYSTEM: Writing \"%s\" from file %s[%d]", strValue.c_str(), _strFilePath.c_str(), uiIndex);
            }
        }

        fromString(strValue, pvValue, _uiScalarSize);

        // Write Value in Blackboard
        blackboardWrite(pvValue, _uiScalarSize);
    }
}
