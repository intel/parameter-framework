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

#include "XmlSource.h"
#include "SelectionCriterion.h"
#include <log/Logger.h>

#include <string>
#include <list>
#include <map>
#include <memory>

/** Criteria Handler */
class CSelectionCriteria : public IXmlSource
{
public:
    CSelectionCriteria();

    /** Create a new Exclusive criterion
     *
     * @param[in] name, the criterion name
     * @return raw pointer on the created criterion
     */
    CSelectionCriterion* createExclusiveCriterion(const std::string& name,
                                                  core::log::Logger& logger);

    /** Create a new Inclusive criterion
     *
     * @param[in] name, the criterion name
     * @return raw pointer on the created criterion
     */
    CSelectionCriterion* createInclusiveCriterion(const std::string& name,
                                                  core::log::Logger& logger);

    /** Criterion Retrieval
     *
     * @param[in] name, the criterion name
     * @result pointer to the desired criterion object
     */
    CSelectionCriterion* getSelectionCriterion(const std::string& name);

    /** Const Criterion Retrieval
     *
     * @param[in] name, the criterion name
     * @result pointer to the desired const criterion object
     */
    const CSelectionCriterion* getSelectionCriterion(const std::string& name) const;

    // List available criteria
    void listSelectionCriteria(std::list<std::string>& strResult, bool bWithTypeInfo, bool bHumanReadable) const;

    // Reset the modified status of the children
    void resetModifiedStatus();

    /** Xml Serialization method
     *
     * @param[out] xmlElement the current xml element to fill with data
     * @param serializingContext context of the current serialization
     */
    virtual void toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const;

private:

    /** Internally wrap criterion pointer to not have to handle destruction */
    typedef std::unique_ptr<CSelectionCriterion> CriterionWrapper;

    /** Criteria instance container type, map which use criterion name as key */
    typedef std::map<std::string, CriterionWrapper> Criteria;

    /** Confine exception use to smooth code transitions
     * Android is not supporting exceptions by default. Nevertheless some
     * solutions has emerged.
     * This function allows to confine exceptions use in only one function.
     * It also allows to easily connect this part of code to the parameter-framework
     * which works without exceptions for now.
     *
     * @param[in] name, name of the criterion to retrieve
     * @return pointer to the desired criterion
     */
    CSelectionCriterion* getCriterionPointer(const std::string& name) const;

    /** Criteria instance container */
    Criteria mCriteria;
};
