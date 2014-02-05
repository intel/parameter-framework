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
#pragma once

#include "ElementLibrary.h"
#include "ElementBuilder.h"

#include <map>
#include <string>

/** Factory that creates an element given an xml element. If no matching builder is found, it uses
  * the default builder.
  *
  * @tparam CDefaultElementBuilder is the class of the element builder to use if no corresponding
  *                                builder is found for a given xml element.
  */
template<class CDefaultElementBuilder>
class CDefaultElementLibrary: public CElementLibrary
{
public:

    explicit CDefaultElementLibrary(bool bEnableDefaultMechanism = true);
    virtual ~CDefaultElementLibrary() {}

    /** Enable the default builder fallback mechanism.
      * @see createElement() for more detail on this mechanism.
      *
      * @param[in] bEnable if true/false, activate/deactivate the default builder mechanism.
      */
    void enableDefaultMechanism(bool bEnable) {
        _bEnableDefaultMechanism = bEnable;
    }


    /** Create and return an element instanciated depending on an xmlElement.
      *
      * @param[in] xmlElement: The xml element used to find a matching builder
      *
      * @return If a matching builder is found, return an element created from the builder,
      *         otherwise:
      *             If the default mechanism is enable (@see enableDefaultMechanism),
      *                 create the elemen with the default element builder.
      *             otherwise, return NULL.
      */
    CElement* createElement(const CXmlElement& xmlElement) const;

private:
    bool _bEnableDefaultMechanism;
    CDefaultElementBuilder _DefaultElementBuilder;
};

template<class CDefaultElementBuilder>
CDefaultElementLibrary<CDefaultElementBuilder>::CDefaultElementLibrary(bool bEnableDefaultMechanism) :
        _bEnableDefaultMechanism(bEnableDefaultMechanism),
        _DefaultElementBuilder() {}

template<class CDefaultElementBuilder>
CElement* CDefaultElementLibrary<CDefaultElementBuilder>::createElement(const CXmlElement& xmlElement) const
{
    CElement* builtElement = CElementLibrary::createElement(xmlElement);

    if (builtElement != NULL) {
        // The element was created, return it
        return builtElement;
    }

    if (!_bEnableDefaultMechanism) {
        // The default builder mechanism is not enabled
        return NULL;
    }

    // Use the default builder
    return _DefaultElementBuilder.createElement(xmlElement);
}

