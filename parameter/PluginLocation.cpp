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
#include "PluginLocation.h"

#define base CKindElement

CPluginLocation::CPluginLocation(const std::string& strName, const std::string& strKind) : base(strName, strKind)
{

}

const std::string& CPluginLocation::getFolder() const
{
    return _strFolder;
}

const std::list<std::string>& CPluginLocation::getPluginList() const
{
    return _pluginList;
}

bool CPluginLocation::fromXml(const CXmlElement &xmlElement, CXmlSerializingContext &serializingContext)
{
    (void) serializingContext;

    // Retrieve folder
    _strFolder = xmlElement.getAttributeString("Folder");

    // Get Info from children
    CXmlElement::CChildIterator childIterator(xmlElement);

    CXmlElement xmlPluginElement;

    while (childIterator.next(xmlPluginElement)) {

        // Fill Plugin List
        _pluginList.push_back(xmlPluginElement.getAttributeString("Name"));
    }

    // Don't dig
    return true;
}
