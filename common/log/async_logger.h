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
* @Date:   2015-03-11 18:12:38
* @Last Modified by:   wangxiaobo
* @Last Modified time: 2015-03-11 18:17:02
*/

#ifndef PB_LOG_ASYNC_LOGGER_H_
#define PB_LOG_ASYNC_LOGGER_H_

// Very fast asynchronous logger (millions of logs per second on an average desktop)
// Uses pre allocated lockfree queue for maximum throughput even under large number of threads.
// Creates a single back thread to pop messages from the queue and log them.
//
// Upon each log write the logger:
//    1. Checks if its log level is enough to log the message
//    2. Push a new copy of the message to a queue (or block the caller until space is available in the queue)
//    3. will throw spdlog_ex upon log exceptions
// Upong destruction, logs all remaining messages in the queue before destructing..

#include <chrono>
#include <functional>
#include "common_def.h"
#include "logger.h"
#include "log.h"

#endif // PB_LOG_ASYNC_LOGGER_H_
