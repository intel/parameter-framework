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
#pragma once

#include "parameter_export.h"

#include "Element.h"
#include <string>

class CMappingData;
class CInstanceConfigurableElement;

class PARAMETER_EXPORT CTypeElement : public CElement
{
public:
    CTypeElement(const std::string &strName = "");
    ~CTypeElement() override;

    // Instantiation
    CInstanceConfigurableElement *instantiate() const;

    // Mapping info
    virtual bool getMappingData(const std::string &strKey, const std::string *&pStrValue) const;
    virtual bool hasMappingData() const;

    /**
     * Returns the mapping associated to the current TypeElement instance
     *
     * @return A std::string containing the mapping as a comma separated key value pairs
     */
    virtual std::string getFormattedMapping() const;

    // Element properties
    void showProperties(std::string &strResult) const override;

    // From IXmlSink
    bool fromXml(const CXmlElement &xmlElement,
                 CXmlSerializingContext &serializingContext) override;

    // From IXmlSource
    void toXml(CXmlElement &xmlElement, CXmlSerializingContext &serializingContext) const override;

    // Scalar or Array?
    bool isScalar() const;

    // Array Length
    size_t getArrayLength() const;

    /**
     * Converts size optimized integer input data (int8, int16, int32) to plain int
     *
     * @param[in] iSizeOptimizedData the data to convert
     *
     * @return the data with int type
     */
    virtual int toPlainInteger(int iSizeOptimizedData) const;

protected:
    // Object creation
    virtual void populate(CElement *pElement) const;
    /** @Returns the mapping associated to the current type and its predecessor
     *
     * The meaning of predecessor depends on the TypeElement type: e.g. for a
     * component instance, the predecessor is the ComponentType; for a
     * ComponentType, the predecessor is its base type.
     *
     * The predecessor's mapping comes first, then the current type's mapping.
     *
     * @param[in] predecessor A pointer to the predecessor or NULL.
     */
    std::string getFormattedMapping(const CTypeElement *predecessor) const;

private:
    CTypeElement(const CTypeElement &);
    CTypeElement &operator=(const CTypeElement &);
    // Actual instance creation
    virtual CInstanceConfigurableElement *doInstantiate() const = 0;

    // Mapping data creation and access
    CMappingData *getMappingData();

    // For Arrays. 0 means scalar
    size_t _arrayLength{0};

    // Mapping info
    CMappingData *_pMappingData{nullptr};
};
