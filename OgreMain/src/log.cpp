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

#include <iostream>

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WINRT
    #include <windows.h>
    #if _WIN32_WINNT >= _WIN32_WINNT_VISTA
        #include <werapi.h>
    #endif
#endif

// LogMsgLevel + LoggingLevel > OGRE_LOG_THRESHOLD = message logged
#define OGRE_LOG_THRESHOLD 4

namespace {
const char* RED = "\x1b[31;1m";
const char* YELLOW = "\x1b[33;1m";
const char* RESET = "\x1b[0m";
} // namespace

namespace Ogre {
//-----------------------------------------------------------------------
Log::Log(const String& name, bool debugger_output, bool suppress_file)
: log_level_(LogMsgLevel::NORMAL)
, debug_output_(debugger_output)
, suppress_file_(suppress_file)
, timestamp_(true)
, log_name_(name)
, term_has_colours_(false)
{
    if (!suppress_file_) {
        log_.open(name.c_str());

#if (                                                                          \
    OGRE_PLATFORM == OGRE_PLATFORM_WIN32                                       \
    || OGRE_PLATFORM == OGRE_PLATFORM_WINRT)                                   \
    && _WIN32_WINNT >= _WIN32_WINNT_VISTA
        // Register log file to be collected by Windows Error Reporting
        const int utf16Length = ::MultiByteToWideChar(
            CP_ACP,
            0,
            name.c_str(),
            (int)name.size(),
            NULL,
            0);
        if (utf16Length > 0) {
            std::wstring wname;
            wname.resize(utf16Length);
            if (0
                != ::MultiByteToWideChar(
                    CP_ACP,
                    0,
                    name.c_str(),
                    (int)name.size(),
                    &wname[0],
                    (int)wname.size()))
                WerRegisterFile(
                    wname.c_str(),
                    WerRegFileTypeOther,
                    WER_FILE_ANONYMOUS_DATA);
        }
#endif
    }

#if OGRE_PLATFORM != OGRE_PLATFORM_WINRT
    char* val = getenv("OGRE_MIN_LOGLEVEL");
    int min_lml;
    if (val && StringConverter::parse(val, min_lml))
        set_min_log_level(LogMsgLevel(min_lml));

    if (debug_output_) {
        val = getenv("TERM");
        term_has_colours_ = val && String(val).find("xterm") != String::npos;
    }
#endif
}
//-----------------------------------------------------------------------
Log::~Log()
{
    if (!suppress_file_) {
        log_.close();
    }
}

//-----------------------------------------------------------------------
void Log::log_message(const String& message, LogMsgLevel lml, bool mask_debug)
{

    if (lml >= log_level_) {
        bool skip_this_msg = false;
        for (auto& l : listenrs_)
            skip_this_msg = l->msg_logged(message, lml, mask_debug, log_name_) || skip_this_msg;

        if (!skip_this_msg) {
            if (debug_output_ && !mask_debug) {
#if (                                                                          \
    OGRE_PLATFORM == OGRE_PLATFORM_WIN32                                       \
    || OGRE_PLATFORM == OGRE_PLATFORM_WINRT)                                   \
    && OGRE_DEBUG_MODE
                OutputDebugStringA("Ogre: ");
                OutputDebugStringA(message.c_str());
                OutputDebugStringA("\n");
#endif

                std::ostream& os
                    = int(lml) >= int(LogMsgLevel::WARNING) ? std::cerr : std::cout;

                if (term_has_colours_) {
                    if (lml == LogMsgLevel::WARNING)
                        os << YELLOW;
                    if (lml == LogMsgLevel::CRITICAL)
                        os << RED;
                }

                os << message;

                if (term_has_colours_) {
                    os << RESET;
                }

                os << std::endl;
            }

            // Write time into log
            if (!suppress_file_) {
                if (timestamp_) {
                    struct tm* p_time;
                    time_t ct_time;
                    time(&ct_time);
                    p_time = localtime(&ct_time);
                    log_ << std::setw(2) << std::setfill('0') << p_time->tm_hour
                         << ":" << std::setw(2) << std::setfill('0')
                         << p_time->tm_min << ":" << std::setw(2)
                         << std::setfill('0') << p_time->tm_sec << ": ";
                }
                log_ << message << std::endl;

                // Flush stcmdream to ensure it is written (incase of a crash,
                // we need log to be up to date)
                log_.flush();
            }
        }
    }
}

//-----------------------------------------------------------------------
void Log::set_timestamp_enabled(bool enabled) { timestamp_ = enabled; }

//-----------------------------------------------------------------------
void Log::set_debug_output_enabled(bool enabled) { debug_output_ = enabled; }

void Log::set_min_log_level(LogMsgLevel lml) { log_level_ = lml; }

//-----------------------------------------------------------------------
void Log::add_listener(LogListener* listener)
{
    auto iter = std::find(listenrs_.begin(), listenrs_.end(), listener);
    if (iter == listenrs_.end())
        listenrs_.push_back(listener);
}

//-----------------------------------------------------------------------
void Log::remove_listener(LogListener* listener)
{
    auto i = std::find(listenrs_.begin(), listenrs_.end(), listener);
    if (i != listenrs_.end())
        listenrs_.erase(i);
}
//---------------------------------------------------------------------
Log::Stream Log::stream(LogMsgLevel lml, bool mask_debug)
{
    return Stream(this, lml, mask_debug);
}
} // namespace Ogre
