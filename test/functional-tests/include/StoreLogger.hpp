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
#include <ParameterMgrFullConnector.h>
#include <vector>
#include <utility>
#include <string>
#include <algorithm>
#include <sstream>
#include <iterator>

namespace parameterFramework
{

/** Logger that stores all fed log in order retrieve them asynchronously.
 *  Compatible with the ParameterFramework::Ilogger api.
 *  Usually used in tests to inspect what was logged by a PF instances
 *  (eg: test if a warring occurred).
 */
class StoreLogger : public CParameterMgrFullConnector::ILogger
{
public:
    struct Log
    {
        enum class Level
        {
            info,
            warning
        };
        Level level;
        std::string msg;
        bool operator==(const Log &other) const
        {
            return level == other.level and msg == other.msg;
        }
    };
    using Logs = std::vector<Log>;

    void warning(const std::string &strLog) override
    {
        logs.push_back({Log::Level::warning, strLog});
    }
    void info(const std::string &strLog) override { logs.push_back({Log::Level::info, strLog}); }

    const Logs &getLogs() const { return logs; }

    Logs filter(Log::Level level) const
    {
        return filter([&level](const Log &log) { return log.level == level; });
    };

    Logs match(const std::string &pattern) const
    {
        return filter(
            [&pattern](const Log &log) { return log.msg.find(pattern) == std::string::npos; });
    }

private:
    template <class Predicate>
    Logs filter(Predicate predicate) const
    {
        Logs filtered;
        std::copy_if(logs.begin(), logs.end(), std::back_inserter(filtered), predicate);
        return filtered;
    }

    Logs logs;
};

/** Overload input stream operator to pretty print a StoreLogger::Log::Level. */
std::ostream &operator<<(std::ostream &os, const StoreLogger::Log::Level &level)
{
    auto levelStr = "UNREACHABLE";
    using L = StoreLogger::Log::Level;
    switch (level) {
    case L::info:
        levelStr = "Info";
        break;
    case L::warning:
        levelStr = "Warn";
        break;
    }
    return os << levelStr << ": ";
}

/** Overload input stream operator to pretty print a StoreLogger::Log. */
std::ostream &operator<<(std::ostream &os, const StoreLogger::Log &log)
{
    return os << log.level << log.msg << std::endl;
}

} // namespace parameterFramework
