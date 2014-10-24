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
#include "FrameworkConfigurationLocation.h"
#include <assert.h>

#define base CKindElement

CFrameworkConfigurationLocation::CFrameworkConfigurationLocation(const std::string& strName, const std::string& strKind) : base(strName, strKind)
{
}

// From IXmlSink
bool CFrameworkConfigurationLocation::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    _strPath = xmlElement.getAttributeString("Path");

    if (_strPath.empty()) {

        serializingContext.setError("Empty Path attribute in element " + xmlElement.getPath());

        return false;
    }
    return true;
}

// File path
std::string CFrameworkConfigurationLocation::getFilePath(const std::string& strBaseFolder) const
{
    if (isPathRelative()) {

        return strBaseFolder + "/" + _strPath;
    }
    return _strPath;
}

// Folder path
std::string CFrameworkConfigurationLocation::getFolderPath(const std::string& strBaseFolder) const
{
    uint32_t uiSlashPos = _strPath.rfind('/', -1);

    if (isPathRelative()) {

        if (uiSlashPos != (uint32_t)-1) {

            return strBaseFolder + "/" + _strPath.substr(0, uiSlashPos);

        } else {

            return strBaseFolder;
        }
    } else {

        assert(uiSlashPos != (uint32_t)-1);

        return _strPath.substr(0, uiSlashPos);
    }
}

// Detect relative path
bool CFrameworkConfigurationLocation::isPathRelative() const
{
    return _strPath[0] != '/';
}
