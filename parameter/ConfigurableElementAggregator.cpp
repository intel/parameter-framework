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
#include "ConfigurableElementAggregator.h"
#include "ConfigurableElement.h"

CConfigurableElementAggregator::CConfigurableElementAggregator(std::list<const CConfigurableElement*>& aggregateList, MatchesAggregationCriterion pfnMatchesAggregationCriterion)
    : _aggregateList(aggregateList), _pfnMatchesAggregationCriterion(pfnMatchesAggregationCriterion)
{
}

// Aggregate
void CConfigurableElementAggregator::aggegate(const CConfigurableElement* pConfigurableElement)
{
    doAggregate(pConfigurableElement, _aggregateList);
}

// Recursive aggregate
bool CConfigurableElementAggregator::doAggregate(const CConfigurableElement* pConfigurableElement, std::list<const CConfigurableElement*>& aggregateList)
{
    if (!(pConfigurableElement->*_pfnMatchesAggregationCriterion)()) {

        // Not a candidate for aggregation
        return false;
    }
    // Check children
    std::list<const CConfigurableElement*> childAggregateElementList;

    size_t uiIndex;
    size_t uiNbChildren = pConfigurableElement->getNbChildren();
    size_t uiNbMatchingChildren = 0;

    for (uiIndex = 0; uiIndex < uiNbChildren; uiIndex++) {

        const CConfigurableElement* pChildConfigurableElement = static_cast<const CConfigurableElement*>(pConfigurableElement->getChild(uiIndex));

        uiNbMatchingChildren += doAggregate(pChildConfigurableElement, childAggregateElementList);
    }

    if (uiNbMatchingChildren == uiNbChildren) {

        // All children match => self is a match
        aggregateList.push_back(pConfigurableElement);

        return true;
    } else {
        // Add children if any
        aggregateList.insert(aggregateList.end(), childAggregateElementList.begin(), childAggregateElementList.end());

        return false;
    }
}
