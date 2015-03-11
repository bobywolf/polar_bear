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
* @Date:   2015-03-10 17:59:20
* @Last Modified by:   wangxiaobo
* @Last Modified time: 2015-03-11 13:48:11
*/

#ifndef PB_LOG_LOGGER_H_
#define PB_LOG_LOGGER_H_

#include <vector>
#include <memory>
#include "sinks/base_sink.h"
#include "common_def.h"

namespace pb
{
// forward declaration
class LineLogger;

class Logger
{
public:
    Logger(const std::string& logger_name, sink_ptr single_sink);
    Logger(const std::string& name, sinks_init_list);
    template<class It>
    Logger(const std::string& name, const It& begin, const It& end);

    virtual ~Logger();
    // disallow implicit ctor and copy assignment
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void set_level(LevelEnum);
    LevelEnum level() const;

    const std::string& name() const;
    bool ShouldLog(LevelEnum) const;

    // logger.info(cppformat_string, arg1, arg2, arg3, ...) call style
    template <typename... Args>
    LineLogger trace(const char* format, const Args&... args);
    template <typename... Args>
    LineLogger debug(const char* format, const Args&... args);
    template <typename... Args>
    LineLogger info(const char* format, const Args&... args);
    template <typename... Args>
    LineLogger notice(const char* format, const Args&... args);
    template <typename... Args>
    LineLogger warn(const char* format, const Args&... args);
    template <typename... Args>
    LineLogger error(const char* format, const Args&... args);
    template <typename... Args>
    LineLogger critical(const char* format, const Args&... args);

    // logger.info(msg) << "..." call style
    template <typename T> LineLogger trace(const T&);
    template <typename T> LineLogger debug(const T&);
    template <typename T> LineLogger info(const T&);
    template <typename T> LineLogger notice(const T&);
    template <typename T> LineLogger warn(const T&);
    template <typename T> LineLogger error(const T&);
    template <typename T> LineLogger critical(const T&);

    // logger.info() << "..." call style
    LineLogger trace(const T&);
    LineLogger debug(const T&);
    LineLogger info(const T&);
    LineLogger notice(const T&);
    LineLogger warn(const T&);
    LineLogger error(const T&);
    LineLogger critical(const T&);

    // create log message with the given level, no matter what is the actual
    // logger's level
    template <typename... Args>
    LineLogger ForceLog(LevelEnum level, const char* format,
                        const Args&... args);

    // set the format of the log messages from this logger
    void set_pattern(const std::string&);
    void set_formatter(formatter_ptr);
protected:
    virtual void _LogMsg(LogMessage&);
    virtual void _SetPattern(const std::string&);
    virtual void _SetFormatter(formatter_ptr);
    LineLogger _LogIfEnabled(LevelEnum level);
    template <typename... Args>
    LineLogger _LogIfEnabled(LevelEnum level,
                             const char* format,
                             const Args&... args);
    template<typename T>
    inline LineLogger _LogIfEnabled(LevelEnum level, const T& msg);

    friend LineLogger;
    std::string name_;
    std::vector<sink_ptr> sinks_;
    formatter_ptr formatter_;
    std::atomic_int level_;
};
}
#endif // PB_LOG_LOGGER_H_
