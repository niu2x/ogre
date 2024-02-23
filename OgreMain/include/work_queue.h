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
#ifndef __OgreWorkQueue_H__
#define __OgreWorkQueue_H__

#include "OgrePrerequisites.h"
#include "any.h"

#include "OgreCommon.h"
#include "threading/thread_header.h"
#include "OgreHeaderPrefix.h"

#include <deque>
#include <functional>

namespace Ogre {
/** \addtogroup Core
 *  @{
 */
/** \addtogroup General
 *  @{
 */

/** Interface to a general purpose task-basedbackground work queue.

    A work queue is a simple structure, where tasks of work are placed
    onto the queue, then removed by a worker for processing.
    The typical use for this is in a threaded environment,
    although any kind of deferred processing could use this approach to
    decouple and distribute work over a period of time even
    if it was single threaded.

    WorkQueues also incorporate thread pools. One or more background worker
   threads can wait on the queue and be notified when a request is waiting to be
    processed. For maximal thread usage, a WorkQueue instance should be shared
    among many sources of work, rather than many work queues being created.
    This way, you can share a small number of hardware threads among a large
    number of background tasks. This doesn't mean you have to implement all the
    request processing in one class, you can plug in many handlers in order to
    process the tasks.

    This is an abstract interface definition; users can subclass this and
    provide their own implementation if required to centralise task management
    in their own subsystems. We also provide a default implementation in the
    form of DefaultWorkQueue.
*/
class WorkQueue {
public:
    /// Numeric identifier for a request
    using RequestID = uint64_t;

    /** General purpose request structure.
     */
    class Request {
    public:
        /// Constructor
        Request(
            uint16_t channel,
            uint16_t rtype,
            const Any& rData,
            uint8_t retry,
            RequestID rid);

        /// Get the request channel (top level categorisation)
        uint16_t channel() const { return channel_; }
        /// Get the type of this request within the given channel
        uint16_t type() const { return type_; }
        /// Get the user details of this request
        const Any& data() const { return data_; }
        /// Get the remaining retry count
        uint8_t retry_count() const { return retry_count_; }
        /// Get the identifier of this request
        RequestID id() const { return id_; }

    private:
        /// The request channel, as an integer
        uint16_t channel_;
        /// The request type, as an integer within the channel (user can define
        /// enumerations on this)
        uint16_t type_;
        /// The details of the request (user defined)
        Any data_;
        /// Retry count - set this to non-zero to have the request try again on
        /// failure
        uint8_t retry_count_;
        /// Identifier (assigned by the system)
        RequestID id_;
        /// Abort Flag
    };

    /** General purpose response structure.
     */
    class Response {

    public:
        Response(
            UniquePtr<const Request>&& rq,
            bool success,
            const Any& data,
            const String& msg = BLANKSTRING);
        ~Response();
        /// Get the request that this is a response to (NB destruction destroys
        /// this)
        const Request* request() const { return request_.get(); }
        /// Return whether this is a successful response
        bool succeeded() const { return success_; }
        /// Get any diagnostic messages about the process
        const String& message() const { return message_; }
        /// Return the response data (user defined, only valid on success)
        const Any& data() const { return data_; }

    private:
        /// Pointer to the request that this response is in relation to
        UniquePtr<const Request> request_;
        /// Whether the work item succeeded or not
        bool success_;
        /// Any diagnostic messages
        String message_;
        /// Data associated with the result of the process
        Any data_;
    };
    WorkQueue() { }
    virtual ~WorkQueue() { }

    /** Get the number of worker threads that this queue will start when
        startup() is called.
    */
    virtual size_t worker_thread_count() const { return 1; }

    /** Set the number of worker threads that this queue will start
        when startup() is called (default 1).
        Calling this will have no effect unless the queue is shut down and
        restarted.
    */
    virtual void set_worker_thread_count(size_t c) { }

    /** Start up the queue with the options that have been set.
    @param force_restart If the queue is already running, whether to shut it
        down and restart.
    */
    virtual void startup(bool force_restart) = 0;

    /** Add a new task to the queue */
    virtual void add_task(std::function<void()> task) = 0;

    /** Set whether to accept new requests or not.
    If true, requests are added to the queue as usual. If false, requests
    are silently ignored until setRequestsAccepted(true) is called.
    */
    virtual void set_requests_accepted(bool accept) = 0;
    /// Returns whether requests are being accepted right now
    virtual bool requests_accepted() const = 0;

    /** Process the tasks in the main-thread queue.

        This method must be called from the main render
        thread to 'pump' tasks through the system. The method will usually
        try to clear all tasks before returning; however, you can specify
        a time limit on the tasks processing to limit the impact of
        spikes in demand by calling @ref setMainThreadProcessingTimeLimit.
    */
    virtual void process_main_thread_tasks() = 0;

    /** Get the time limit imposed on the processing of tasks in a
        single frame, in milliseconds (0 indicates no limit).
    */
    virtual uint64_t main_thread_processing_time_limit() const = 0;

    /** Set the time limit imposed on the processing of tasks in a
        single frame, in milliseconds (0 indicates no limit).
        This sets the maximum time that will be spent in @ref
       process_main_thread_tasks() in a single frame. The default is 10ms.
    */
    virtual void set_main_thread_processing_time_limit(uint64_t ms) = 0;

    /** Add a deferred task that will be processed on the main render thread */
    virtual void add_main_thread_task(std::function<void()> task) = 0;

    /** Shut down the queue.
     */
    virtual void shutdown() = 0;

    virtual bool is_shutting_down() const = 0;
};

/** @} */
/** @} */

} // namespace Ogre

#include "OgreHeaderSuffix.h"

#endif
