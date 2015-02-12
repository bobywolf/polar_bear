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
* @Date:   2015-02-12 19:22:58
* @Last Modified by:   wangxiaobo
* @Last Modified time: 2015-02-12 19:27:29
*/

#ifndef PB_LOG_SINKS_SINK_H_
#define PB_LOG_SINKS_SINK_H_

#include <common/log/log_message.h>

namespace pb
{
// pure abstract base class
class Sink
{
public:
    virtual ~Sink() {}
    virtual void Log(const LogMessage& msg) = 0;
};
}

#endif // PB_LOG_SINKS_SINK_H_
