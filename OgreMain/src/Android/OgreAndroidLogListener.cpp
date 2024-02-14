#include "Android/OgreAndroidLogListener.h"
#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "OGRE", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "OGRE", __VA_ARGS__))

namespace Ogre
{
    bool AndroidLogListener::messageLogged(const String& message, LogMsgLevel lml, bool maskDebug, const String &logName, bool& skipThisMessage )
    {
        if(lml < Ogre::LogMsgLevel::CRITICAL)
        {
            LOGI("%s", message.c_str());
        }
        else
        {
            LOGE("%s", message.c_str());
        }
        return false;
    }
}
