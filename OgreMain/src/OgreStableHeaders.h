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

/* Stable headers which will be used for precompilation if the compiler
   supports it. Add entries here when headers are unlikely to change.
   NB: a change to any of these headers will result in a full rebuild,
   so don't add things to this lightly.
*/

#ifndef __OgreStableHeaders__
#define __OgreStableHeaders__

extern "C" {
#   include <sys/types.h>
#   include <sys/stat.h>
}

#include "config.h"
#include "OgreExports.h"
#include "OgrePrerequisites.h"
#include "OgrePlatform.h"
#include "std_header.h"
#include <iomanip>

#include "any.h"
#include "archive.h"
#include "OgreArchiveManager.h"
#include "axis_aligned_box.h"
#include "bitwise.h"
#include "OgreBone.h"
#include "OgreCamera.h"
#include "OgreCodec.h"
#include "colour_value.h"
#include "OgreCommon.h"
#include "data_stream.h"
#include "default_work_queue.h"
#include "exception.h"
#include "file_system.h"
#include "OgreFrustum.h"
#include "OgreHardwareBufferManager.h"
#include "OgreLight.h"
#include "log.h"
#include "log_manager.h"
#include "OgreManualObject.h"
#include "OgreMaterialManager.h"
#include "OgreMaterialSerializer.h"
#include "OgreMath.h"
#include "matrix3.h"
#include "matrix4.h"
#include "OgreMesh.h"
#include "OgreMeshManager.h"
#include "OgreMeshSerializer.h"
#include "OgreMovableObject.h"
#include "OgreNode.h"
#include "OgreParticleSystemManager.h"
#include "OgrePass.h"
#include "plane.h"
#include "OgrePlatformInformation.h"
#include "OgreProfiler.h"
#include "OgreQuaternion.h"
#include "OgreRadixSort.h"
#include "OgreRay.h"
#include "OgreRectangle2D.h"
#include "OgreBuiltinMovableFactories.h"
#include "OgreRenderSystem.h"
#include "OgreResourceGroupManager.h"
#include "resource.h"
#include "OgreRoot.h"
#include "OgreShadowTextureManager.h"
#include "OgreSceneManager.h"
#include "OgreSceneNode.h"
#include "OgreScriptCompiler.h"
#include "OgreSerializer.h"

#include "OgreSimpleRenderable.h"
#include "OgreSimpleSpline.h"
#include "singleton.h"
#include "OgreSkeleton.h"
#include "sphere.h"
#include "string_converter.h"
#include "string_util.h"
#include "string_interface.h"
#include "string_vector.h"
#include "OgreSubMesh.h"
#include "OgreTechnique.h"
#include "OgreTextureManager.h"
#include "threading/thread_header.h"
#include "OgreUserObjectBindings.h"
#include "vector.h"
#if OGRE_NO_ZIP_ARCHIVE == 0
#   include "zip.h"
#endif

#define FOURCC(c0, c1, c2, c3) (c0 | (c1 << 8) | (c2 << 16) | (c3 << 24))

#if OGRE_COMPILER == OGRE_COMPILER_MSVC
#define OGRE_IGNORE_DEPRECATED_BEGIN __pragma(warning(push)) \
    __pragma(warning(disable:4996))
#define OGRE_IGNORE_DEPRECATED_END __pragma(warning(pop))
#else
#define OGRE_IGNORE_DEPRECATED_BEGIN _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
#define OGRE_IGNORE_DEPRECATED_END _Pragma("GCC diagnostic pop")
#endif

#ifndef OGRE_SERIALIZER_VALIDATE_CHUNKSIZE
#define OGRE_SERIALIZER_VALIDATE_CHUNKSIZE OGRE_DEBUG_MODE
#endif

namespace Ogre
{
void logMaterialNotFound(const String& name, const String& groupName, const String& destType, const String& destName,
                         LogMsgLevel lml = LogMsgLevel::CRITICAL);
}

#endif 
