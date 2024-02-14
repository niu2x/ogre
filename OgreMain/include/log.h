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

#ifndef __Log_H__
#define __Log_H__

#include "OgrePrerequisites.h"
#include "OgreCommon.h"
#include "threading/thread_header.h"
#include "OgreHeaderPrefix.h"

#include <fstream>
#include <sstream>

namespace Ogre {

/** \addtogroup Core
 *  @{
 */
/** \addtogroup General
 *  @{
 */

/** The importance of a logged message.
 */

enum class LogMsgLevel { TRIVIAL = 1, NORMAL = 2, WARNING = 3, CRITICAL = 4 };

/** @remarks Pure Abstract class, derive this class and register to the Log to
 * listen to log messages */
class LogListener {
public:
    virtual ~LogListener() { }

    /**

        This is called whenever the log receives a message and is about to write
    it out
    @param msg
        The message to be logged
    @param lml
        The message level the log is using
    @param mask_debug
        If we are printing to the console or not
    @param log_name
        The name of this log (so you can have several listeners for different
    logs, and identify them)
    @param skipThisMessage
        If set to true by the message_logged() implementation message will not
    be logged
    */
    virtual bool msg_logged(
        const String& msg,
        LogMsgLevel lml,
        bool mask_debug,
        const String& log_name)
        = 0;
};

/**
    Log class for writing debug/log data to files.

    You can control the default log level through the `OGRE_MIN_LOGLEVEL`
   environment variable. Here, the value 1 corresponds to #LogMsgLevel::TRIVIAL etc.
    @note Should not be used directly, but through the LogManager class.
*/

class Log {

public:
    class Stream;

    /**

        Usual constructor - called by LogManager.
    */
    Log(const String& name,
        bool debug_output = true,
        bool suppress_file_output = false);

    /**

    Default destructor.
    */
    ~Log();

    /// Return the name of the log
    const String& name() const { return log_name_; }
    /// Get whether debug output is enabled for this log
    bool debug_output_enabled() const { return debug_output_; }
    /// Get whether file output is suppressed for this log
    bool file_output_suppressed() const { return suppress_file_; }
    /// Get whether time stamps are printed for this log
    bool timestamp_enabled() const { return timestamp_; }

    /** Log a message to the debugger and to log file (the default is
        "<code>OGRE.log</code>"),
    */
    void log_message(
        const String& message,
        LogMsgLevel lml = LogMsgLevel::NORMAL,
        bool mask_debug = false);

    /** Get a stream object targeting this log. */
    Stream
    stream(LogMsgLevel lml = LogMsgLevel::NORMAL, bool mask_debug = false);

    /**

        Enable or disable outputting log messages to the debugger.
    */
    void set_debug_output_enabled(bool debugOutput);

    /// set the minimal #LogMsgLevel for a message to be logged
    void set_min_log_level(LogMsgLevel lml);
    /**

        Enable or disable time stamps.
    */
    void set_timestamp_enabled(bool timeStamp);
    /** Gets the level of the log detail.
     */
    LogMsgLevel min_log_level() const { return log_level_; }
    /**

        Register a listener to this log
    @param listener
        A valid listener derived class
    */
    void add_listener(LogListener* listener);

    /**

        Unregister a listener from this log
    @param listener
        A valid listener derived class
    */
    void remove_listener(LogListener* listener);

    /** Stream object which targets a log.

        A stream logger object makes it simpler to send various things to
        a log. You can just use the operator<< implementation to stream
        anything to the log, which is cached until a Stream::Flush is
        encountered, or the stream itself is destroyed, at which point the
        cached contents are sent to the underlying log. You can use
    Log::stream() directly without assigning it to a local variable and as soon
    as the streaming is finished, the object will be destroyed and the message
        logged.
    @par
        You can stream control operations to this object too, such as
        std::setw() and std::setfill() to control formatting.
    @note
        Each Stream object is not thread safe, so do not pass it between
        threads. Multiple threads can hold their own Stream instances pointing
        at the same Log though and that is threadsafe.
    */
    class Stream {
    public:
        /// Simple type to indicate a flush of the stream to the log
        struct Flush {
        };

        Stream(Log* target, LogMsgLevel lml, bool mask_debug)
        : target_(target)
        , level_(lml)
        , mask_debug_(mask_debug)
        {
        }
        // move constructor
        Stream(Stream&& rhs) = default;

        ~Stream()
        {
            // flush on destroy
            if (cache_.tellp() > 0) {
                target_->log_message(cache_.str(), level_, mask_debug_);
            }
        }

        template <typename T>
        Stream& operator<<(const T& v)
        {
            cache_ << v;
            return *this;
        }

        Stream& operator<<(const Flush& v)
        {
            (void)v;
            target_->log_message(cache_.str(), level_, mask_debug_);
            cache_.str(BLANKSTRING);
            return *this;
        }

    private:
        Log* target_;
        LogMsgLevel level_;
        bool mask_debug_;
        using BaseStream = StringStream;
        BaseStream cache_;
    };

private:
    std::ofstream log_;
    LogMsgLevel log_level_;
    bool debug_output_;
    bool suppress_file_;
    bool timestamp_;
    String log_name_;
    bool term_has_colours_;

    using LogListenerList = std::vector<LogListener*>;
    LogListenerList listenrs_;
};

/** @} */
/** @} */
} // namespace Ogre

#include "OgreHeaderSuffix.h"

#endif
