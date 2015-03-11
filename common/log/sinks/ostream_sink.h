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
* @Date:   2015-03-10 11:17:42
* @Last Modified by:   wangxiaobo
* @Last Modified time: 2015-03-10 14:42:05
*/

#ifndef PB_LOG_SINKS_OSTREAM_SINK_H_
#define PB_LOG_SINKS_OSTREAM_SINK_H_

#include <ostream>
#include <mutex>
#include <memory>

#include <common/util/null_mutex.h>
#include "base_sink.h"

namespace pb
{
template<class Mutex>
class OStreamSink : public BaseSink<Mutex>
{
public:
    explicit OStreamSink(std::ostream& os, bool force_flush=false)
        : ostream_(os), force_flush_(force_flush) {}
    OStreamSink(const OStreamSink&) = delete;
    OStreamSink& operator=(const OStreamSink&) = delete;
    virtual ~OStreamSink() = default;
protected:
    virtual void SinkIt(const LogMessage& msg) override
    {
        ostream_.write(msg.formatted.data(), msg.formatted.size());
        if (force_flush_)
        {
            ostream_.flush();
        }
    }
    std::ostream ostream_;
    bool force_flush_;
};

typedef OStreamSink<std::mutex> OStreamSinkMt;
typedef OStreamSink<NullMutex> OStreamSinkSt;
}
#endif // PB_LOG_SINKS_OSTREAM_SINK_H_