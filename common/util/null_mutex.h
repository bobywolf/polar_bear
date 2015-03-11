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
* @Date:   2015-03-10 11:26:29
* @Last Modified by:   wangxiaobo
* @Last Modified time: 2015-03-10 14:14:43
*/

#ifndef PB_UTIL_NULL_MUTEX_H_
#define PB_UTIL_NULL_MUTEX_H_

namespace pb
{
class NullMutex
{
public:
    void Lock() {}
    void UnLock() {}
    bool TryLock() { return true; }
};
} // ns pb

#endif // PB_UTIL_NULL_MUTEX_H_