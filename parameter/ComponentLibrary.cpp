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
#include "ComponentLibrary.h"
#include "ComponentType.h"
#include <assert.h>

CComponentLibrary::CComponentLibrary()
{
}

bool CComponentLibrary::childrenAreDynamic() const
{
    return true;
}

std::string CComponentLibrary::getKind() const
{
    return "ComponentLibrary";
}

const CComponentType* CComponentLibrary::getComponentType(const std::string& strName) const
{
    return static_cast<const CComponentType*>(findChild(strName));
}

bool CComponentLibrary::fromXml(const CXmlElement& xmlElement,
                                CXmlSerializingContext& serializingContext)
{
    CXmlElement childElement;

    CXmlElement::CChildIterator it(xmlElement);

    // XML populate all component libraries
    while (it.next(childElement)) {

        // Filter component library/type set elements
        if (childElement.getType() == "ComponentLibrary" ||
            childElement.getType() == "ComponentTypeSet") {

            if (!fromXml(childElement, serializingContext)) {

                return false;
            }
        } else {
            // Regular child creation and populating
            CElement* pChild = createChild(childElement, serializingContext);

            if (!pChild || !pChild->fromXml(childElement, serializingContext)) {

                return false;
            }
        }
    }

    return true;
}

