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

/**
 * A modified version of Bounded MPMC queue by Dmitry Vyukov.
 * Original code from:
 * http://www.1024cores.net/home/lock-free-algorithms/queues/bounded-mpmc-queue
 */
/* 
* @Author: wangxiaobo
* @Date:   2015-03-10 17:19:16
* @Last Modified by:   wangxiaobo
* @Last Modified time: 2015-03-10 17:25:45
*/

#ifndef PB_CONTAINER_QUEUE_MPMC_BOUNDED_QUEUE_H_
#define PB_CONTAINER_QUEUE_MPMC_BOUNDED_QUEUE_H_

#include <atomic>
#include <common/log/common_def.h>

namespace pb
{
template<typename T>
class mpmc_bounded_queue
{
public:
    using item_type = T;
    mpmc_bounded_queue(size_t buffer_size)
        : buffer_(new cell_t [buffer_size]),
          buffer_mask_(buffer_size - 1)
    {
        //queue size must be power of two
        if(!((buffer_size >= 2) && ((buffer_size & (buffer_size - 1)) == 0)))
        {
            throw SimpleException("async logger queue size must be power of two");
        }

        for (size_t i = 0; i != buffer_size; i += 1)
        {
            buffer_[i].sequence_.store(i, std::memory_order_relaxed);
        }
        enqueue_pos_.store(0, std::memory_order_relaxed);
        dequeue_pos_.store(0, std::memory_order_relaxed);
    }

    virtual ~mpmc_bounded_queue()
    {
        delete [] buffer_;
    }

    bool enqueue(T&& data)
    {
        cell_t* cell = nullptr;
        size_t pos = enqueue_pos_.load(std::memory_order_relaxed);
        for (;;)
        {
            cell = &buffer_[pos & buffer_mask_];
            size_t seq = cell->sequence_.load(std::memory_order_acquire);
            intptr_t dif = (intptr_t)seq - (intptr_t)pos;
            if (dif == 0)
            {
                if (enqueue_pos_.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed))
                {
                    break;
                }
            }
            else if (dif < 0)
            {
                return false;
            }
            else
            {
                pos = enqueue_pos_.load(std::memory_order_relaxed);
            }
        }
        cell->data_ = std::move(data);
        cell->sequence_.store(pos + 1, std::memory_order_release);
        return true;
    }

    bool dequeue(T& data)
    {
        cell_t* cell = nullptr;
        size_t pos = dequeue_pos_.load(std::memory_order_relaxed);
        for (;;)
        {
            cell = &buffer_[pos & buffer_mask_];
            size_t seq =
                cell->sequence_.load(std::memory_order_acquire);
            intptr_t dif = (intptr_t)seq - (intptr_t)(pos + 1);
            if (dif == 0)
            {
                if (dequeue_pos_.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed))
                {
                    break;
                }
            }
            else if (dif < 0)
            {
                return false;
            }
            else
            {
                pos = dequeue_pos_.load(std::memory_order_relaxed);
            }
        }
        data = std::move(cell->data_);
        cell->sequence_.store(pos + buffer_mask_ + 1, std::memory_order_release);
        return true;
    }

private:
    struct cell_t
    {
        std::atomic<size_t>   sequence_;
        T                     data_;
    };

    static size_t const     cacheline_size = 64;
    typedef char            cacheline_pad_t [cacheline_size];

    cacheline_pad_t         pad0_;
    cell_t* const           buffer_;
    size_t const            buffer_mask_;
    cacheline_pad_t         pad1_;
    std::atomic<size_t>     enqueue_pos_;
    cacheline_pad_t         pad2_;
    std::atomic<size_t>     dequeue_pos_;
    cacheline_pad_t         pad3_;

    mpmc_bounded_queue(mpmc_bounded_queue const&);
    void operator = (mpmc_bounded_queue const&);
};

} // ns pb

#define PB_CONTAINER_QUEUE_MPMC_BOUNDED_QUEUE_H_