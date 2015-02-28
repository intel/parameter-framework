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

#include "Utility.h"

#include <sstream>
#include <iterator>
#include <stdint.h>

using std::string;

// Format string list
void CUtility::asString(const std::list<std::string>& lstr,
                        std::string& strOutput,
                        const std::string& strSeparator)
{
    std::ostringstream ostrFormatedList;

    std::copy(lstr.begin(), lstr.end(),
              std::ostream_iterator<std::string>(ostrFormatedList, strSeparator.c_str()));

    strOutput = ostrFormatedList.str();

    // Remove last separator
    if (strOutput.size() > strSeparator.size()) {

        strOutput.erase(strOutput.size() - strSeparator.size());
    }
}

// Format string map
void CUtility::asString(const std::map<std::string, std::string>& mapStr,
                        std::string& strOutput,
                        const std::string& strItemSeparator,
                        const std::string& strKeyValueSeparator)
{
    std::list<std::string> listKeysValues;

    std::map<std::string, std::string>::const_iterator iter;

    for(iter = mapStr.begin(); iter != mapStr.end(); ++iter) {

        listKeysValues.push_back(iter->first + strKeyValueSeparator + iter->second);
    }

    CUtility::asString(listKeysValues, strOutput, strItemSeparator);
}

void CUtility::appendTitle(string& strTo, const string& strTitle)
{
    strTo += "\n" + strTitle + "\n";

    uint32_t uiLength = strTitle.size();

    while (uiLength--) {

        strTo += "=";
    }
    strTo += "\n";
}
