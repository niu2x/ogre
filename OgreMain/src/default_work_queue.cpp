/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2014 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#include "OgreStableHeaders.h"
#include "timer.h"

namespace Ogre {
//---------------------------------------------------------------------
DefaultWorkQueue::DefaultWorkQueue(const String& name)
: name_(name)
, worker_thread_count_(1)
, response_time_limit_ms_(10)
, accept_requests_(true)
, shutting_down_(false)
, running_(false)
{
}

//---------------------------------------------------------------------
DefaultWorkQueue::~DefaultWorkQueue() { shutdown(); }

//---------------------------------------------------------------------
void DefaultWorkQueue::startup(bool force_restart)
{
    if (running_) {
        if (force_restart)
            shutdown();
        else
            return;
    }

    shutting_down_ = false;

    LogManager::singleton().stream()
        << "DefaultWorkQueue('" << name_ << "') initialising on thread "
        << OGRE_THREAD_CURRENT_ID << ".";

#if XDOG_USE_THREAD
    for (size_t i = 0; i < worker_thread_count_; ++i) {
        OGRE_THREAD_CREATE(t, [this]() { thread_main(); });
        workers_.push_back(t);
    }

#endif

    running_ = true;
}
//---------------------------------------------------------------------
void DefaultWorkQueue::shutdown()
{
    if (!running_)
        return;

    LogManager::singleton().stream()
        << "DefaultWorkQueue('" << name_ << "') shutting down on thread "
        << OGRE_THREAD_CURRENT_ID << ".";

    shutting_down_ = true;

#if XDOG_USE_THREAD
    // wake all threads (they should check shutting down as first thing after
    // wait)
    OGRE_THREAD_NOTIFY_ALL(request_cond_);

    // all our threads should have been woken now, so join
    for (auto& w : workers_) {
        w->join();
        OGRE_THREAD_DESTROY(w);
    }
    workers_.clear();
#endif

    running_ = false;
}
//---------------------------------------------------------------------
void DefaultWorkQueue::notify_workers()
{
    // wake up waiting thread
    OGRE_THREAD_NOTIFY_ONE(request_cond_);
}

//---------------------------------------------------------------------
void DefaultWorkQueue::wait_for_next_request()
{
#if XDOG_USE_THREAD
    // Lock; note that OGRE_THREAD_WAIT will free the lock
    OGRE_WQ_LOCK_MUTEX_NAMED(request_mutex_, queue_lock);
    if (tasks_.empty()) {
        // frees lock and suspends the thread
        OGRE_THREAD_WAIT(request_cond_, request_mutex_, queue_lock);
    }
    // When we get back here, it's because we've been notified
    // and thus the thread has been woken up. Lock has also been
    // re-acquired, but we won't use it. It's safe to try processing and fail
    // if another thread has got in first and grabbed the request
#endif
}
//---------------------------------------------------------------------
void DefaultWorkQueue::thread_main()
{
    // default worker thread
#if XDOG_USE_THREAD
    LogManager::singleton().stream()
        << "DefaultWorkQueue('" << name_ << "')::WorkerFunc - thread "
        << OGRE_THREAD_CURRENT_ID << " starting.";

    // Spin forever until we're told to shut down
    while (!is_shutting_down()) {
        wait_for_next_request();
        process_next_request();
    }

    LogManager::singleton().stream()
        << "DefaultWorkQueue('" << name_ << "')::WorkerFunc - thread "
        << OGRE_THREAD_CURRENT_ID << " stopped.";
#endif
}

void DefaultWorkQueue::add_main_thread_task(std::function<void()> task)
{
    if (!requests_accepted() || is_shutting_down())
        return;

    OGRE_WQ_LOCK_MUTEX(response_mutex_);
    main_thread_tasks_.push_back(task);
}

//---------------------------------------------------------------------
void DefaultWorkQueue::add_task(std::function<void()> task)
{
    OGRE_WQ_LOCK_MUTEX(request_mutex_);

    if (!requests_accepted() || is_shutting_down())
        return;

#if XDOG_USE_THREAD
    tasks_.push_back(task);
    notify_workers();
#else
    task(); // no threading, just run it
#endif

    LogManager::singleton().stream(LogMsgLevel::TRIVIAL)
        << "DefaultWorkQueueBase('" << name_
        << "') - QUEUED(thread:" << OGRE_THREAD_CURRENT_ID << ")";
}

//---------------------------------------------------------------------
void DefaultWorkQueue::set_requests_accepted(bool accept)
{
    OGRE_WQ_LOCK_MUTEX(request_mutex_);
    accept_requests_ = accept;
}
//---------------------------------------------------------------------
bool DefaultWorkQueue::requests_accepted() const { return accept_requests_; }

//---------------------------------------------------------------------
void DefaultWorkQueue::process_next_request()
{
    std::function<void()> task;
    {
        // scoped to only lock while retrieving the next request
        OGRE_WQ_LOCK_MUTEX(request_mutex_);
        if (tasks_.empty())
            return;

        LogManager::singleton().stream(LogMsgLevel::TRIVIAL)
            << "DefaultWorkQueueBase('" << name_
            << "') - PROCESS_TASK(thread:" << OGRE_THREAD_CURRENT_ID << ")";

        task = std::move(tasks_.front());
        tasks_.pop_front();
    }
    task();
}

//---------------------------------------------------------------------
void DefaultWorkQueue::process_main_thread_tasks()
{
    auto ms_start = Root::singleton().getTimer()->milli_seconds();
    auto ms_current = 0;

    // keep going until we run out of responses or out of time
    while (!main_thread_tasks_.empty()) {
        std::function<void()> task;
        {
            OGRE_WQ_LOCK_MUTEX(response_mutex_);
            LogManager::singleton().stream(LogMsgLevel::TRIVIAL)
                << "DefaultWorkQueueBase('" << name_
                << "') - PROCESS_MAIN_TASK";
            task = std::move(main_thread_tasks_.front());
            main_thread_tasks_.pop_front();
        }
        task();

        // time limit
        if (response_time_limit_ms_) {
            ms_current = Root::singleton().getTimer()->milli_seconds();
            if (ms_current - ms_start > response_time_limit_ms_)
                break;
        }
    }
}

} // namespace Ogre
