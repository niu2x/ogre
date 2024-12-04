#pragma once
#include "OgrePrerequisites.h"

#if OGRE_COMPILER == OGRE_COMPILER_MSVC
#define OGRE_IGNORE_DEPRECATED_BEGIN __pragma(warning(push)) __pragma(warning(disable : 4996))
#define OGRE_IGNORE_DEPRECATED_END __pragma(warning(pop))
#else
#define OGRE_IGNORE_DEPRECATED_BEGIN                                                                                   \
    _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
#define OGRE_IGNORE_DEPRECATED_END _Pragma("GCC diagnostic pop")
#endif
