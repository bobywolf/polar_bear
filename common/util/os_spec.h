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
* @Date:   2015-02-12 15:26:25
* @Last Modified by:   wangxiaobo
* @Last Modified time: 2015-02-12 17:40:54
*/

#ifndef PB_UTIL_OS_SPEC_H_
#define PB_UTIL_OS_SPEC_H_

#include <string>
#include <cstdio>
#include <ctime>

#ifdef _WIN32
# ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
# endif
#include <Windows.h>
#endif

#include <common/log/common_def.h>

namespace pb
{
namespace os
{
inline pb::log_clock::time_point now()
{
#ifdef PB_LOG_CLOCK_COARSE
    timespec ts;
    ::clock_gettime(CLOCK_REALTIME_COARSE, &ts);
    return std::chrono::time_point<log_clock, typename log_clock::duration>(
      std::chrono::duration_cast<typename log_clock::duration>(
      std::chrono::seconds(tv.tv_sec) + std::chrono::nanoseconds(tv.tv_nsec)));
#endif
    return log_clock::now();
}

inline std::tm localtime()
{
    std::time_t now_t = time(nullptr);
    return localtime(now_t);
}

inline std::tm gmtime(const std::time_t &time_tt)
{
    std::tm tm;
#ifdef _WIN32
    gmtime_s(&tm, &time_tt);
#else
    gmtime_r(&time_tt, &tm);
#endif
    return tm;
}

inline std::tm gmtime()
{
    std::time_t now_t = time(nullptr);
    return gmtime(now_t);
}

inline bool operator==(const std::tm& tm1, const std::tm& tm2)
{
    return (tm1.tm_sec == tm2.tm_sec &&
            tm1.tm_min == tm2.tm_min &&
            tm1.tm_hour == tm2.tm_hour &&
            tm1.tm_mday == tm2.tm_mday &&
            tm1.tm_mon == tm2.tm_mon &&
            tm1.tm_year == tm2.tm_year &&
            tm1.tm_isdst == tm2.tm_isdst);
}

inline bool operator!=(const std::tm& tm1, const std::tm& tm2)
{
    return !(tm1 == tm2);
}

#ifdef _WIN32
inline const char *eol() { return "\r\n"; }
inline unsigned short eol_size() { return 2; }
#else
constexpr inline const char* eol() { return "\r\n"; }
constexpr inline unsigned short eol_size() { return 1; }
#endif

//fopen_s on non windows for writing
inline int fopen_s(FILE** fp, const std::string& filename, const char* mode)
{
#ifdef _WIN32
    *fp = _fsopen((filename.c_str()), mode, _SH_DENYWR);
    return *fp == nullptr;
#else
    *fp = std::fopen((filename.c_str()), mode);
    return *fp == nullptr;
#endif
}

//Return utc offset in minutes or -1 on failure
inline int utc_minutes_offset(const std::tm& tm = os::localtime())
{

#ifdef _WIN32
    (void)tm; // avoid unused param warning
    DYNAMIC_TIME_ZONE_INFORMATION tzinfo;
    auto rv = GetDynamicTimeZoneInformation(&tzinfo);
    if (!rv)
    {
        return -1;
    }
    return -1 * (tzinfo.Bias + tzinfo.DaylightBias);
#else
    return static_cast<int>(tm.tm_gmtoff / 60);
#endif
}
} // ns os
} // ns pb

#endif
