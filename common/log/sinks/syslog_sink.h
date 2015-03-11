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
* @Date:   2015-03-10 14:49:08
* @Last Modified by:   wangxiaobo
* @Last Modified time: 2015-03-10 15:24:57
*/

#ifndef PB_LOG_SINKS_SYSLOG_SINK_H_
#define PB_LOG_SINKS_SYSLOG_SINK_H_

#ifdef __linux__

#include <array>
#include <string>
#include <syslog.h>

#include <common/log/common_def.h>
#include <common/log/log_message.h>
#include "sink.h"

namespace pb
{
// sink that write to syslog using syscall() library call.
// locking is not needed, as syslog() itself is thread-safe.
class SyslogSink : public sink
{
public:
    SyslogSink(const std::string& ident = "", int syslog_option = 0,
               int syslog_facility = LOG_USER) : ident_(ident)
    {
        priorities_[static_cast<int>(kTrace)] = LOG_DEBUG;
        priorities_[static_cast<int>(kDebug)] = LOG_DEBUG;
        priorities_[static_cast<int>(kInfo)] = LOG_INFO;
        priorities_[static_cast<int>(kNotice)] = LOG_NOTICE;
        priorities_[static_cast<int>(kWarning)] = LOG_WARNING;
        priorities_[static_cast<int>(kError)] = LOG_ERR;
        priorities_[static_cast<int>(kCritical)] = LOG_CRIT;
        priorities_[static_cast<int>(kOff)] = LOG_INFO;

        // set ident to be program name if empty
        ::openlog(ident_.empty() ? nullptr : ident_.c_str(),
                   syslog_option, syslog_facility);
    }

    virtual ~SyslogSink() { :: closelog(); }

    SyslogSink(const SyslogSink&) = delete;
    SyslogSink& operator=(const SyslogSink&) = delete;

    void Log(const LogMessage& msg) override
    {
        ::syslog(SyslogPriorityFromLevel(msg), "%s",
                 msg.formatted.str().c_str());
    }
private:
    std::array<int, 10> priorities_;
    // must store the ident because manual says openlog might use the pointer
    // as is and not a string copy
    const std::string ident_;

    /**
     * @brief simply maps our log level to syslog priority level.
     * 
     * @param msg [in]
     * @return [sys log priority]
     */
    int SyslogPriorityFromLevel(const LogMessage& msg) const
    {
        return priorities_[static_cast<int>(msg.level)];
    }
};
}

#endif // __linux__

#endif // PB_LOG_SINKS_SYSLOG_SINK_H_