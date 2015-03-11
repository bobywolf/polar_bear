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
* @Date:   2015-03-11 18:17:49
* @Last Modified by:   wangxiaobo
* @Last Modified time: 2015-03-11 18:38:56
*/

#ifndef PB_LOG_LOG_H_
#define PB_LOG_LOG_H_

#include "common_def.h"
#include "logger.h"

namespace pb
{
namespace log
{
// Return an existing logger or nullptr if a logger with such name
// doesn't exist.
// Examples:
//
// pb::log::get("mylog")->info("Hello");
// auto logger = pb::log::get("mylog");
// logger.info("This is another message" , x, y, z);
// logger.info() << "This is another message" << x << y << z;
std::shared_ptr<Logger> get(const std::string& name);

//
// Set global formatting
// example: pb::log::set_pattern("%Y-%m-%d %H:%M:%S.%e %l : %v");
//
void set_pattern(const std::string& format_string);
void set_formatter(formatter_ptr f);

//
// Set global logging level for
//
void set_level(level::level_enum log_level);

//
// Turn on async mode (off by default) and set the queue size for each
// async_logger. effective only for loggers created after this call.
// queue_size: size of queue (must be power of 2):
//    Each logger will pre-allocate a dedicated queue with queue_size entries
//    upon construction.
//
// AsyncOverflowPolicy (optional, kBlockRetry by default):
//    AsyncOverflowPolicy::kBlockRetry - if queue is full, block until queue has
//                                       room for the new log entry.
//    AsyncOverflowPolicy::kDiscardLogMsg - never block and discard any new
//                                          messages when queue  overflows.
//
// worker_warmup_cb (optional):
//     callback function that will be called in worker thread upon start (can be
//     used to init stuff like thread affinity)
//
void set_async_mode(size_t queue_size,
                    const AsyncOverflowPolicy overflow_policy =
                            AsyncOverflowPolicy::kBlockRetry,
                    const std::function<void()>& worker_warmup_cb = nullptr);

// Turn off async mode
void set_sync_mode();

//
// Create multi/single threaded rotating file logger
//
std::shared_ptr<Logger> rotating_logger_mt(
    const std::string& logger_name,
    const std::string& filename,
    size_t max_file_size,
    size_t max_files,
    bool force_flush = false);
std::shared_ptr<Logger> rotating_logger_st(
    const std::string& logger_name,
    const std::string& filename,
    size_t max_file_size,
    size_t max_files,
    bool force_flush = false);

//
// Create file logger which creates new file on the given time (default in  midnight):
//
std::shared_ptr<Logger> daily_logger_mt(
    const std::string& logger_name,
    const std::string& filename,
    int hour=0,
    int minute=0,
    bool force_flush = false);
std::shared_ptr<Logger> daily_logger_st(
    const std::string& logger_name,
    const std::string& filename,
    int hour=0,
    int minute=0,
    bool force_flush = false);


//
// Create stdout/stderr loggers
//
std::shared_ptr<Logger> stdout_logger_mt(const std::string& logger_name);
std::shared_ptr<Logger> stdout_logger_st(const std::string& logger_name);
std::shared_ptr<Logger> stderr_logger_mt(const std::string& logger_name);
std::shared_ptr<Logger> stderr_logger_st(const std::string& logger_name);


//
// Create a syslog logger
//
#ifdef __linux__
std::shared_ptr<Logger> syslog_logger(const std::string& logger_name,
                                      const std::string& ident = "",
                                      int syslog_option = 0);
#endif


// Create a logger with multiple sinks
std::shared_ptr<Logger> create(const std::string& logger_name,
                               sinks_init_list sinks);
template<class It>
std::shared_ptr<Logger> create(const std::string& logger_name,
                               const It& sinks_begin,
                               const It& sinks_end);


// Create a logger with templated sink type
// Example: pb::log::create<DailyFileSinkSt>("mylog", "dailylog_filename", "txt");
template <typename Sink, typename... Args>
std::shared_ptr<Logger> create(const std::string& logger_name, const Args&...);

//
// Trace & debug macros to be switched on/off at compile time for zero cost
// debug statements.
// Note: using these mactors overrides the runtime log threshold of the logger.
//
// Example:
//
// Enable debug macro, must be defined before including spdlog.h
// #define SPDLOG_DEBUG_ON
// include "spdlog/spdlog.h"
// SPDLOG_DEBUG(my_logger, "Some debug message {} {}", 1, 3.2);
//

#ifdef SPDLOG_TRACE_ON
#define SPDLOG_TRACE(logger, ...) logger->ForceLog(pb::kTrace,  __VA_ARGS__) << " (" << __FILE__ << " #" << __LINE__ <<")";
#else
#define SPDLOG_TRACE(logger, ...)
#endif


#ifdef SPDLOG_DEBUG_ON
#define SPDLOG_DEBUG(logger, ...) logger->ForceLog(pb::kDebug, __VA_ARGS__)
#else
#define SPDLOG_DEBUG(logger, ...)
#endif



// Drop the reference to the given logger
void drop(const std::string &name);

// Drop all references
void drop_all();

} // ns log
} // ns pb

#endif // PB_LOG_LOG_H_
