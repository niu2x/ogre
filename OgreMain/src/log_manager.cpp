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

namespace Ogre {

    //-----------------------------------------------------------------------
    template<> LogManager* Singleton<LogManager>::msSingleton = 0;
    LogManager* LogManager::singleton_ptr((void)
    {
        return msSingleton;
    }
    LogManager& LogManager::singleton(void)
    {  
        assert( msSingleton );  return ( *msSingleton );  
    }
    //-----------------------------------------------------------------------
    LogManager::LogManager()
    {
        default_log_ = NULL;
    }
    //-----------------------------------------------------------------------
    LogManager::~LogManager()
    {
        for(auto &x: logs_) {
            OGRE_DELETE x.second;
        }
    }
    //-----------------------------------------------------------------------
    Log* LogManager::create_log( const String& name, bool default_log, bool debugger_output, 
        bool suppress_file_output)
    {
        Log* new_log = OGRE_NEW Log(name, debugger_output, suppress_file_output);

        if( !default_log_ || default_log )
        {
            default_log_ = new_log;
        }

        logs_.emplace(name, new_log);

        return new_log;
    }
    //-----------------------------------------------------------------------
    Log* LogManager::default_log()
    {
        
        return default_log_;
    }
    //-----------------------------------------------------------------------
    Log* LogManager::set_default_log(Log* new_log)
    {
        Log* old_log = default_log_;
        default_log_ = new_log;
        return old_log;
    }
    //-----------------------------------------------------------------------
    Log* LogManager::log( const String& name)
    {
        LogList::iterator i = logs_.find(name);
        OgreAssert(i != logs_.end(), "Log not found");
        return i->second;
    }
    //-----------------------------------------------------------------------
    void LogManager::destroy_log(const String& name)
    {
        LogList::iterator i = logs_.find(name);
        if (i != logs_.end())
        {
            if (default_log_ == i->second)
            {
                default_log_ = nullptr;
            }
            OGRE_DELETE i->second;
            logs_.erase(i);
        }

        // Set another default log if this one removed
        if (!default_log_ && !logs_.empty())
        {
            default_log_ = logs_.begin()->second;
        }
    }
    //-----------------------------------------------------------------------
    void LogManager::destroy_log(Log* log)
    {
        OgreAssert(log, "Cannot destroy a null log");
        destroy_log(log->name());
    }
    //-----------------------------------------------------------------------
    void LogManager::log_message( const String& message, LogMsgLevel lml, bool maskDebug)
    {
        
        if (default_log_)
        {
            default_log_->log_message(message, lml, maskDebug);
        }
    }

    void LogManager::log_error(const String& message, bool maskDebug )
    {
        stream(LogMsgLevel::CRITICAL, maskDebug) << "Error: " << message;
    }

    void LogManager::log_warning(const String& message, bool maskDebug )
    {
        stream(LogMsgLevel::WARNING, maskDebug) << "Warning: " << message;
    }
    //-----------------------------------------------------------------------
    void LogManager::set_min_log_level(LogMsgLevel lml)
    {
        
        if (default_log_)
        {
            default_log_->set_min_log_level(lml);
        }
    }
    //---------------------------------------------------------------------
    Log::Stream LogManager::stream(LogMsgLevel lml, bool maskDebug)
    {
        OgreAssert(default_log_, "Default log not found");
        return default_log_->stream(lml, maskDebug);
    }
}
