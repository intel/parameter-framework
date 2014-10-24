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

#include <map>
#include <string>

#include "Element.h"

class CElementBuilder;

class CElementLibrary
{
    typedef std::map<std::string, const CElementBuilder*> ElementBuilderMap;
    typedef ElementBuilderMap::iterator ElementBuilderMapIterator;
    typedef ElementBuilderMap::const_iterator ElementBuilderMapConstIterator;

public:
    CElementLibrary();
    virtual ~CElementLibrary();

    /** Add a xml tag and it's corresponding builder in the library.
       *
       * @param[in] xmlTag is the tag of an xml element that can be given to the builder to
       *               create a new element.
       * @param[in] pElementBuilder is the tag associated element builder.
       */
    void addElementBuilder(const std::string& type, const CElementBuilder *pElementBuilder);
    void clean();

    // Instantiation
    CElement* createElement(const CXmlElement& xmlElement) const;

private:
    // Builder type
    virtual std::string getBuilderType(const CXmlElement& xmlElement) const;

    // Builders
    ElementBuilderMap _elementBuilderMap;
};
