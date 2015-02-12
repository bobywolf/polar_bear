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
* @Date:   2015-02-12 19:26:36
* @Last Modified by:   wangxiaobo
* @Last Modified time: 2015-02-12 19:51:13
*/

#ifndef PB_LOG_SINKS_BASE_SINK_H_
#define PB_LOG_SINKS_BASE_SINK_H_

#include <string>
#include <mutex>
#include <atomic>
#include "sink.h"

namespace pb
{
// base sink templated over a mutex (either dummy or realy)
// concrete implementation should only override the SinkIt method
// all locking is taken care of here so no locking needed by the implementators.
template <class Mutex>
class BaseSink : public Sink
{
public:
    BaseSink(): mutex_() {}
    virtual ~BaseSink() = default;

    BaseSink(const BaseSink&) = delete;
    BaseSink& operator=(const BaseSink&) = delete;

    void Log(const LogMessage& msg) override
    {
        std::lock_guard<Mutex>lock(mutex_);
        SinkIt(msg);
    }
protected:
    virtual void SinkIt(const LogMessage &msg) = 0;
    Mutex mutex_;
};
}

#define // PB_LOG_SINKS_BASE_SINK_H_