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
* @Date:   2015-03-10 17:27:34
* @Last Modified by:   wangxiaobo
* @Last Modified time: 2015-03-10 17:55:54
*/

#ifndef PB_LOG_ASYNC_LOG_HELPER_H_
#define PB_LOG_ASYNC_LOG_HELPER_H_

#include <chrono>
#include <thread>
#include <atomic>
#include <functional>

#include <common/log/common_def.h>
#include <common/log/sinks/sink.h>
#include <common/container/queue/mpmc_bounded_queue.h>
#include <common/log/log_message.h>
#include <common/util/format.h>

namespace pb
{
class AsyncLogHelper
{
    // Async msg to move to/from the queue
    // Movable only. should never be copied
    struct AsyncMsg
    {
        std::string logger_name;
        LevelEnum level;
        log_clock::time_point time;
        std::string txt;

        AsyncMsg() = default;
        ~AsyncMsg() = default;

        AsyncMsg(AsyncMsg&& other) PB_NOEXCEPT:
            logger_name(std::move(other.logger_name)),
            level(std::move(other.level)),
            time(std::move(other.time)),
            txt(std::move(other.txt))
        {}

        AsyncMsg& operator=(AsyncMsg&& other) PB_NOEXCEPT
        {
            logger_name = std::move(other.logger_name);
            level = other.level;
            time = std::move(other.time);
            txt = std::move(other.txt);
            return *this;
        }
        // never copy or assign. should only be moved..
        AsyncMsg(const AsyncMsg&) = delete;
        AsyncMsg& operator=(AsyncMsg& other) = delete;

        // construct from log_msg
        AsyncMsg(const LogMessage& m) :
            logger_name(m.logger_name),
            level(m.level),
            time(m.time),
            txt(m.raw.data(), m.raw.size())
        {}


        // copy into log_msg
        void FillLogMsg(LogMessage &msg)
        {
            msg.clear();
            msg.logger_name = logger_name;
            msg.level = level;
            msg.time = time;
            msg.raw << txt;
        }
    };

public:

    using item_type = async_msg;
    using q_type = mpmc_bounded_queue<item_type>;

    using clock = std::chrono::steady_clock;


    AsyncLogHelper(formatter_ptr formatter,
                   const std::vector<sink_ptr>& sinks,
                   size_t queue_size,
                   const AsyncOverflowPolicy overflow_policy = AsyncOverflowPolicy::kBlockRetry,
                   const std::function<void()>& worker_warmup_cb = nullptr);

    void Log(const LogMessage& msg);

    // stop logging and join the back thread
    virtual ~AsyncLogHelper();

    void SetFormatter(formatter_ptr);
private:
    formatter_ptr _formatter;
    std::vector<std::shared_ptr<Sink>> _sinks;

    // queue of messages to log
    q_type _q;

    // last exception thrown from the worker thread
    std::shared_ptr<SimpleException> _last_workerthread_ex;

    // overflow policy
    const AsyncOverflowPolicy _overflow_policy;

    // worker thread warmup callback - one can set thread priority, affinity, etc
    const std::function<void()> _worker_warmup_cb;

    // worker thread
    std::thread _worker_thread;

    // throw last worker thread exception or if worker thread is not active
    void ThrowIfBadWorker();

    // worker thread main loop
    void WorkerLoop();

    // pop next message from the queue and process it
    // return true if a message was available (queue was not empty),
    // will set the last_pop to the pop time
    bool ProcessNextMsg(clock::time_point& last_pop);

    // sleep,yield or return immediatly using the time passed since
    // last message as a hint
    static void SleepOrYield(const clock::time_point& last_op_time);
};
} // ns pb

///////////////////////////////////////////////////////////////////////////////
// async_sink class implementation
///////////////////////////////////////////////////////////////////////////////
inline AsyncLogHelper::AsyncLogHelper(
    formatter_ptr formatter,
    const std::vector<sink_ptr>& sinks,
    size_t queue_size,
    const AsyncOverflowPolicy overflow_policy,
    const std::function<void()>& worker_warmup_cb) :
    _formatter(formatter),
    _sinks(sinks),
    _q(queue_size),
    _overflow_policy(overflow_policy),
    _worker_warmup_cb(worker_warmup_cb),
    _worker_thread(&AsyncLogHelper::worker_loop, this)
{}

// Send to the worker thread termination message(level=off)
// and wait for it to finish gracefully
inline AsyncLogHelper::~AsyncLogHelper()
{

    try
    {
        Log(LogMsg(level::kOff));
        _worker_thread.join();
    }
    catch (...) //Dont crash if thread not joinable
    {}
}


//Try to push and block until succeeded
inline void AsyncLogHelper::Log(const LogMessage& msg)
{
    ThrowIfBadWorker();
    AsyncMsg new_msg(msg);
    if (!_q.enqueue(std::move(new_msg))
        && _overflow_policy != AsyncOverflowPolicy::kDiscardLogMsg)
    {
        auto last_op_time = clock::now();
        do
        {
            SleepOrYield(last_op_time);
        }
        while (!_q.enqueue(std::move(new_msg)));
    }
}

inline void AsyncLogHelper::WorkerLoop()
{
    try
    {
        if (_worker_warmup_cb)
        {
            _worker_warmup_cb();
        }
        clock::time_point last_pop = clock::now();
        while (ProcessNextMsg(last_pop));
    }
    catch (const std::exception& ex)
    {
        _last_workerthread_ex = std::make_shared<SimpleExecption>(
            std::string("async_logger worker thread exception: ") + ex.what());
    }
    catch (...)
    {
        _last_workerthread_ex = std::make_shared<SimpleExecption>(
            "async_logger worker thread exception");
    }
}

// process next message in the queue
// return true if this thread should still be active (no msg with level::off was received)
inline bool AsyncLogHelper::ProcessNextMsg(clock::time_point& last_pop)
{

    AsyncMsg incoming_async_msg;
    LogMessage incoming_log_msg;

    if (_q.dequeue(incoming_async_msg))
    {
        last_pop = clock::now();

        if(incoming_async_msg.level == level::kOff)
        {
            return false;
        }

        incoming_async_msg.FillLogMsg(incoming_log_msg);
        _formatter->format(incoming_log_msg);
        for (auto &s : _sinks)
        {
            s->Log(incoming_log_msg);
        }
    }
    else //empty queue
    {
        SleepOrYield(last_pop);
    }
    return true;
}

inline void AsyncLogHelper::SetFormatter(formatter_ptr msg_formatter)
{
    _formatter = msg_formatter;
}


// sleep,yield or return immediatly using the time passed since last message as a hint
inline void AsyncLogHelper::SleepOrYield(const clock::time_point& last_op_time)
{
    using std::chrono::milliseconds;
    using namespace std::this_thread;

    auto time_since_op = clock::now() - last_op_time;

    // spin upto 1 ms
    if (time_since_op <= milliseconds(1))
    {
        return;
    }

    // yield upto 10ms
    if (time_since_op <= milliseconds(10))
    {
        return yield();
    }

    // sleep for half of duration since last op
    if (time_since_op <= milliseconds(100))
    {
        return sleep_for(time_since_op / 2);
    }

    return sleep_for(milliseconds(100));
}

// throw if the worker thread threw an exception or not active
inline void AsyncLogHelper::ThrowIfBadWorker()
{
    if (_last_workerthread_ex)
    {
        auto ex = std::move(_last_workerthread_ex);
        throw *ex;
    }
}

#endif // PB_LOG_ASYNC_LOG_HELPER_H_