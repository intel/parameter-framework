/*
* Copyright (c) 2011-2015, Intel Corporation
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
#include <fstream>
#include <assert.h>
#include "TESTSubsystem.h"
#include "TESTSubsystemBinary.h"
#include "TESTSubsystemString.h"
#include "TESTMappingKeys.h"
#include "SubsystemObjectFactory.h"
#include <stdlib.h>
#include <stdio.h>

#define base CSubsystem

// Directory for isAlive and NeedResync files
const char* gacFwNamePropName = getenv("PFW_RESULT");

// Implementation
CTESTSubsystem::CTESTSubsystem(const std::string& strName) : base(strName)
{
    // Provide mapping keys to upper layer
    addContextMappingKey("Directory");
    addContextMappingKey("Log");

    // Provide creators to upper layer
    addSubsystemObjectFactory(new TSubsystemObjectFactory<CTESTSubsystemBinary>("Binary", 1 << ETESTDirectory));
    addSubsystemObjectFactory(new TSubsystemObjectFactory<CTESTSubsystemString>("String", 1 << ETESTDirectory));
}

// Susbsystem sanity health
bool CTESTSubsystem::isAlive() const
{
    assert(gacFwNamePropName != NULL);
    return read(std::string(gacFwNamePropName) + "/isAlive") == "true";
}

// Resynchronization after subsystem restart needed
bool CTESTSubsystem::needResync(bool bClear)
{
    assert(gacFwNamePropName != NULL);
    std::string strNeedResyncFile = std::string(gacFwNamePropName) + "/needResync";
    bool bNeedResync;

    bNeedResync = read(strNeedResyncFile) == "true";

    if (!bNeedResync) {

        // subsystem does not need resync
        return false;
    } else {
        // subsystem needs resync
        // If indicated, clear need resync state
        if (bClear) {

            write(strNeedResyncFile, "false");
        }

        return true;
    }
}

// Read boolean from file
std::string CTESTSubsystem::read(const std::string& strFileName)
{
    std::ifstream file;
    std::string strContent;

    file.open(strFileName.c_str());

    file >> strContent;

    return strContent;
}

// Write boolean to file
void CTESTSubsystem::write(const std::string& strFileName, const std::string& strContent)
{
    std::ofstream file;

    file.open(strFileName.c_str());

    assert(file.is_open());

    file << strContent;
}
