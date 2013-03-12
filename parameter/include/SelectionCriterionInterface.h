/*  SelectionCriterionInterface.h
 **
 ** Copyright © 2011 Intel
 **
 ** Licensed under the Apache License, Version 2.0 (the "License");
 ** you may not use this file except in compliance with the License.
 ** You may obtain a copy of the License at
 **
 **     http://www.apache.org/licenses/LICENSE-2.0
 **
 ** Unless required by applicable law or agreed to in writing, software
 ** distributed under the License is distributed on an "AS IS" BASIS,
 ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 ** See the License for the specific language governing permissions and
 ** limitations under the License.
 **
 ** AUTHOR: Patrick Benavoli (patrickx.benavoli@intel.com)
 ** CREATED: 2011-06-01
 ** UPDATED: 2011-07-27
 **
 */
#pragma once

#include <string>

#include "SelectionCriterionTypeInterface.h"

class ISelectionCriterionInterface
{
public:
    virtual void setCriterionState(int iState) = 0;
    virtual int getCriterionState() const = 0;
    virtual std::string getCriterionName() const = 0;
    virtual const ISelectionCriterionTypeInterface* getCriterionType() const = 0;

protected:
    virtual ~ISelectionCriterionInterface() {}
};
