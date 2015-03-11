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
* @Date:   2015-03-11 14:21:28
* @Last Modified by:   wangxiaobo
* @Last Modified time: 2015-03-11 18:09:23
*/

#ifndef PB_LOG_LINE_LOGGER_H_
#define PB_LOG_LINE_LOGGER_H_

#include <type_traits>
#include "common_def.h"
#include "logger.h"

namespace pb
{
// line logger class - aggregates operator << calls to fast ostream
// and logs upon destruction
class LineLogger
{
public:
    LineLogger(Logger* callback_logger, LevelEnum msg_level, bool enabled) :
        callback_logger_(callback_logger),
        log_msg_(msg_level),
        enabled_(enabled) {}

    // no copy intended. only move
    LineLogger(const LineLogger& other) = delete;
    LineLogger& operator=(const LineLogger&) = delete;
    LineLogger& operator=(LineLogger&&) = delete;

    LineLogger(LineLogger&& other) :
        callback_logger_(other.callback_logger_),
        log_msg_(std::move(other.log_msg_)),
        enabled_(other.enabled_)
    {
        other.Disable();
    }

    // log the message using the callback logger
    virtual ~LineLogger()
    {
        if (enabled_)
        {
            log_msg_.logger_name = callback_logger_->name();
            log_msg_.time = os::now();
            callback_logger_->_LogMsg(log_msg_);
        }
    }

    // support for format string with variadic arguments
    void write(const char* what)
    {
        if (enabled_)
        {
            log_msg_.raw << what;
        }
    }

    template <typename... Args>
    void write(const char* fmt, const Args&... args)
    {
        if (!enabled_)
        {
            return;
        }
        try
        {
            log_msg_.raw.write(fmt, args...);
        }
        catch (const fmt::FormatError& e)
        {
            throw SimpleException(
                fmt::format("formatting error while processing format string '{}': {}",
                fmt, what()));
        }
    }

    // support for operator <<
    LineLogger& operator <<(const char *what)
    {
        if (enabled_)
        {
            log_msg_.raw << what;
        }
        return *this;
    }

    LineLogger& operator <<(const std::string& what)
    {
        if (enabled_)
        {
            log_msg_.raw << what;
        }
        return *this;
    }

    LineLogger& operator <<(int what)
    {
        if (enabled_)
        {
            log_msg_.raw << what;
        }
        return *this;
    }

    LineLogger& operator <<(unsigned int what)
    {
        if (enabled_)
        {
            log_msg_.raw << what;
        }
        return *this;
    }

    LineLogger& operator <<(long what)
    {
        if (enabled_)
        {
            log_msg_.raw << what;
        }
        return *this;
    }

    LineLogger& operator <<(unsigned long what)
    {
        if (enabled_)
        {
            log_msg_.raw << what;
        }
        return *this;
    }

    LineLogger& operator <<(long long what)
    {
        if (enabled_)
        {
            log_msg_.raw << what;
        }
        return *this;
    }

    LineLogger& operator <<(unsigned long long what)
    {
        if (enabled_)
        {
            log_msg_.raw << what;
        }
        return *this;
    }

    LineLogger& operator <<(double what)
    {
        if (enabled_)
        {
            log_msg_.raw << what;
        }
        return *this;
    }

    LineLogger& operator <<(long double what)
    {
        if (enabled_)
        {
            log_msg_.raw << what;
        }
        return *this;
    }

    LineLogger& operator <<(float what)
    {
        if (enabled_)
        {
            log_msg_.raw << what;
        }
        return *this;
    }

    LineLogger& operator <<(char what)
    {
        if (enabled_)
        {
            log_msg_.raw << what;
        }
        return *this;
    }

    // support user types which implements operator <<
    template<typename T>
    LineLogger& operator <<(const T& what)
    {
        if (enabled_)
        {
            log_msg_.raw.write("{}", what);
        }
        return *this;
    }

    void Disable()
    {
        enabled_ = false;
    }
private:
    Logger* callback_logger_;
    LogMessage log_msg_;
    bool enabled_;
};
} // ns pb
#endif // PB_LOG_LINE_LOGGER_H_
