// Copyright (C) 2015  wangxiaobo

// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.

// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.

// You should have received a copy of the GNU General Public License along
// with this program.  If not, see <http://www.gnu.org/licenses/>.

/*
* @Author: wangxiaobo
* @Date:   2015-02-12 17:51:05
* @Last Modified by:   wangxiaobo
* @Last Modified time: 2015-02-12 18:22:53
*/

#ifndef PB_LOG_MESSAGE_H_
#define PB_LOG_MESSAGE_H_

#include <common/util/format.h>
#include "common_def.h"

namespace pb
{
struct LogMessage
{
    LogMessage() = default;
    LogMessage(LevelEnum l):
        logger_name(),
        level(l),
        time(),
        raw(),
        formatted() {}

    LogMessage(const LogMessage& other):
        logger_name(other.logger_name),
        level(other.level),
        time(other.time)
    {
        if (other.raw.size())
        {
            raw << fmt::BasicStringRef<char>(other.raw.data(),
                                             other.raw.size());
        }
        if (other.formatted.size())
        {
            formatted << fmt::BasicStringRef<char>(other.formatted.data(),
                                                   other.formatted.size());
        }
    }

    LogMessage(LogMessage&& other):
        logger_name(std::move(other.logger_name)),
        level(other.level),
        time(std::move(other.time)),
        raw(std::move(other.raw)),
        formatted(std::move(other.formatted))
    {
        other.clear();
    }

    LogMessage& operator = (LogMessage&& other)
    {
        if (this == &other)
        {
            return *this;
        }
        logger_name = std::move(other.logger_name);
        level = other.level;
        time = std::move(other.time);
        raw = std::move(other.raw);
        formatted = std::move(other.formatted);
        other.clear();
        return *this;
    }

    void clear()
    {
        level = LevelEnum::kOff;
        raw.clear();
        formatted.clear();
    }

    std::string logger_name;
    LevelEnum level;
    log_clock::time_point time;
    fmt::MemoryWriter raw; // raw string
    fmt::MemoryWriter formatted; // formatted string
};
} // ns pb
#endif // PB_LOG_MESSAGE_H_
