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

#include <string>
#include <list>
#include <map>
#include <sstream>

class CUtility
{
public:
    /**
    * Format the items of a map into a string as a list of key-value pairs. The map must be
    * composed of pairs of strings.
    *
    * @param[in] mapStr A map of strings
    * @param[out] strOutput The output string
    * @param[in] separator The separator to use between each item
    */
    static void asString(const std::list<std::string>& lstr,
                         std::string& strOutput,
                         const std::string& separator = "\n");

    /**
     * Format the items of a map into a string as a list of key-value pairs. The map must be
     * composed of pairs of strings.
     *
     * @param[in] mapStr A map of strings
     * @param[out] strOutput The output string
     * @param[in] strItemSeparator The separator to use between each item (key-value pair)
     * @param[in] strKeyValueSeparator The separator to use between key and value
     */
    static void asString(const std::map<std::string, std::string>& mapStr,
                         std::string& strOutput,
                         const std::string& strItemSeparator = ", ",
                         const std::string& strKeyValueSeparator = ":");

    /** Utility to easily convert a builtin type into string
     *
     * FIXME: Should be replaced by std::to_string after C++11 introduction
     */
    template <class T>
    static std::string toString(T uiValue)
    {
        std::ostringstream ostr;

        ostr << uiValue;

        return ostr.str();
    }

    /** Utility to underline */
    static void appendTitle(std::string& strTo, const std::string& strTitle);
};
