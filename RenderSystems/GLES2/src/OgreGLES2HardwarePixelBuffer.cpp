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

#include "OgreGLES2HardwareBufferManager.h"
#include "OgreGLES2HardwarePixelBuffer.h"

#include "OgreTextureManager.h"

#include "OgreGLES2PixelFormat.h"
#include "OgreGLES2FBORenderTexture.h"
#include "OgreGLUtil.h"
#include "OgreGLES2RenderSystem.h"
#include "OgreGLES2StateCacheManager.h"
#include "OgreRoot.h"
#include "OgreGLSLESProgramManager.h"
#include "OgreGLSLESLinkProgram.h"
#include "OgreGLSLESProgramPipeline.h"
#include "bitwise.h"
#include "OgreGLNativeSupport.h"
#include "OgreGLES2HardwareBuffer.h"
#include "OgreGLES2Texture.h"
#include "log_manager.h"

namespace Ogre {
    void GLES2TextureBuffer::_blitFromMemory(const PixelBox &src, const Box &dst)
    {
        if (!mBuffer.contains(dst))
        {
            OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
                        "Destination box out of range",
                        "GLES2HardwarePixelBuffer::blitFromMemory");
        }

        PixelBox converted;

        if (src.format != mFormat)
        {
            // Extents match, but format is not accepted as valid source format for GL
            // do conversion in temporary buffer
            allocateBuffer();
            converted = mBuffer.get_sub_volume(src);
            PixelUtil::bulk_pixel_conversion(src, converted);
        }
        else
        {
            // No conversion needed
            converted = src;
        }

        upload(converted, dst);
        freeBuffer();
    }

    void GLES2TextureBuffer::blitToMemory(const Box &srcBox, const PixelBox &dst)
    {
        if (!mBuffer.contains(srcBox))
        {
            OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
                        "source box out of range",
                        "GLES2HardwarePixelBuffer::blitToMemory");
        }

        if (srcBox.origin() == Vector3i(0, 0, 0) && srcBox.size() == size()
            && dst.size() == size()
            && GLES2PixelUtil::getGLInternalFormat(dst.format) != 0) {
            // The direct case: the user wants the entire texture in a format supported by GL
            // so we don't need an intermediate buffer
            download(dst);
        } else {
            // Use buffer for intermediate copy
            allocateBuffer();
            // Download entire buffer
            download(mBuffer);
            if (srcBox.size() != dst.size()) {
                // We need scaling
                Image::scale(
                    mBuffer.get_sub_volume(srcBox),
                    dst,
                    Image::FILTER_BILINEAR);
            } else {
                // Just copy the bit that we need
                PixelUtil::bulk_pixel_conversion(
                    mBuffer.get_sub_volume(srcBox),
                    dst);
            }
            freeBuffer();
        }
    }
    
    // TextureBuffer
    GLES2TextureBuffer::GLES2TextureBuffer(GLES2Texture* parent, GLint face, GLint level,
                                           GLint width, GLint height, GLint depth)
        : GLHardwarePixelBufferCommon(width, height, depth, parent->getFormat(), (Usage)parent->getUsage()),
          mTarget(parent->getGLES2TextureTarget()), mTextureID(parent->getGLID()),
          mLevel(level)
    {
        // Get face identifier
        mFaceTarget = mTarget;
        if(mTarget == GL_TEXTURE_CUBE_MAP)
            mFaceTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X + face;

        mGLInternalFormat =
            GLES2PixelUtil::getGLInternalFormat(mFormat, parent->isHardwareGammaEnabled());

        // Set up a pixel box
        mBuffer = PixelBox(mWidth, mHeight, mDepth, mFormat);
        
        if (mWidth==0 || mHeight==0 || mDepth==0)
            // We are invalid, do not allocate a buffer
            return;

        // Is this a render target?
        if (mUsage & TU_RENDERTARGET)
        {
            // Create render target for each slice
            mSliceTRT.reserve(mDepth);
            for(uint32 zoffset=0; zoffset<mDepth; ++zoffset)
            {
                String name;
                name = "rtt/" + StringConverter::to_string((size_t)this) + "/" + parent->name();
                GLSurfaceDesc surface;
                surface.buffer = this;
                surface.zoffset = zoffset;
                RenderTexture* trt
                    = GLRTTManager::singleton().createRenderTexture(
                        name,
                        surface,
                        parent->isHardwareGammaEnabled(),
                        parent->getFSAA());
                mSliceTRT.push_back(trt);
                Root::singleton().getRenderSystem()->attachRenderTarget(
                    *mSliceTRT[zoffset]);
            }
        }
    }

    GLES2TextureBuffer::~GLES2TextureBuffer()
    {
    }

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID || OGRE_PLATFORM == OGRE_PLATFORM_EMSCRIPTEN
    void GLES2TextureBuffer::updateTextureId(GLuint textureID)
    {
        mTextureID = textureID;
    }
#endif

    void GLES2TextureBuffer::upload(const PixelBox &data, const Box &dest)
    {
        GLES2RenderSystem* rs = getGLES2RenderSystem();

        rs->_getStateCacheManager()->bindGLTexture(mTarget, mTextureID);

        bool hasGLES30 = rs->hasMinGLVersion(3, 0);
        // PBO handling is broken
#if 0// OGRE_NO_GLES3_SUPPORT == 0
        // Calculate size for all mip levels of the texture
        size_t dataSize = 0;
        if(mTarget == GL_TEXTURE_2D_ARRAY)
        {
            dataSize = PixelUtil::get_memory_size(dest.width(), dest.height(), dest.depth(), data.format);
        }
        else
        {
            dataSize = PixelUtil::get_memory_size(data.width(), data.height(), mDepth, data.format);
        }

        // Upload data to PBO
        GLES2HardwareBuffer buffer(GL_PIXEL_UNPACK_BUFFER, dataSize, mUsage);
        buffer.writeData(0, dataSize, data.data, false);

        void* pdata = NULL;
#else
        void* pdata = data.get_top_left_front_pixel_ptr();
#endif

        if (PixelUtil::is_compressed(data.format)) {
            if (data.format != mFormat || !data.is_consecutive())
                OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
                            "Compressed images must be consecutive, in the source format",
                            "GLES2TextureBuffer::upload");

            GLenum format = GLES2PixelUtil::getGLInternalFormat(mFormat);
            // Data must be consecutive and at beginning of buffer as PixelStorei not allowed
            // for compressed formats
            switch(mTarget) {
                case GL_TEXTURE_2D:
                case GL_TEXTURE_CUBE_MAP:
                    OGRE_CHECK_GL_ERROR(glCompressedTexSubImage2D(
                        mFaceTarget,
                        mLevel,
                        dest.left,
                        dest.top,
                        dest.width(),
                        dest.height(),
                        format,
                        data.get_consecutive_size(),
                        pdata));
                    break;
                case GL_TEXTURE_2D_ARRAY:
                    if(!hasGLES30)
                        break;
                    OGRE_FALLTHROUGH;
                case GL_TEXTURE_3D_OES:
                    OGRE_CHECK_GL_ERROR(glCompressedTexSubImage3DOES(
                        mTarget,
                        mLevel,
                        dest.left,
                        dest.top,
                        dest.front,
                        dest.width(),
                        dest.height(),
                        dest.depth(),
                        format,
                        data.get_consecutive_size(),
                        pdata));
                    break;
            }
        } else {
            if (data.width() != data.row_pitch) {
                if(!hasGLES30)
                    OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
                                "Unsupported texture format",
                                "GLES2TextureBuffer::upload");

                OGRE_CHECK_GL_ERROR(
                    glPixelStorei(GL_UNPACK_ROW_LENGTH, data.row_pitch))
            }

            if (data.height() * data.width() != data.slice_pitch) {
                if(!hasGLES30)
                    OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
                                "Unsupported texture format",
                                "GLES2TextureBuffer::upload");
                OGRE_CHECK_GL_ERROR(glPixelStorei(
                    GL_UNPACK_IMAGE_HEIGHT,
                    (data.slice_pitch / data.width())));
            }

            if ((data.width() * PixelUtil::get_num_elem_bytes(data.format))
                & 3) {
                // Standard alignment of 4 is not right
                OGRE_CHECK_GL_ERROR(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
            }

            switch(mTarget) {
                case GL_TEXTURE_2D:
                case GL_TEXTURE_CUBE_MAP:
                    OGRE_CHECK_GL_ERROR(glTexSubImage2D(mFaceTarget, mLevel,
                                    dest.left, dest.top,
                                    dest.width(), dest.height(),
                                    GLES2PixelUtil::getGLOriginFormat(data.format), GLES2PixelUtil::getGLOriginDataType(data.format),
                                    pdata));
                    break;
                case GL_TEXTURE_2D_ARRAY:
                    if(!hasGLES30)
                        break;
                    OGRE_FALLTHROUGH;
                case GL_TEXTURE_3D_OES:
                    OGRE_CHECK_GL_ERROR(glTexSubImage3DOES(
                                    mTarget, mLevel,
                                    dest.left, dest.top, dest.front,
                                    dest.width(), dest.height(), dest.depth(),
                                    GLES2PixelUtil::getGLOriginFormat(data.format), GLES2PixelUtil::getGLOriginDataType(data.format),
                                    pdata));
                    break;
            }

            // TU_AUTOMIPMAP is only enabled when there are no custom mips
            // so we do not have to care about overwriting
            if ((mUsage & TU_AUTOMIPMAP) && (mLevel == 0)
                    && (hasGLES30 || mTarget == GL_TEXTURE_2D || mTarget == GL_TEXTURE_CUBE_MAP))
            {
                OGRE_CHECK_GL_ERROR(glGenerateMipmap(mTarget));
            }
        }

        // Restore defaults
        if(hasGLES30) {
            OGRE_CHECK_GL_ERROR(glPixelStorei(GL_UNPACK_ROW_LENGTH, 0));
            OGRE_CHECK_GL_ERROR(glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, 0));
        }

        OGRE_CHECK_GL_ERROR(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));
    }

    //-----------------------------------------------------------------------------  
    void GLES2TextureBuffer::download(const PixelBox &data)
    {
        if (data.size() != size())
            OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "only download of entire buffer is supported by GL ES",
                        "GLES2TextureBuffer::download");

        if (PixelUtil::is_compressed(data.format)) {
            OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
                        "Compressed images cannot be downloaded by GL ES",
                        "GLES2TextureBuffer::download");
        }

        if ((data.width() * PixelUtil::get_num_elem_bytes(data.format)) & 3) {
            // Standard alignment of 4 is not right
            OGRE_CHECK_GL_ERROR(glPixelStorei(GL_PACK_ALIGNMENT, 1));
        }

        GLint currentFBO = 0;
        GLuint tempFBO = 0;
        OGRE_CHECK_GL_ERROR(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFBO));
        OGRE_CHECK_GL_ERROR(glGenFramebuffers(1, &tempFBO));
        OGRE_CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, tempFBO));

        // Construct a temp PixelBox that is RGBA because GL_RGBA/GL_UNSIGNED_BYTE is the only combination that is
        // guaranteed to work on all platforms.
        size_t sizeInBytes = PixelUtil::get_memory_size(
            data.width(),
            data.height(),
            data.depth(),
            PixelFormat::A8B8G8R8);
        PixelBox tempBox = PixelBox(
            data.width(),
            data.height(),
            data.depth(),
            PixelFormat::A8B8G8R8);
        tempBox.data = new uint8[sizeInBytes];

        switch (mTarget)
        {
            case GL_TEXTURE_2D:
            case GL_TEXTURE_CUBE_MAP:
                OGRE_CHECK_GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureID, 0));
                OGRE_CHECK_GL_ERROR(glCheckFramebufferStatus(GL_FRAMEBUFFER));
                OGRE_CHECK_GL_ERROR(glReadPixels(0, 0, data.width(), data.height(),
                                                 GL_RGBA,
                                                 GL_UNSIGNED_BYTE,
                                                 tempBox.data));
                break;
        }

        PixelUtil::bulk_pixel_conversion(tempBox, data);

        delete[] tempBox.data;
        tempBox.data = 0;

        // Restore defaults
        OGRE_CHECK_GL_ERROR(glPixelStorei(GL_PACK_ALIGNMENT, 4));
        OGRE_CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, currentFBO));
        OGRE_CHECK_GL_ERROR(glDeleteFramebuffers(1, &tempFBO));
    }

    //-----------------------------------------------------------------------------  
    void GLES2TextureBuffer::bindToFramebuffer(uint32 attachment, uint32 zoffset)
    {
        assert(zoffset < mDepth);
        OGRE_CHECK_GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment,
                                                   mFaceTarget, mTextureID, mLevel));
    }
    
    void GLES2TextureBuffer::copyFromFramebuffer(size_t zoffset)
    {
        getGLES2RenderSystem()->_getStateCacheManager()->bindGLTexture(mTarget, mTextureID);
        OGRE_CHECK_GL_ERROR(glCopyTexSubImage2D(mFaceTarget, mLevel, 0, 0, 0, 0, mWidth, mHeight));
    }

    //-----------------------------------------------------------------------------  
    void GLES2TextureBuffer::blit(const HardwarePixelBufferSharedPtr &src, const Box &srcBox, const Box &dstBox)
    {
        GLES2TextureBuffer *srct = static_cast<GLES2TextureBuffer *>(src.get());
        if ((srcBox.width() == dstBox.width() && srcBox.height() == dstBox.height() &&
             srcBox.depth() == 1))
        {
            blitFromTexture(srct, srcBox, dstBox);
        }
        else
        {
            GLHardwarePixelBufferCommon::blit(src, srcBox, dstBox);
        }
    }
    
    //-----------------------------------------------------------------------------  
    void GLES2TextureBuffer::blitFromTexture(GLES2TextureBuffer *src, const Box &srcBox, const Box &dstBox)
    {
        GLES2RenderSystem* rs = getGLES2RenderSystem();

        // Store old binding so it can be restored later
        GLint oldfb;
        OGRE_CHECK_GL_ERROR(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldfb));

        // Set up temporary FBO
        GLuint tempFBO;
        OGRE_CHECK_GL_ERROR(glGenFramebuffers(1, &tempFBO));
        OGRE_CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, tempFBO));

        src->bindToFramebuffer(GL_COLOR_ATTACHMENT0, 0);
        rs->_getStateCacheManager()->bindGLTexture(mTarget, mTextureID);

        switch (mTarget)
        {
        case GL_TEXTURE_2D:
        case GL_TEXTURE_CUBE_MAP:
            OGRE_CHECK_GL_ERROR(glCopyTexSubImage2D(mFaceTarget, mLevel, dstBox.left, dstBox.top,
                                                    srcBox.left, srcBox.top, dstBox.width(),
                                                    dstBox.height()));
            break;
        case GL_TEXTURE_3D:
        case GL_TEXTURE_2D_ARRAY:
            // Process each destination slice
            for (uint32 slice = dstBox.front; slice < dstBox.back; ++slice)
            {
                OGRE_CHECK_GL_ERROR(glCopyTexSubImage3D(mFaceTarget, mLevel, dstBox.left, dstBox.top, slice,
                                                        srcBox.left, srcBox.top, dstBox.width(),
                                                        dstBox.height()));
            }
            break;
        }

        // Restore old framebuffer
        OGRE_CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, oldfb));
        OGRE_CHECK_GL_ERROR(glDeleteFramebuffers(1, &tempFBO));

        // Generate mipmaps
        if (mUsage & TU_AUTOMIPMAP)
        {
            OGRE_CHECK_GL_ERROR(glGenerateMipmap(mTarget));
        }
    }
    //-----------------------------------------------------------------------------  
    // blitFromMemory doing hardware trilinear scaling
    void GLES2TextureBuffer::blitFromMemory(const PixelBox &src, const Box &dstBox)
    {
        // Fall back to normal GLHardwarePixelBuffer::blitFromMemory in case 
        // the source dimensions match the destination ones, in which case no scaling is needed
        // FIXME: always uses software path, as blitFromTexture is not implemented
        if (true || (src.size() == dstBox.size())) {
            _blitFromMemory(src, dstBox);
            return;
        }
        if(!mBuffer.contains(dstBox))
            OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Destination box out of range",
                        "GLES2TextureBuffer::blitFromMemory");

        TextureType type = (src.depth() != 1) ? TEX_TYPE_3D : TEX_TYPE_2D;

        // Set automatic mipmap generation; nice for minimisation
        TexturePtr tex = TextureManager::singleton().createManual(
            "GLBlitFromMemoryTMP",
            ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME,
            type,
            src.width(),
            src.height(),
            src.depth(),
            MIP_UNLIMITED,
            src.format);

        // Upload data to 0,0,0 in temporary texture
        Box tempTarget(src.size());
        tex->getBuffer()->blitFromMemory(src);

        // Blit from texture
        blit(tex->getBuffer(), tempTarget, dstBox);

        // Delete temp texture
        TextureManager::singleton().remove(tex);
    }
    
    //********* GLES2RenderBuffer
    //----------------------------------------------------------------------------- 
    GLES2RenderBuffer::GLES2RenderBuffer(GLenum format, uint32 width, uint32 height, GLsizei numSamples):
    GLHardwarePixelBufferCommon(width, height, 1, GLES2PixelUtil::getClosestOGREFormat(format), HBU_GPU_ONLY)
    {
        GLES2RenderSystem* rs = getGLES2RenderSystem();

        mGLInternalFormat = format;
        mNumSamples = numSamples;
        
        // Generate renderbuffer
        OGRE_CHECK_GL_ERROR(glGenRenderbuffers(1, &mRenderbufferID));

        // Bind it to FBO
        OGRE_CHECK_GL_ERROR(glBindRenderbuffer(GL_RENDERBUFFER, mRenderbufferID));

        // Allocate storage for depth buffer
        if (mNumSamples > 0)
        {
            if(rs->hasMinGLVersion(3, 0) || rs->checkExtension("GL_APPLE_framebuffer_multisample"))
            {
                OGRE_CHECK_GL_ERROR(glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER,
                                                                          mNumSamples, mGLInternalFormat, mWidth, mHeight));
            }
        }
        else
        {
            OGRE_CHECK_GL_ERROR(glRenderbufferStorage(GL_RENDERBUFFER, mGLInternalFormat,
                                                      mWidth, mHeight));
        }
    }
    //----------------------------------------------------------------------------- 
    GLES2RenderBuffer::~GLES2RenderBuffer()
    {
        OGRE_CHECK_GL_ERROR(glDeleteRenderbuffers(1, &mRenderbufferID));
    }
    //-----------------------------------------------------------------------------  
    void GLES2RenderBuffer::bindToFramebuffer(uint32 attachment, uint32 zoffset)
    {
        assert(zoffset < mDepth);
        OGRE_CHECK_GL_ERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment,
                                                      GL_RENDERBUFFER, mRenderbufferID));
    }
}
