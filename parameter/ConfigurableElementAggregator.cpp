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
#include "ConfigurableElementAggregator.h"
#include "ConfigurableElement.h"

CConfigurableElementAggregator::CConfigurableElementAggregator(list<const CConfigurableElement*>& aggregateList, MatchesAggregationCriterion pfnMatchesAggregationCriterion)
    : _aggregateList(aggregateList), _pfnMatchesAggregationCriterion(pfnMatchesAggregationCriterion)
{
}

// Aggregate
void CConfigurableElementAggregator::aggegate(const CConfigurableElement* pConfigurableElement)
{
    doAggregate(pConfigurableElement, _aggregateList);
}

// Recursive aggregate
bool CConfigurableElementAggregator::doAggregate(const CConfigurableElement* pConfigurableElement, list<const CConfigurableElement*>& aggregateList)
{
    if (!(pConfigurableElement->*_pfnMatchesAggregationCriterion)()) {

        // Not a candidate for aggregation
        return false;
    }
    // Check children
    list<const CConfigurableElement*> childAggregateElementList;

    uint32_t uiIndex;
    uint32_t uiNbChildren = pConfigurableElement->getNbChildren();
    uint32_t uiNbMatchingChildren = 0;

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
