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
* @Date:   2015-03-10 14:43:08
* @Last Modified by:   wangxiaobo
* @Last Modified time: 2015-03-10 14:48:08
*/

#ifndef PB_LOG_SINKS_STDOUT_SINK_H_
#define PB_LOG_SINKS_STDOUT_SINK_H_

#include <iostream>
#include <mutex>
#include <common/util/null_mutex.h>
#include "ostream_sink.h"

namespace pb
{
template <class Mutex>
class StdoutSink : public OStreamSink<Mutex>
{
public:
    StdoutSink() : OStreamSink<Mutex>(std::cout, true) {}
};

typedef StdoutSink<NullMutex> StdoutSinkSt;
typedef StdoutSink<std::mutex> StdoutSinkMt;
}
#endif