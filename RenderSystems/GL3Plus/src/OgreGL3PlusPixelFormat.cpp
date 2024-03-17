/*
  -----------------------------------------------------------------------------
  This source file is part of OGRE
  (Object-oriented Graphics Rendering Engine)
  For the latest info, see http://www.ogre3d.org

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

#include "OgreGL3PlusPixelFormat.h"
#include "OgreRoot.h"
#include "OgreRenderSystem.h"

#ifndef GL_EXT_texture_compression_s3tc
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT   0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT  0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT  0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT  0x83F3
#endif

#ifndef GL_EXT_texture_sRGB
#define GL_SRGB_EXT                       0x8C40
#define GL_SRGB8_EXT                      0x8C41
#define GL_SRGB_ALPHA_EXT                 0x8C42
#define GL_SRGB8_ALPHA8_EXT               0x8C43
#define GL_SLUMINANCE_ALPHA_EXT           0x8C44
#define GL_SLUMINANCE8_ALPHA8_EXT         0x8C45
#define GL_SLUMINANCE_EXT                 0x8C46
#define GL_SLUMINANCE8_EXT                0x8C47
#define GL_COMPRESSED_SRGB_EXT            0x8C48
#define GL_COMPRESSED_SRGB_ALPHA_EXT      0x8C49
#define GL_COMPRESSED_SLUMINANCE_EXT      0x8C4A
#define GL_COMPRESSED_SLUMINANCE_ALPHA_EXT 0x8C4B
#define GL_COMPRESSED_SRGB_S3TC_DXT1_EXT  0x8C4C
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT 0x8C4D
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT 0x8C4E
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT 0x8C4F
#endif


namespace Ogre  {

    struct GLPixelFormatDescription {
        GLenum format;
        GLenum type;
        GLenum internalFormat;
    };

    static GLPixelFormatDescription _pixelFormats[] = {
        { GL_NONE }, // PixelFormat::UNKNOWN
        { GL_RED, GL_UNSIGNED_BYTE, GL_R8 }, // PixelFormat::L8
        { GL_RED, GL_UNSIGNED_SHORT, GL_R16 }, // PixelFormat::L16
        { GL_RED, GL_UNSIGNED_BYTE, GL_R8 }, // PixelFormat::A8
        { GL_RG, GL_UNSIGNED_BYTE, GL_RG8 }, // PixelFormat::BYTE_LA
        { GL_RGB, GL_UNSIGNED_SHORT_5_6_5, GL_RGB5 }, // PixelFormat::R5G6B5
        { GL_BGR, GL_UNSIGNED_SHORT_5_6_5, GL_RGB5 }, // PixelFormat::B5G6R5
        { GL_BGRA,
          GL_UNSIGNED_SHORT_4_4_4_4_REV,
          GL_RGBA4 }, // PixelFormat::A4R4G4B4
        { GL_BGRA,
          GL_UNSIGNED_SHORT_1_5_5_5_REV,
          GL_RGB5_A1 }, // PixelFormat::A1R5G5B5
#if OGRE_ENDIAN == OGRE_ENDIAN_BIG
        { GL_RGB, GL_UNSIGNED_BYTE, GL_RGB8 }, // PixelFormat::R8G8B8
        { GL_BGR, GL_UNSIGNED_BYTE, GL_RGB8 }, // PixelFormat::B8G8R8
#else
        { GL_BGR, GL_UNSIGNED_BYTE, GL_RGB8 }, // PixelFormat::R8G8B8
        { GL_RGB, GL_UNSIGNED_BYTE, GL_RGB8 }, // PixelFormat::B8G8R8
#endif
        { GL_BGRA,
          GL_UNSIGNED_INT_8_8_8_8_REV,
          GL_RGBA8 }, // PixelFormat::A8R8G8B8
        { GL_RGBA,
          GL_UNSIGNED_INT_8_8_8_8_REV,
          GL_RGBA8 }, // PixelFormat::A8B8G8R8
        { GL_BGRA, GL_UNSIGNED_INT_8_8_8_8, GL_RGBA8 }, // PixelFormat::B8G8R8A8
        { GL_BGRA_INTEGER,
          GL_UNSIGNED_INT_2_10_10_10_REV,
          GL_RGB10_A2UI }, // PixelFormat::A2R10G10B10
        { GL_RGBA_INTEGER,
          GL_UNSIGNED_INT_2_10_10_10_REV,
          GL_RGB10_A2UI }, // PixelFormat::A2B10G10R10
        { GL_NONE,
          GL_NONE,
          GL_COMPRESSED_RGBA_S3TC_DXT1_EXT }, // PixelFormat::DXT1
        { GL_NONE }, // PixelFormat::DXT2
        { GL_NONE,
          GL_NONE,
          GL_COMPRESSED_RGBA_S3TC_DXT3_EXT }, // PixelFormat::DXT3
        { GL_NONE }, // PixelFormat::DXT4
        { GL_NONE,
          GL_NONE,
          GL_COMPRESSED_RGBA_S3TC_DXT5_EXT }, // PixelFormat::DXT5
        { GL_RGB, GL_HALF_FLOAT, GL_RGB16F }, // PixelFormat::FLOAT16_RGB
        { GL_RGBA, GL_HALF_FLOAT, GL_RGBA16F }, // PixelFormat::FLOAT16_RGBA
        { GL_RGB, GL_FLOAT, GL_RGB32F }, // PixelFormat::FLOAT32_RGB
        { GL_RGBA, GL_FLOAT, GL_RGBA32F }, // PixelFormat::FLOAT32_RGBA
        { GL_BGRA,
          GL_UNSIGNED_INT_8_8_8_8_REV,
          GL_RGBA8 }, // PixelFormat::X8R8G8B8
        { GL_RGBA,
          GL_UNSIGNED_INT_8_8_8_8_REV,
          GL_RGBA8 }, // PixelFormat::X8B8G8R8
        { GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, GL_RGBA8 }, // PixelFormat::R8G8B8A8
        { GL_DEPTH_COMPONENT,
          GL_UNSIGNED_SHORT,
          GL_DEPTH_COMPONENT16 }, // PixelFormat::DEPTH16
        { GL_RGBA, GL_UNSIGNED_SHORT, GL_RGBA16 }, // PixelFormat::SHORT_RGBA
        { GL_RGB, GL_UNSIGNED_BYTE_3_3_2, GL_R3_G3_B2 }, // PixelFormat::R3G3B2
        { GL_RED, GL_HALF_FLOAT, GL_R16F }, // PixelFormat::FLOAT16_R
        { GL_RED, GL_FLOAT, GL_R32F }, // PixelFormat::FLOAT32_R
        { GL_RG, GL_UNSIGNED_SHORT, GL_RG16 }, // PixelFormat::SHORT_GR
        { GL_RG, GL_HALF_FLOAT, GL_RG16F }, // PixelFormat::FLOAT16_GR
        { GL_RG, GL_FLOAT, GL_RG32F }, // PixelFormat::FLOAT32_GR
        { GL_RGB, GL_UNSIGNED_SHORT, GL_RGB16 }, // PixelFormat::SHORT_RGB
        { GL_NONE }, // PixelFormat::PVRTC_RGB2
        { GL_NONE }, // PixelFormat::PVRTC_RGBA2
        { GL_NONE }, // PixelFormat::PVRTC_RGB4
        { GL_NONE }, // PixelFormat::PVRTC_RGBA4
        { GL_NONE }, // PixelFormat::PVRTC2_2BPP
        { GL_NONE }, // PixelFormat::PVRTC2_4BPP
        { GL_RGB,
          GL_UNSIGNED_INT_10F_11F_11F_REV,
          GL_R11F_G11F_B10F }, // PixelFormat::R11G11B10_FLOAT
        { GL_RED_INTEGER, GL_UNSIGNED_BYTE, GL_R8UI }, // PixelFormat::R8_UINT
        { GL_RG_INTEGER, GL_UNSIGNED_BYTE, GL_RG8UI }, // PixelFormat::R8G8_UINT
        { GL_RGB_INTEGER,
          GL_UNSIGNED_BYTE,
          GL_RGB8UI }, // PixelFormat::R8G8B8_UINT
        { GL_RGBA_INTEGER,
          GL_UNSIGNED_BYTE,
          GL_RGBA8UI }, // PixelFormat::R8G8B8A8_UINT
        { GL_RED_INTEGER,
          GL_UNSIGNED_SHORT,
          GL_R16UI }, // PixelFormat::R16_UINT
        { GL_RG_INTEGER,
          GL_UNSIGNED_SHORT,
          GL_RG16UI }, // PixelFormat::R16G16_UINT
        { GL_RGB_INTEGER,
          GL_UNSIGNED_SHORT,
          GL_RGB16UI }, // PixelFormat::R16G16B16_UINT
        { GL_RGBA_INTEGER,
          GL_UNSIGNED_SHORT,
          GL_RGBA16UI }, // PixelFormat::R16G16B16A16_UINT
        { GL_RED_INTEGER, GL_UNSIGNED_INT, GL_R32UI }, // PixelFormat::R32_UINT
        { GL_RG_INTEGER,
          GL_UNSIGNED_INT,
          GL_RG32UI }, // PixelFormat::R32G32_UINT
        { GL_RGB_INTEGER,
          GL_UNSIGNED_INT,
          GL_RGB32UI }, // PixelFormat::R32G32B32_UINT
        { GL_RGBA_INTEGER,
          GL_UNSIGNED_INT,
          GL_RGBA32UI }, // PixelFormat::R32G32B32A32_UINT
        { GL_RED_INTEGER, GL_BYTE, GL_R8I }, // PixelFormat::R8_SINT
        { GL_RG_INTEGER, GL_BYTE, GL_RG8I }, // PixelFormat::R8G8_SINT
        { GL_RGB_INTEGER, GL_BYTE, GL_RGB8I }, // PixelFormat::R8G8B8_SINT
        { GL_RGBA_INTEGER, GL_BYTE, GL_RGBA8I }, // PixelFormat::R8G8B8A8_SINT
        { GL_RED_INTEGER, GL_SHORT, GL_R16I }, // PixelFormat::R16_SINT
        { GL_RG_INTEGER, GL_SHORT, GL_RG16I }, // PixelFormat::R16G16_SINT
        { GL_RGB_INTEGER, GL_SHORT, GL_RGB16I }, // PixelFormat::R16G16B16_SINT
        { GL_RGBA_INTEGER,
          GL_SHORT,
          GL_RGBA16I }, // PixelFormat::R16G16B16A16_SINT
        { GL_RED_INTEGER, GL_INT, GL_R32I }, // PixelFormat::R32_SINT
        { GL_RG_INTEGER, GL_INT, GL_RG32I }, // PixelFormat::R32G32_SINT
        { GL_RGB_INTEGER, GL_INT, GL_RGB32I }, // PixelFormat::R32G32B32_SINT
        { GL_RGBA_INTEGER,
          GL_INT,
          GL_RGBA32I }, // PixelFormat::R32G32B32A32_SINT
        { GL_RGB,
          GL_UNSIGNED_INT_5_9_9_9_REV,
          GL_RGB9_E5 }, // PixelFormat::R9G9B9E5_SHAREDEXP
        { GL_NONE, GL_NONE, GL_COMPRESSED_RED_RGTC1 }, // PixelFormat::BC4_UNORM
        { GL_NONE,
          GL_NONE,
          GL_COMPRESSED_SIGNED_RED_RGTC1 }, // PixelFormat::BC4_SNORM
        { GL_NONE, GL_NONE, GL_COMPRESSED_RG_RGTC2 }, // PixelFormat::BC5_UNORM
        { GL_NONE,
          GL_NONE,
          GL_COMPRESSED_SIGNED_RG_RGTC2 }, // PixelFormat::BC5_SNORM
        { GL_NONE,
          GL_NONE,
          GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB }, // PixelFormat::BC6H_UF16
        { GL_NONE,
          GL_NONE,
          GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB }, // PixelFormat::BC6H_SF16
        { GL_NONE,
          GL_NONE,
          GL_COMPRESSED_RGBA_BPTC_UNORM_ARB }, // PixelFormat::BC7_UNORM
        { GL_RED, GL_UNSIGNED_BYTE, GL_R8 }, // PixelFormat::R8
        { GL_RG, GL_UNSIGNED_BYTE, GL_RG8 }, // PixelFormat::RG8
        { GL_RED, GL_UNSIGNED_BYTE, GL_R8_SNORM }, // PixelFormat::R8_SNORM
        { GL_RG, GL_UNSIGNED_BYTE, GL_RG8_SNORM }, // PixelFormat::RG8_SNORM
        { GL_RGB, GL_UNSIGNED_BYTE, GL_RGB8_SNORM }, // PixelFormat::RGB8_SNORM
        { GL_RGBA,
          GL_UNSIGNED_BYTE,
          GL_RGBA8_SNORM }, // PixelFormat::RGBA8_SNORM
        { GL_RED, GL_UNSIGNED_SHORT, GL_R16_SNORM }, // PixelFormat::R16_SNORM
        { GL_RG, GL_UNSIGNED_SHORT, GL_RG16_SNORM }, // PixelFormat::RG16_SNORM
        { GL_RGB,
          GL_UNSIGNED_SHORT,
          GL_RGB16_SNORM }, // PixelFormat::RGB16_SNORM
        { GL_RGBA,
          GL_UNSIGNED_SHORT,
          GL_RGBA16_SNORM }, // PixelFormat::RGBA16_SNORM
        { GL_NONE }, // PixelFormat::ETC1_RGB8
        { GL_NONE, GL_NONE, GL_COMPRESSED_RGB8_ETC2 }, // PixelFormat::ETC2_RGB8
        { GL_NONE,
          GL_NONE,
          GL_COMPRESSED_RGBA8_ETC2_EAC }, // PixelFormat::ETC2_RGBA8
        { GL_NONE,
          GL_NONE,
          GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2 }, // PixelFormat::ETC2_RGB8A1
        { GL_NONE }, // PixelFormat::ATC_RGB
        { GL_NONE }, // PixelFormat::ATC_RGBA_EXPLICIT_ALPHA
        { GL_NONE }, // PixelFormat::ATC_RGBA_INTERPOLATED_ALPHA
        { GL_NONE,
          GL_NONE,
          GL_COMPRESSED_RGBA_ASTC_4x4_KHR }, // PixelFormat::ASTC_RGBA_4X4_LDR
        { GL_NONE,
          GL_NONE,
          GL_COMPRESSED_RGBA_ASTC_5x4_KHR }, // PixelFormat::ASTC_RGBA_5X4_LDR
        { GL_NONE,
          GL_NONE,
          GL_COMPRESSED_RGBA_ASTC_5x5_KHR }, // PixelFormat::ASTC_RGBA_5X5_LDR
        { GL_NONE,
          GL_NONE,
          GL_COMPRESSED_RGBA_ASTC_6x5_KHR }, // PixelFormat::ASTC_RGBA_6X5_LDR
        { GL_NONE,
          GL_NONE,
          GL_COMPRESSED_RGBA_ASTC_6x6_KHR }, // PixelFormat::ASTC_RGBA_6X6_LDR
        { GL_NONE,
          GL_NONE,
          GL_COMPRESSED_RGBA_ASTC_8x5_KHR }, // PixelFormat::ASTC_RGBA_8X5_LDR
        { GL_NONE,
          GL_NONE,
          GL_COMPRESSED_RGBA_ASTC_8x6_KHR }, // PixelFormat::ASTC_RGBA_8X6_LDR
        { GL_NONE,
          GL_NONE,
          GL_COMPRESSED_RGBA_ASTC_8x8_KHR }, // PixelFormat::ASTC_RGBA_8X8_LDR
        { GL_NONE,
          GL_NONE,
          GL_COMPRESSED_RGBA_ASTC_10x5_KHR }, // PixelFormat::ASTC_RGBA_10X5_LDR
        { GL_NONE,
          GL_NONE,
          GL_COMPRESSED_RGBA_ASTC_10x6_KHR }, // PixelFormat::ASTC_RGBA_10X6_LDR
        { GL_NONE,
          GL_NONE,
          GL_COMPRESSED_RGBA_ASTC_10x8_KHR }, // PixelFormat::ASTC_RGBA_10X8_LDR
        { GL_NONE,
          GL_NONE,
          GL_COMPRESSED_RGBA_ASTC_10x10_KHR }, // PixelFormat::ASTC_RGBA_10X10_LDR
        { GL_NONE,
          GL_NONE,
          GL_COMPRESSED_RGBA_ASTC_12x10_KHR }, // PixelFormat::ASTC_RGBA_12X10_LDR
        { GL_NONE,
          GL_NONE,
          GL_COMPRESSED_RGBA_ASTC_12x12_KHR }, // PixelFormat::ASTC_RGBA_12X12_LDR
        { GL_DEPTH_COMPONENT,
          GL_UNSIGNED_INT,
          GL_DEPTH_COMPONENT32 }, // PixelFormat::DEPTH32
        { GL_DEPTH_COMPONENT,
          GL_FLOAT,
          GL_DEPTH_COMPONENT32F }, // PixelFormat::DEPTH32F
        { GL_DEPTH_COMPONENT,
          GL_UNSIGNED_INT,
          GL_DEPTH24_STENCIL8 }, // PixelFormat::DEPTH24_STENCIL8
    };

    GLenum GL3PlusPixelUtil::getGLOriginFormat(PixelFormat pf)
    {
        static_assert(
            sizeof(_pixelFormats) / sizeof(GLPixelFormatDescription)
                == (int) PixelFormat::COUNT,
            "Did you add a new format?");
        return _pixelFormats[(int)pf].format;
    }

    GLenum GL3PlusPixelUtil::getGLOriginDataType(PixelFormat pf)
    {
        return _pixelFormats[(int)pf].type;
    }

    GLenum GL3PlusPixelUtil::getGLInternalFormat(PixelFormat pf, bool hwGamma)
    {
        GLenum ret = _pixelFormats[(int)pf].internalFormat;

        if(!hwGamma)
            return ret;

        switch(ret)
        {
        case GL_RGB8:
            return GL_SRGB8;
        case GL_RGBA8:
            return GL_SRGB8_ALPHA8;
        case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
            return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
        case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
            return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
        case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
            return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
        case GL_COMPRESSED_RGBA_BPTC_UNORM_ARB:
            return GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB;
        case GL_COMPRESSED_RGBA_ASTC_4x4_KHR:
        case GL_COMPRESSED_RGBA_ASTC_5x4_KHR:
        case GL_COMPRESSED_RGBA_ASTC_5x5_KHR:
        case GL_COMPRESSED_RGBA_ASTC_6x5_KHR:
        case GL_COMPRESSED_RGBA_ASTC_6x6_KHR:
        case GL_COMPRESSED_RGBA_ASTC_8x5_KHR:
        case GL_COMPRESSED_RGBA_ASTC_8x6_KHR:
        case GL_COMPRESSED_RGBA_ASTC_8x8_KHR:
        case GL_COMPRESSED_RGBA_ASTC_10x5_KHR:
        case GL_COMPRESSED_RGBA_ASTC_10x6_KHR:
        case GL_COMPRESSED_RGBA_ASTC_10x8_KHR:
        case GL_COMPRESSED_RGBA_ASTC_10x10_KHR:
        case GL_COMPRESSED_RGBA_ASTC_12x10_KHR:
        case GL_COMPRESSED_RGBA_ASTC_12x12_KHR:
            return ret + 0x20; // ASTC SRGBA format offset
        default:
            return ret;
        }
    }

    GLenum GL3PlusPixelUtil::getGLImageInternalFormat(PixelFormat format)
    {
        switch(format)
        {
            case PixelFormat::L8:
            case PixelFormat::A8:
                return GL_R8;
            case PixelFormat::L16:
                return GL_R16;
            case PixelFormat::BYTE_LA:
                return GL_RG8;
            case PixelFormat::A8R8G8B8:
            case PixelFormat::B8G8R8A8:
            case PixelFormat::A8B8G8R8:
            case PixelFormat::R8G8B8A8:
                return GL_RGBA8;
            case PixelFormat::A2R10G10B10:
            case PixelFormat::A2B10G10R10:
                // return GL_RGB10_A2UI;
                return GL_RGB10_A2;
            case PixelFormat::FLOAT16_R:
                return GL_R16F;
            case PixelFormat::FLOAT16_GR:
                return GL_RG16F;
            case PixelFormat::FLOAT16_RGBA:
                return GL_RGBA16F;
            case PixelFormat::FLOAT32_R:
                return GL_R32F;
            case PixelFormat::FLOAT32_GR:
                return GL_RG32F;
            case PixelFormat::FLOAT32_RGBA:
                return GL_RGBA32F;
            case PixelFormat::SHORT_RGBA:
                return GL_RGBA16;
            case PixelFormat::SHORT_GR:
                return GL_RG16;
            case PixelFormat::R11G11B10_FLOAT:
                return GL_R11F_G11F_B10F;
            case PixelFormat::R8_UINT:
                return GL_R8UI;
            case PixelFormat::R8G8_UINT:
                return GL_RG8UI;
            case PixelFormat::R8G8B8A8_UINT:
                return GL_RGBA8UI;
            case PixelFormat::R16_UINT:
                return GL_R16UI;
            case PixelFormat::R16G16_UINT:
                return GL_RG16UI;
            case PixelFormat::R16G16B16A16_UINT:
                return GL_RGBA16UI;
            case PixelFormat::R32_UINT:
                return GL_R32UI;
            case PixelFormat::R32G32_UINT:
                return GL_RG32UI;
            case PixelFormat::R32G32B32A32_UINT:
                return GL_RGBA32UI;
            case PixelFormat::R8_SINT:
                return GL_R8I;
            case PixelFormat::R8G8_SINT:
                return GL_RG8I;
            case PixelFormat::R8G8B8_SINT:
                return GL_RG8I;
            case PixelFormat::R16_SINT:
                return GL_R16I;
            case PixelFormat::R16G16_SINT:
                return GL_RG16I;
            case PixelFormat::R16G16B16A16_SINT:
                return GL_RGBA16I;
            case PixelFormat::R32_SINT:
                return GL_R32I;
            case PixelFormat::R32G32_SINT:
                return GL_RG32I;
            case PixelFormat::R32G32B32A32_SINT:
                return GL_RGBA32I;
            case PixelFormat::R8G8_SNORM:
                return GL_RG8_SNORM;
            case PixelFormat::R16_SNORM:
                return GL_R16_SNORM;
            case PixelFormat::R16G16_SNORM:
                return GL_RG16_SNORM;
            case PixelFormat::R16G16B16A16_SNORM:
                return GL_RGBA16_SNORM;
            case PixelFormat::R8G8B8A8_SNORM:
                return GL_RGBA8_SNORM;

            default:
                return GL_NONE;
        }
    }


    GLenum GL3PlusPixelUtil::getClosestGLImageInternalFormat(PixelFormat format)
    {
        GLenum GLformat = getGLImageInternalFormat(format);
        return (GLformat == GL_NONE ? GL_RGBA8 : GLformat);
    }


    PixelFormat GL3PlusPixelUtil::getClosestOGREFormat(GLenum format)
    {
        switch(format)
        {
        case GL_DEPTH_COMPONENT24:
        case GL_DEPTH_COMPONENT:
            return PixelFormat::DEPTH16;
        case GL_DEPTH_COMPONENT32:
            return PixelFormat::DEPTH32;
        case GL_DEPTH_COMPONENT32F:
            return PixelFormat::DEPTH32F;
        case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT:
            return PixelFormat::DXT1;
        case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT:
            return PixelFormat::DXT3;
        case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT:
            return PixelFormat::DXT5;
        case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB:
            return PixelFormat::BC7_UNORM;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR:
            return PixelFormat(
                int(PixelFormat::ASTC_RGBA_4X4_LDR)
                + (format - GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR));
        case GL_SRGB8:
        case GL_RGB8: // prefer native endian byte format
            return PixelFormat::BYTE_RGB;
        case GL_SRGB8_ALPHA8:
        case GL_RGBA8: // prefer native endian byte format
            return PixelFormat::BYTE_RGBA;
        };

        for (int pf = 0; pf < (int)PixelFormat::COUNT; pf++) {
            if(_pixelFormats[pf].internalFormat == format)
                return (PixelFormat)pf;
        }

        return PixelFormat::BYTE_RGBA;
    }
}
