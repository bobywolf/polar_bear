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
* @Date:   2015-02-10 18:01:29
* @Last Modified by:   wangxiaobo
* @Last Modified time: 2015-02-12 17:26:26
*/

#ifndef PB_LOG_COMMON_DEF_H_
#define PB_LOG_COMMON_DEF_H_

#include <string>
#include <initializer_list>
#include <chrono>
#include <memory>

// under linux, you can use the much faster CLOCK_REALTIME_COARSE clock.
// this clock is less accurate - can be off by few millis - depending on the
// kernel HZ uncomment to use it instead of the regular (and slower) clock

//#ifdef __linux__
//#define PB_LOG_CLOCK_COARSE
//#endif

namespace pb
{
// forward declarations
class formatter;
namespace sinks
{
class sink;
} // ns sinks

// common types across the library
using log_clock = std::chrono::system_clock;
using sink_ptr = std::shared_ptr<sinks::sink>;
using sinks_init_list = std::initializer_list<sink_ptr>;
using formatter_ptr = std::shared_ptr<formatter>;

// log level enumeration
enum LevelEnum
{
    kTrace = 0,
    kDebug = 1,
    kInfo = 2;
    kNotice = 3,
    kWarning = 4,
    kError = 5,
    kCritical = 6,
    kOff = 7
};

static const char* level_names[] {"trace", "debug", "info", "notice", "warning,"
                                  "error", "critical", "off"};
static const char* short_level_names[] {"T", "D", "I", "N", "W", "E", "C", "O"};

inline const char* to_str(LevelEnum l) { return level_names[l]; }
inline const char* to_short_str(LevelEnum l) { return short_level_names[l]; }

//
// async overflow policy - block by default
enum class AsyncOverflowPolicy
{
    kBlockRetry; // block / yield / sleep until message can be enqueued
    kDiscardLogMsg; // discard the message it enqueue fails
};

//
// log exception
//
class SimpleException : public std::exception
{
public:
    explicit LogException(const std::string& msg) : msg_(msg) {}
    const char* what() const throw() override
    {
        return msg_.c_str();
    }
private:
    std::string msg_;
};
} // ns pb
#endif // PB_LOG_COMMON_DEF_H_
