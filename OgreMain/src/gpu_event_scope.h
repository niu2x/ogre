#pragma once

#include "OgreRoot.h"

#if OGRE_PROFILING == 1
#define OgreGpuEventScope(name) GpuEventScope _gpuEventScope(name)
#else
#define OgreGpuEventScope(name)
#endif

namespace Ogre
{

struct GpuEventScope
{
    GpuEventScope(const String& name) { Root::getSingleton().getRenderSystem()->beginProfileEvent(name); }
    ~GpuEventScope() { Root::getSingleton().getRenderSystem()->endProfileEvent(); }
};

} // namespace Ogre
