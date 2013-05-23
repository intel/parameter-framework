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
 * CREATED: 2012-12-17
 */

#include "Utility.h"

#include <sstream>
#include <iterator>

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

