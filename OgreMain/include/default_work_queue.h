/*-------------------------------------------------------------------------
This source file is a part of OGRE
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
THE SOFTWARE
-------------------------------------------------------------------------*/
#ifndef __OgreDefaultWorkQueue_H__
#define __OgreDefaultWorkQueue_H__

#include "work_queue.h"

namespace Ogre {
/** Implementation of a general purpose request / response style background work
   queue.

    This default implementation of a work queue starts a thread pool and
    provides queues to process requests.
*/
class DefaultWorkQueue : public WorkQueue {
public:
    DefaultWorkQueue(const String& name = BLANKSTRING);
    ~DefaultWorkQueue();

    /// @copydoc WorkQueue::shutdown
    void shutdown() override;

    /// @copydoc WorkQueue::startup
    void startup(bool force_restart) override;

    /// Get the name of the work queue
    const String& name() const { return name_; }

    /** Get whether worker threads will be allowed to access render system
        resources.
        Accessing render system resources from a separate thread can require
       that a context is maintained for that thread. Also, it requires that the
        render system is running in threadsafe mode, which only happens
        when OGRE_THREAD_SUPPORT=1. This option defaults to false, which means
        that threads can not use GPU resources, and the render system can
        work in non-threadsafe mode, which is more efficient.
    */
    // virtual bool getWorkersCanAccessRenderSystem() const;

    /** Set whether worker threads will be allowed to access render system
        resources.
        Accessing render system resources from a separate thread can require
       that a context is maintained for that thread. Also, it requires that the
        render system is running in threadsafe mode, which only happens
        when OGRE_THREAD_SUPPORT=1. This option defaults to false, which means
        that threads can not use GPU resources, and the render system can
        work in non-threadsafe mode, which is more efficient.
        Calling this will have no effect unless the queue is shut down and
        restarted.
    */
    // virtual void setWorkersCanAccessRenderSystem(bool access);

    /** Process the next request on the queue.

        This method is public, but only intended for advanced users to call.
        The only reason you would call this, is if you were using your
        own thread to drive the worker processing. The thread calling this
        method will be the thread used to call the RequestHandler.
    */
    void process_next_request();

    /// @copydoc WorkQueue::setRequestsAccepted
    void set_requests_accepted(bool accept) override;
    /// @copydoc WorkQueue::getRequestsAccepted
    virtual bool requests_accepted() const override;

    void process_main_thread_tasks() override;
    /// @copydoc WorkQueue::getResponseProcessingTimeLimit
    uint64_t main_thread_processing_time_limit() const override
    {
        return response_time_limit_ms_;
    }
    /// @copydoc WorkQueue::setResponseProcessingTimeLimit
    void set_main_thread_processing_time_limit(uint64_t ms) override
    {
        response_time_limit_ms_ = ms;
    }

    size_t worker_thread_count() const override { return worker_thread_count_; }

    void set_worker_thread_count(size_t c) override
    {
        worker_thread_count_ = c;
    }

    void add_main_thread_task(std::function<void()> task) override;

    void add_task(std::function<void()> task) override;

    bool is_shutting_down() const override { return shutting_down_; }

private:
    /** To be called by a separate thread; will return immediately if there
        are items in the queue, or suspend the thread until new items are added
        otherwise.
    */
    void wait_for_next_request();

    void notify_workers();

    /// Main function for each thread spawned.
    void thread_main();

    std::deque<std::function<void()>> tasks_;
    std::deque<std::function<void()>> main_thread_tasks_;

    OGRE_WQ_MUTEX(request_mutex_);
    OGRE_WQ_MUTEX(response_mutex_);

    OGRE_WQ_THREAD_SYNCHRONISER(request_cond_);

#if XDOG_USE_THREAD
    using WorkerThreadList = std::vector<OGRE_THREAD_TYPE*>;
    WorkerThreadList workers_;
#endif

    String name_;
    size_t worker_thread_count_;
    uint64_t response_time_limit_ms_;

    bool accept_requests_;
    bool shutting_down_;

    bool running_;
};

} // namespace Ogre

#endif
