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
* @Date:   2015-03-10 15:47:09
* @Last Modified by:   wangxiaobo
* @Last Modified time: 2015-03-10 16:59:52
*/

#ifndef PB_LOG_SINKS_FILE_SINK_H_
#define PB_LOG_SINKS_FILE_SINK_H_

#include <mutex>
#include <common/util/null_mutex.h>
#include <common/io/file_helper.h>
#include <common/util/format.h>
#include <common/util/os_spec.h>
#include "base_sink.h"

namespace pb
{
// trivial file sink with single file as target
template <class Mutex>
class SimpleFileSink : public BaseSink<Mutex>
{
public:
    explicit SimpleFileSink(const std::string& filename,
                            bool force_flush=false) :
        file_helper_(force_flush)
    {
        file_helper_.Open(filename);
    }
protected:
    void SinkIt(const LogMessage& msg) override
    {
        file_helper.Write(msg);
    }
private:
    FileHelper file_helper_;
};

typedef SimpleFileSink<std::mutex> SimpleFileSinkMt;
typedef SimpleFileSink<NullMutex> SimpleFileSinkSt;

// Rotating file sink based on size
template<class Mutex>
class RotatingFileSink : public BaseSink<Mutex>
{
public:
    RotatingFileSink(const std::string &base_filename,
                     const std::string &extension,
                     std::size_t max_size,
                     std::size_t max_files,
                     bool force_flush=false):
        base_filename_(base_filename),
        extension_(extension),
        max_size_(max_size),
        max_files_(max_files),
        current_size_(0),
        file_helper_(force_flush)
    {
        file_helper_.Open(CalcFilename(base_filename_, 0, extension_));
    }


protected:
    void SinkIt(const LogMessage& msg) override
    {
        current_size_ += msg.formatted.size();
        if (current_size_  > max_size_)
        {
            _Rotate();
            current_size_ = msg.formatted.size();
        }
        file_helper_.Write(msg);
    }


private:
    static std::string CalcFilename(const std::string& filename,
                                    std::size_t index,
                                    const std::string& extension)
    {
        fmt::MemoryWriter w;
        if (index)
        {
            w.write("{}.{}.{}", filename, index, extension);
        } else
        {
            w.write("{}.{}", filename, extension);
        }
        return w.str();
    }


    // Rotate files:
    // log.txt -> log.1.txt
    // log.1.txt -> log2.txt
    // log.2.txt -> log3.txt
    // log.3.txt -> delete

    void _Rotate()
    {
        file_helper_.close();
        for (auto i = max_files_; i > 0; --i)
        {
            std::string src = CalcFilename(base_filename_, i - 1, extension_);
            std::string target = CalcFilename(base_filename_, i, extension_);

            if (FileHelper::file_exists(target))
            {
                if (std::remove(target.c_str()) != 0)
                {
                    throw SimpleException("rotating_file_sink: failed removing " + target);
                }
            }
            if (FileHelper::file_exists(src)
                && std::rename(src.c_str(), target.c_str()))
            {
                throw SimpleException("rotating_file_sink: failed renaming " + src + " to " + target);
            }
        }
        file_helper_.reopen(true);
    }
    std::string base_filename_;
    std::string extension_;
    std::size_t max_size_;
    std::size_t max_files_;
    std::size_t current_size_;
    FileHelper file_helper_;
};

typedef RotatingFileSink<std::mutex> RotatingFileSinkMt;
typedef RotatingFileSink<NullMutex> RotatingFileSinkSt;


// Rotating file sink based on date. rotates at midnight
template<class Mutex>
class DailyFileSink : public BaseSink<Mutex>
{
public:
    //create daily file sink which rotates on given time
    DailyFileSink(
        const std::string& base_filename,
        const std::string& extension,
        int rotation_hour,
        int rotation_minute,
        bool force_flush=false) :
        base_filename_(base_filename),
        extension_(extension),
        rotation_h_(rotation_hour),
        rotation_m_(rotation_minute),
        file_helper_(force_flush)
    {
        if (rotation_hour < 0 || rotation_hour > 23
            || rotation_minute < 0 || rotation_minute > 59)
        {
            throw SimpleException("daily_file_sink: Invalid rotation time in ctor");
        }
        rotation_tp_ = _NextRotationTp(),
        file_helper_.Open(CalcFilename(base_filename_, extension_));
    }

protected:
    void SinkIt(const LogMessage& msg) override
    {
        if (std::chrono::system_clock::now() >= rotation_tp_)
        {
            file_helper_.close();
            file_helper_.open(CalcFilename(base_filename_, extension_));
            rotation_tp_ = _NextRotationTp();
        }
        file_helper_.Write(msg);
    }

private:
    std::chrono::system_clock::time_point _NextRotationTp()
    {
        using namespace std::chrono;
        auto now = system_clock::now();
        time_t tnow = system_clock::to_time_t(now);
        tm date = pb::os::localtime(tnow);
        date.tm_hour = rotation_h_;
        date.tm_min = rotation_m_;
        date.tm_sec = 0;
        auto rotation_time = system_clock::from_time_t(std::mktime(&date));
        if (rotation_time > now)
        {
            return rotation_time;
        }
        else
        {
            return system_clock::time_point(rotation_time + hours(24));
        }
    }

    //Create filename for the form basename.YYYY-MM-DD.extension
    static std::string CalcFilename(const std::string& basename,
                                    const std::string& extension)
    {
        std::tm tm = pb::os::localtime();
        fmt::MemoryWriter w;
        w.write("{}_{:04d}-{:02d}-{:02d}_{:02d}-{:02d}.{}",
                basename,
                tm.tm_year + 1900,
                tm.tm_mon + 1,
                tm.tm_mday,
                tm.tm_hour,
                tm.tm_min,
                extension);
        return w.str();
    }

    std::string base_filename_;
    std::string extension_;
    int rotation_h_;
    int rotation_m_;
    std::chrono::system_clock::time_point rotation_tp_;
    FileHelper file_helper_;
};

typedef DailyFileSink<std::mutex> DailyFileSinkMt;
typedef DailyFileSink<NullMutex> DailyFileSinkSt;
} // ns pb
#define PB_LOG_SINKS_FILE_SINK_H_
