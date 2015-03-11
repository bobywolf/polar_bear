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
* @Date:   2015-03-11 13:27:19
* @Last Modified by:   wangxiaobo
* @Last Modified time: 2015-03-11 14:17:26
*/

#include "line_logger.h"

// create logger with given name, sinks and the default pattern formatter
// all other ctors will call this one
template<class It>
inline pb::Logger::Logger(const std::string& logger_name,
                          const It& begin, const It& end) :
    name_(logger_name), sinks_(begin, end),
    _formatter(std::make_shared<pattern_formatter>("%+"))
{
    // no support under vs2013 for member initialization for std::atomic
    level_ = kInfo;
}

// ctor with sinks as init list
inline pb::Logger::Logger(const std::string& logger_name,
                          pb::sinks_init_list sinks_list) :
    Logger(logger_name, sinks_list.begin(), sinks_list.end()) {}

// ctor with single sink
inline pb::Logger::Logger(const std::string& logger_name,
                          pb::sink_ptr single_sink) :
    Logger(logger_name, { single_sink }) {}

inline pb::Logger::~Logger() = default;

inline void pb::Logger::set_formatter(pb::formatter_ptr msg_formatter)
{
    _SetFormatter(msg_formatter);
}

inline void pb::Logger::set_pattern(const std::string& pattern)
{
    _SetPattern(pattern);
}

// log only if the given level >= logger's log level
template <typename... Args>
inline pb::LineLogger pb::Logger::_LogIfEnabled(
    LevelEnum level, const char* fmt, const Args&... args)
{
    bool msg_enabled = ShouldLog(level);
    LineLogger logger(this, level, msg_enabled);
    logger.write(fmt, args...);
}

inline pb::LineLogger pb::Logger::_LogIfEnabled(LevelEnum level)
{
    return LineLogger(this, level, ShouldLog(level));
}

template <typename T>
inline pb::LineLogger pb::Logger::_LogIfEnabled(LevelEnum level, const T& msg)
{
    bool msg_enabled = ShouldLog(level);
    LineLogger logger(this, level, msg_enabled);
    logger << msg;
    return logger;
}

// logger.info(cppformat_string, arg1, arg2, arg3, ...) call style
template <typename... Args>
inline pb::LineLogger pb::Logger::trace(const char* fmt, const Args&... args)
{
    return _LogIfEnabled(kTrace, fmt, args...);
}

template <typename... Args>
inline pb::LineLogger pb::Logger::debug(const char* fmt, const Args&... args)
{
    return _LogIfEnabled(kDebug, fmt, args...);
}

template <typename... Args>
inline pb::LineLogger pb::Logger::info(const char* fmt, const Args&... args)
{
    return _LogIfEnabled(kInfo, fmt, args...);
}

template <typename... Args>
inline pb::LineLogger pb::Logger::notice(const char* fmt, const Args&... args)
{
    return _LogIfEnabled(kNotice, fmt, args...);
}

template <typename... Args>
inline pb::LineLogger pb::Logger::warn(const char* fmt, const Args&... args)
{
    return _LogIfEnabled(kWarning, fmt, args...);
}

template <typename... Args>
inline pb::LineLogger pb::Logger::error(const char* fmt, const Args&... args)
{
    return _LogIfEnabled(kError, fmt, args...);
}

template <typename... Args>
inline pb::LineLogger pb::Logger::critical(const char* fmt, const Args&... args)
{
    return _LogIfEnabled(kCritical, fmt, args...);
}

// logger.info(msg) << ".." call style
template <typename T>
inline pb::LineLogger pb::Logger::trace(const T& msg)
{
    return _LogIfEnabled(kTrace, msg);
}

template <typename T>
inline pb::LineLogger pb::Logger::debug(const T& msg)
{
    return _LogIfEnabled(kDebug, msg);
}

template <typename T>
inline pb::LineLogger pb::Logger::info(const T& msg)
{
    return _LogIfEnabled(kInfo, msg);
}

template <typename T>
inline pb::LineLogger pb::Logger::notice(const T& msg)
{
    return _LogIfEnabled(kNotice, msg);
}

template <typename T>
inline pb::LineLogger pb::Logger::warn(const T& msg)
{
    return _LogIfEnabled(kWarning, msg);
}

template <typename T>
inline pb::LineLogger pb::Logger::error(const T& msg)
{
    return _LogIfEnabled(kError, msg);
}

template <typename T>
inline pb::LineLogger pb::Logger::critical(const T& msg)
{
    return _LogIfEnabled(kCritical, msg);
}

// logger.info << ".." call style
inline pb::LineLogger pb::Logger::trace()
{
    return _LogIfEnabled(kTrace)
}

inline pb::LineLogger pb::Logger::debug()
{
    return _LogIfEnabled(kDebug)
}

inline pb::LineLogger pb::Logger::info()
{
    return _LogIfEnabled(kInfo)
}

inline pb::LineLogger pb::Logger::notice()
{
    return _LogIfEnabled(kNotice)
}

inline pb::LineLogger pb::Logger::warn()
{
    return _LogIfEnabled(kWarning)
}

inline pb::LineLogger pb::Logger::error()
{
    return _LogIfEnabled(kError)
}

inline pb::LineLogger pb::Logger::critical()
{
    return _LogIfEnabled(kCritical)
}

// always log, no matter what is the actual logger's log level
template <typename... Args>
inline pb::LineLogger pb::Logger::ForceLog(
    LevelEnum level, const char *fmt, const Args&... args)
{
    pb::LineLogger logger(this, level, true);
    logger.write(fmt, args...);
    return logger;
}

// name and level
inline const std::string& pb::Logger::name() const { return name_; }

inline void pb::Logger::set_level(pb::LevelEnum log_level)
{
    level_.store(log_level);
}

inline pb::LevelEnum pb::Logger::level() const
{
    return static_cast<pb::LevelEnum>(level_.load(std::memory_order_relaxed));
}

inline bool pb::Logger::ShouldLog(pb::LevelEnum msg_level) const
{
    return msg_level >= level_.load(std::memory_order_relaxed);
}

// protected virtual called at end of each user log call (if enabled)
// by the line_logger
inline void pb::Logger::_LogMsg(LogMessage& msg)
{
    formatter_->format(msg);
    for (auto &sink : sinks_)
    {
        sink->Log(msg);
    }
}

inline void pb::Logger::_SetPattern(const std::string& pattern)
{
    formatter_ = std::make_shared<pattern_formatter>(pattern);
}

inline void pb::Logger::_SetFormatter(formatter_ptr msg_formatter)
{
    formatter_ = msg_formatter;
}
