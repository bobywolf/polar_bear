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
* @Date:   2015-02-12 17:21:45
* @Last Modified by:   wangxiaobo
* @Last Modified time: 2015-03-10 17:00:09
*/

#ifndef PB_IO_FILE_HELPER_H_
#define PB_IO_FILE_HELPER_H_

#include <string>
#include <thread>
#include <chrono>

#include <common/util/os_spec.h>

namespace pb
{
// helper class for file sink.
// when failling to open a file, retry several times(5) with small delay between
// the tries(10 ms)
// throws SimpleException
class FileHelper
{
public:
    const int open_tries = 5;
    const int open_interval = 10;

    explicit FileHelper(bool force_flush) :
        fd_(nullptr), force_flush_(force_flush) {}

    // disable copy constructor and assignment
    FileHelper(const FileHelper&) = delete;
    FileHelper& operator = (const FileHelper&) = delete;

    virtual ~FileHelper() { close(); }

    /**
     * @brief open file with name and truncate option
     *
     * @param filename [specified filename]
     * @param truncate [whether truncate the existing file]
     */
    void Open(const std::string &filename, bool truncate = false)
    {
        close();
        const char *mode = truncate ? "wb" : "ab";
        filename_ = filename;
        for (int tries = 0; tries < open_tries; ++tries)
        {
            if (! os::fopen_s(&fd_, filename_, mode))
            {
                return;
            }
            std::this_thread::sleep_for(
                std::chrono::milliseconds(open_interval));
        }
    }

    /**
     * @brief reopen file
     *
     * @param truncate [whether truncate the existing file]
     */
    void Reopen(bool truncate)
    {
        if (filename_.empty())
        {
            throw SimpleException("failed re opening file - was not opened"
                " before");
            open(filename_, truncate);
        }
    }

    void Close()
    {
        if (fd_)
        {
            std::fclose(fd_);
            fd_ = nullptr;
        }
    }

    void Write(const LogMessage &msg)
    {
        size_t size = msg.formatted.size();
        auto data = msg.formatted.data();
        if (std::fwrite(data, 1, size, fd_) != size)
        {
            throw SimpleException("failed writing to file " + filename_);
        }
        if (force_flush_)
        {
            std::fflush(fd_);
        }
    }

    const std::string& filename() const { return filename_; }
    static bool FileExists(const std::string& name)
    {
        std::FILE *file = nullptr;
        if (! os::fopen_s(&file, name.c_str(), "r"))
        {
            std::fclose(file);
            return true;
        } else
        {
            return false;
        }
    }
private:
    std::FILE *fd_;
    std::string filename_;
    bool force_flush_;
};
} // ns pb

#endif // PB_IO_FILE_HELPER_H_