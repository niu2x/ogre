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

#ifndef __LogManager_H__
#define __LogManager_H__

#include "OgrePrerequisites.h"

#include "log.h"
#include "OgreSingleton.h"
#include "OgreHeaderPrefix.h"

namespace Ogre
{
    /** \addtogroup Core
    *  @{
    */
    /** \addtogroup General
    *  @{
    */
    /** The log manager handles the creation and retrieval of logs for the
        application.

            This class will create new log files and will retrieve instances
            of existing ones. Other classes wishing to log output can either
            create a fresh log or retrieve an existing one to output to.
            One log is the default log, and is the one written to when the
            logging methods of this class are called.
        @par
            By default, Root will instantiate a LogManager (which becomes the 
            Singleton instance) on construction, and will create a default log
            based on the Root construction parameters. If you want more control,
            for example redirecting log output right from the start or suppressing
            debug output, you need to create a LogManager yourself before creating
            a Root instance, then create a default log. Root will detect that 
            you've created one yourself and won't create one of its own, thus
            using all your logging preferences from the first instance.
    */
    class _OgreExport LogManager : public Singleton<LogManager>
    {
    private:
        using LogList = std::map<String, Log*>;

        /// A list of all the logs the manager can access
        LogList logs_;

        /// The default log to which output is done
        Log* default_log_;

    public:

        LogManager();
        ~LogManager();

        /** Creates a new log with the given name.
            @param
                name The name to give the log e.g. 'Ogre.log'
            @param
                defaultLog If true, this is the default log output will be
                sent to if the generic logging methods on this class are
                used. The first log created is always the default log unless
                this parameter is set.
            @param
                debuggerOutput If true, output to this log will also be
                routed to the debugger's output window.
            @param
                suppressFileOutput If true, this is a logical rather than a physical
                log and no file output will be written. If you do this you should
                register a LogListener so log output is not lost.
        */
        Log* create_log( const String& name, bool default_log = false, bool debugger_output = true, 
            bool suppress_file_output = false);

        /** Retrieves a log managed by this class.
        */
        Log* log( const String& name);

        /** Returns a pointer to the default log.
        */
        Log* default_log();

        /** Closes and removes a named log. */
        void destroy_log(const String& name);
        /** Closes and removes a log. */
        void destroy_log(Log* log);

        /** Sets the passed in log as the default log.
        @return The previous default log.
        */
        Log* set_default_log(Log* newLog);

        /** Log a message to the default log.
        */
        void log_message( const String& message, LogMsgLevel lml = LogMsgLevel::NORMAL, 
            bool mask_debug = false);

        /// @overload
        void log_error(const String& message, bool mask_debug = false );
        /// @overload
        void log_warning(const String& message, bool mask_debug = false );

        /** Log a message to the default log (signature for backward compatibility).
        */
        void log_message( LogMsgLevel lml, const String& message,  
            bool mask_debug = false) { log_message(message, lml, mask_debug); }

        /** Get a stream on the default log. */
        Log::Stream stream(LogMsgLevel lml = LogMsgLevel::NORMAL, 
            bool mask_debug = false);

        /// sets the minimal #LogMsgLevel for the default log
        void set_min_log_level(LogMsgLevel lml);
        /// @copydoc Singleton::getSingleton()
        static LogManager& getSingleton(void);
        /// @copydoc Singleton::getSingleton()
        static LogManager* getSingletonPtr(void);

    };


    /** @} */
    /** @} */
}

#include "OgreHeaderSuffix.h"

#endif
