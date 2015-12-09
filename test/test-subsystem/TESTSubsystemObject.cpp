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
#include "ParameterType.h"
#include "MappingContext.h"
#include "TESTMappingKeys.h"
#include "InstanceConfigurableElement.h"
#include "TESTSubsystemObject.h"
#include <log/Context.h>
#include <sstream>
#include <vector>

#define base CSubsystemObject

CTESTSubsystemObject::CTESTSubsystemObject(
    const std::string & /*strMappingValue*/,
    CInstanceConfigurableElement *pInstanceConfigurableElement, const CMappingContext &context,
    core::log::Logger &logger)
    : base(pInstanceConfigurableElement, logger)
{
    // Get actual element type
    const CParameterType *pParameterType =
        static_cast<const CParameterType *>(pInstanceConfigurableElement->getTypeElement());

    _scalarSize = pParameterType->getSize();
    _arraySize = pInstanceConfigurableElement->getFootPrint() / _scalarSize;
    _bIsScalar = pParameterType->isScalar();

    _strFilePath = context.getItem(ETESTDirectory) + "/" + pInstanceConfigurableElement->getName();
    _bLog = context.iSet(ETESTLog) && (context.getItem(ETESTLog) == "yes");
}

bool CTESTSubsystemObject::sendToHW(std::string &strError)
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

bool CTESTSubsystemObject::receiveFromHW(std::string & /*strError*/)
{
    std::ifstream inputFile;

    inputFile.open(_strFilePath.c_str());

    if (!inputFile.is_open()) {

        return true;
    }

    receiveFromFile(inputFile);

    inputFile.close();
    return true;
}

void CTESTSubsystemObject::sendToFile(std::ofstream &outputFile)
{
    for (size_t index = 0; index < _arraySize; index++) {

        std::vector<uint8_t> aucValue(_scalarSize);

        void *pvValue = aucValue.data();

        // Read Value in BlackBoard
        blackboardRead(pvValue, _scalarSize);

        std::string strValue = toString(pvValue, _scalarSize);

        outputFile << strValue << std::endl;

        if (_bLog) {

            if (_bIsScalar) {

                info() << "TESTSUBSYSTEM: Writing '" << strValue << "' to file " << _strFilePath;
            } else {

                info() << "TESTSUBSYSTEM: Writing '" << strValue << "' to file " << _strFilePath
                       << "[" << index << "]";
            }
        }
    }
}

void CTESTSubsystemObject::receiveFromFile(std::ifstream &inputFile)
{
    for (size_t index = 0; index < _arraySize; index++) {

        std::vector<uint8_t> aucValue(_scalarSize);

        void *pvValue = aucValue.data();

        std::string strValue;

        inputFile >> strValue;

        if (_bLog) {

            if (_bIsScalar) {

                info() << "TESTSUBSYSTEM: Reading '" << strValue << "' to file " << _strFilePath;
            } else {

                info() << "TESTSUBSYSTEM: Reading '" << strValue << "' to file " << _strFilePath
                       << "[" << index << "]";
            }
        }

        fromString(strValue, pvValue, _scalarSize);

        // Write Value in Blackboard
        blackboardWrite(pvValue, _scalarSize);
    }
}
