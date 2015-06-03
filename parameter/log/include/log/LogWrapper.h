/*
 * Copyright (c) 2015, Intel Corporation
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

#include "log/ILogger.h"
#include "Utility.h"
#include <string>
#include <sstream>
#include <iterator>
#include <list>

namespace core
{
namespace log
{
namespace details
{

/**
 * Template log wrapper
 * Simulate a stream which can be used instead of basic ILogger API.
 *
 * @tparam isWarning indicates which log canal to use
 */
template<bool isWarning>
class LogWrapper
{
public:

    /** @param logger the ILogger to wrap */
    LogWrapper(ILogger& logger, const std::string& prolog = "")
        : mLogger(logger), mProlog(prolog)
    {}

    /**
     * Class copy constructor
     *
     * @param[in] logWrapper the instance to copy
     */
    LogWrapper(const LogWrapper& logWrapper)
        : mLogger(logWrapper.mLogger), mProlog(logWrapper.mProlog)
    {}

    /** Class destructor */
    ~LogWrapper()
    {
        for (auto& log : mLogs) {
            (mLogger.*mLogCallback)(mProlog + log.str());
        }
    }

    /**
     * Simulate stream behaviour
     *
     * Both operator<<() versions must not be used in conjunction: either use
     * the 'scalar' version (this one) or the 'list' version but not both on
     * the same object.
     *
     * @tparam T the type of the information to log
     * @param[in] log the information to log
     */
    template <class T>
    LogWrapper& operator<<(const T &log)
    {
        if (mLogs.empty()) {
            mLogs.emplace_back();
        }

        mLogs.back() << log;
        return *this;
    }

    /**
     * Simulate stream behaviour for string list
     *
     * Both operator<<() versions must not be used in conjunction: either use
     * the 'scalar' version or the 'list' version (this one) but not both on
     * the same object.
     *
     * @param[in] logs list of information to log
     */
    LogWrapper& operator<<(const std::list<std::string>& logs)
    {
        for (auto& log : logs) {
            mLogs.emplace_back(log);
        }
        return *this;
    }

private:
    LogWrapper& operator=(const LogWrapper&);

    /** Log stream holder */
    std::list<std::ostringstream> mLogs;

    /** Wrapped logger */
    ILogger& mLogger;
    static constexpr const auto mLogCallback = isWarning ? &ILogger::warning : &ILogger::info;

    /** Log Prefix */
    const std::string& mProlog;
};

/** Default information logger type */
typedef details::LogWrapper<false> Info;

/** Default warning logger type */
typedef details::LogWrapper<true> Warning;

} /** details namespace */
} /** log namespace */
} /** core namespace */
