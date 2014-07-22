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
#include "SelectionCriteriaDefinition.h"
#include "SelectionCriterion.h"

CSelectionCriteriaDefinition::CSelectionCriteriaDefinition()
{
}

std::string CSelectionCriteriaDefinition::getKind() const
{
    return "SelectionCriteriaDefinition";
}

// Selection Criterion creation
CSelectionCriterion* CSelectionCriteriaDefinition::createSelectionCriterion(const std::string& strName, const CSelectionCriterionType* pSelectionCriterionType)
{
    CSelectionCriterion* pSelectionCriterion = new CSelectionCriterion(strName, pSelectionCriterionType);

    addChild(pSelectionCriterion);

    return pSelectionCriterion;
}

// Selection Criterion access
const CSelectionCriterion* CSelectionCriteriaDefinition::getSelectionCriterion(const std::string& strName) const
{
    return static_cast<const CSelectionCriterion*>(findChild(strName));
}

CSelectionCriterion* CSelectionCriteriaDefinition::getSelectionCriterion(const std::string& strName)
{
    return static_cast<CSelectionCriterion*>(findChild(strName));
}

// List available criteria
void CSelectionCriteriaDefinition::listSelectionCriteria(std::list<std::string>& lstrResult, bool bWithTypeInfo, bool bHumanReadable) const
{
    // Propagate
    size_t uiNbChildren = getNbChildren();
    size_t uiChild;

    for (uiChild = 0; uiChild < uiNbChildren; uiChild++) {

        const CSelectionCriterion* pSelectionCriterion = static_cast<const CSelectionCriterion*>(getChild(uiChild));

        lstrResult.push_back(pSelectionCriterion->getFormattedDescription(bWithTypeInfo, bHumanReadable));
    }
}

// Reset the modified status of the children
void CSelectionCriteriaDefinition::resetModifiedStatus()
{
    // Propagate
    size_t uiNbChildren = getNbChildren();
    size_t uiChild;
    CSelectionCriterion* pSelectionCriterion;

    for (uiChild = 0; uiChild < uiNbChildren; uiChild++) {

        pSelectionCriterion = static_cast<CSelectionCriterion*>(getChild(uiChild));

        pSelectionCriterion->resetModifiedStatus();
    }
}
