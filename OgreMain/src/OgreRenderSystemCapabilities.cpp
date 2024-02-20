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

#include "OgreRenderSystemCapabilities.h"

namespace Ogre {

    String DriverVersion::toString() const
    {
        StringStream str;
        str << major << "." << minor << "." << release << "." << build;
        return str.str();
    }

    void  DriverVersion::fromString(const String& versionString)
    {
        StringVector tokens = StringUtil::split(versionString, ".");
        if(!tokens.empty())
        {
            major = StringConverter::parseInt(tokens[0]);
            if (tokens.size() > 1)
                minor = StringConverter::parseInt(tokens[1]);
            if (tokens.size() > 2)
                release = StringConverter::parseInt(tokens[2]);
            if (tokens.size() > 3)
                build = StringConverter::parseInt(tokens[3]);
        }
    }

    //-----------------------------------------------------------------------
    RenderSystemCapabilities::RenderSystemCapabilities()
        : mVendor(GPU_UNKNOWN)
        , mNumTextureUnits(0)
        , mStencilBufferBitDepth(8)
        , mConstantFloatCount{}
        , mNumMultiRenderTargets(1)
        , mMaxPointSize(1)
        , mNonPOW2TexturesLimited(false)
        , mMaxSupportedAnisotropy(0)
        , mGeometryProgramNumOutputVertices(0)
        , mNumVertexAttributes(1)
    {
        for(int i = 0; i < CAPS_CATEGORY_COUNT; i++)
        {
            mCapabilities[i] = 0;
        }
        mCategoryRelevant[CAPS_CATEGORY_COMMON] = true;
        mCategoryRelevant[CAPS_CATEGORY_COMMON_2] = true;
        // each rendersystem should enable these
        mCategoryRelevant[CAPS_CATEGORY_D3D9] = false;
        mCategoryRelevant[CAPS_CATEGORY_GL] = false;
    }

    void RenderSystemCapabilities::addShaderProfile(const String& profile) { mSupportedShaderProfiles.insert(profile); }

    void RenderSystemCapabilities::removeShaderProfile(const String& profile)
    {
        mSupportedShaderProfiles.erase(profile);
    }

    bool RenderSystemCapabilities::isShaderProfileSupported(const String& profile) const
    {
        return (mSupportedShaderProfiles.end() != mSupportedShaderProfiles.find(profile));
    }

    //-----------------------------------------------------------------------
    void RenderSystemCapabilities::log(Log* pLog) const
    {
        pLog->log_message("RenderSystem capabilities");
        pLog->log_message("-------------------------");
        pLog->log_message("RenderSystem Name: " + getRenderSystemName());
        pLog->log_message("GPU Vendor: " + vendorToString(getVendor()));
        pLog->log_message("Device Name: " + getDeviceName());
        pLog->log_message("Driver Version: " + getDriverVersion().toString());
        pLog->log_message(" * Fixed function pipeline: " +
                         StringConverter::toString(hasCapability(RSC_FIXED_FUNCTION), true));
        pLog->log_message(" * 32-bit index buffers: " + StringConverter::toString(hasCapability(RSC_32BIT_INDEX), true));
        pLog->log_message(" * Hardware stencil buffer: " +
                         StringConverter::toString(hasCapability(RSC_HWSTENCIL), true));
        if (hasCapability(RSC_HWSTENCIL))
        {
            pLog->log_message("   - Two sided stencil support: " +
                             StringConverter::toString(hasCapability(RSC_TWO_SIDED_STENCIL), true));
            pLog->log_message("   - Wrap stencil values: " +
                             StringConverter::toString(hasCapability(RSC_STENCIL_WRAP), true));
        }
        pLog->log_message(" * Gpu programs: " + StringConverter::toString(hasCapability(RSC_VERTEX_PROGRAM), true));
        if (hasCapability(RSC_VERTEX_PROGRAM))
        {
            pLog->log_message("   - Vertex constant 4-vectors: " +
                             StringConverter::toString(mConstantFloatCount[GPT_VERTEX_PROGRAM]));
            pLog->log_message("   - Fragment constant 4-vectors: " +
                             StringConverter::toString(mConstantFloatCount[GPT_FRAGMENT_PROGRAM]));
        }
        pLog->log_message(" * Geometry programs: " +

                         StringConverter::toString(hasCapability(RSC_GEOMETRY_PROGRAM), true));
        if (hasCapability(RSC_GEOMETRY_PROGRAM))
        {
            pLog->log_message("   - Number of constant 4-vectors: " +
                             StringConverter::toString(mConstantFloatCount[GPT_GEOMETRY_PROGRAM]));
        }
        pLog->log_message(" * Tessellation programs: " +
                         StringConverter::toString(hasCapability(RSC_TESSELLATION_PROGRAM), true));
        if (hasCapability(RSC_TESSELLATION_PROGRAM))
        {
            pLog->log_message("   - Hull program constant 4-vectors: " +
                             StringConverter::toString(mConstantFloatCount[GPT_HULL_PROGRAM]));
            pLog->log_message("   - Domain program constant 4-vectors: " +
                             StringConverter::toString(mConstantFloatCount[GPT_DOMAIN_PROGRAM]));
        }
        pLog->log_message(" * Compute programs: " + StringConverter::toString(hasCapability(RSC_COMPUTE_PROGRAM), true));
        if (hasCapability(RSC_COMPUTE_PROGRAM))
        {
            pLog->log_message("   - Number of constant 4-vectors: " +
                             StringConverter::toString(mConstantFloatCount[GPT_COMPUTE_PROGRAM]));
        }
        pLog->log_message(
            " * Supported Shader Profiles: " +
            StringConverter::toString(StringVector(mSupportedShaderProfiles.begin(), mSupportedShaderProfiles.end())));
        pLog->log_message(" * Read-back compiled shader: " +
                         StringConverter::toString(hasCapability(RSC_CAN_GET_COMPILED_SHADER_BUFFER), true));
        pLog->log_message(" * Number of vertex attributes: " + StringConverter::toString(mNumVertexAttributes));
        pLog->log_message(" * Textures");
        pLog->log_message("   - Number of texture units: " + StringConverter::toString(mNumTextureUnits));
        pLog->log_message("   - Floating point: " + StringConverter::toString(hasCapability(RSC_TEXTURE_FLOAT), true));
        pLog->log_message(
            "   - Non-power-of-two: " + StringConverter::toString(hasCapability(RSC_NON_POWER_OF_2_TEXTURES), true) +
            (mNonPOW2TexturesLimited ? " (limited)" : ""));
        pLog->log_message("   - 1D textures: " + StringConverter::toString(hasCapability(RSC_TEXTURE_1D), true));
        pLog->log_message("   - 2D array textures: " + StringConverter::toString(hasCapability(RSC_TEXTURE_2D_ARRAY), true));
        pLog->log_message("   - 3D textures: " + StringConverter::toString(hasCapability(RSC_TEXTURE_3D), true));
        pLog->log_message("   - Anisotropic filtering: " + StringConverter::toString(hasCapability(RSC_ANISOTROPY), true));

        pLog->log_message(
            " * Texture Compression: "
            + StringConverter::toString(hasCapability(RSC_TEXTURE_COMPRESSION), true));
        if (hasCapability(RSC_TEXTURE_COMPRESSION))
        {
            pLog->log_message("   - DXT: " +
                             StringConverter::toString(hasCapability(RSC_TEXTURE_COMPRESSION_DXT), true));
            pLog->log_message("   - VTC: " +
                             StringConverter::toString(hasCapability(RSC_TEXTURE_COMPRESSION_VTC), true));
            pLog->log_message("   - PVRTC: " +
                             StringConverter::toString(hasCapability(RSC_TEXTURE_COMPRESSION_PVRTC), true));
            pLog->log_message("   - ATC: " +
                             StringConverter::toString(hasCapability(RSC_TEXTURE_COMPRESSION_ATC), true));
            pLog->log_message("   - ETC1: " +
                             StringConverter::toString(hasCapability(RSC_TEXTURE_COMPRESSION_ETC1), true));
            pLog->log_message("   - ETC2: " +
                             StringConverter::toString(hasCapability(RSC_TEXTURE_COMPRESSION_ETC2), true));
            pLog->log_message("   - BC4/BC5: " +
                             StringConverter::toString(hasCapability(RSC_TEXTURE_COMPRESSION_BC4_BC5), true));
            pLog->log_message("   - BC6H/BC7: " +
                             StringConverter::toString(hasCapability(RSC_TEXTURE_COMPRESSION_BC6H_BC7), true));
            pLog->log_message("   - ASTC: " +
                             StringConverter::toString(hasCapability(RSC_TEXTURE_COMPRESSION_ASTC), true));
            pLog->log_message("   - Automatic mipmap generation: " +
                             StringConverter::toString(hasCapability(RSC_AUTOMIPMAP_COMPRESSED), true));
        }

        pLog->log_message(" * Vertex Buffers");
        pLog->log_message("   - Render to Vertex Buffer: " +
                         StringConverter::toString(hasCapability(RSC_HWRENDER_TO_VERTEX_BUFFER), true));
        pLog->log_message("   - Instance Data: " +
                         StringConverter::toString(hasCapability(RSC_VERTEX_BUFFER_INSTANCE_DATA), true));
        pLog->log_message("   - Primitive Restart: " +
                         StringConverter::toString(hasCapability(RSC_PRIMITIVE_RESTART), true));
        pLog->log_message("   - INT_10_10_10_2_NORM element type: " +
                         StringConverter::toString(hasCapability(RSC_VERTEX_FORMAT_INT_10_10_10_2), true));
        pLog->log_message(" * Vertex texture fetch: " +
                         StringConverter::toString(hasCapability(RSC_VERTEX_TEXTURE_FETCH), true));
        if (hasCapability(RSC_VERTEX_TEXTURE_FETCH))
        {
            pLog->log_message("   - Max vertex textures: " + StringConverter::toString(mNumVertexTextureUnits));
        }
        pLog->log_message(" * Read/Write Buffers: " +
                         StringConverter::toString(hasCapability(RSC_READ_WRITE_BUFFERS), true));
        pLog->log_message(
            " * Hardware Occlusion Query: "
            + StringConverter::toString(hasCapability(RSC_HWOCCLUSION), true));
        pLog->log_message(
            " * User clip planes: "
            + StringConverter::toString(hasCapability(RSC_USER_CLIP_PLANES), true));
        pLog->log_message(
            " * Depth clamping: "
            + StringConverter::toString(hasCapability(RSC_DEPTH_CLAMP), true));
        pLog->log_message(
            " * Hardware render-to-texture: "
            + StringConverter::toString(hasCapability(RSC_HWRENDER_TO_TEXTURE), true));
        pLog->log_message("   - Multiple Render Targets: " + StringConverter::toString(mNumMultiRenderTargets));
        pLog->log_message(" * Point Sprites: " + StringConverter::toString(hasCapability(RSC_POINT_SPRITES), true));
        if (hasCapability(RSC_POINT_SPRITES))
        {
            pLog->log_message("   - Max Size: " + StringConverter::toString(mMaxPointSize));
        }
        pLog->log_message(
            " * Wide Lines: "
            + StringConverter::toString(hasCapability(RSC_WIDE_LINES), true));
        pLog->log_message(
            " * Hardware Gamma: "
            + StringConverter::toString(hasCapability(RSC_HW_GAMMA), true));
        if (mCategoryRelevant[CAPS_CATEGORY_GL])
        {
            pLog->log_message(
                " * PBuffer support: "
                + StringConverter::toString(hasCapability(RSC_PBUFFER), true));
            pLog->log_message(
                " * Vertex Array Objects: "
                + StringConverter::toString(hasCapability(RSC_VAO), true));
            pLog->log_message(" * Separate shader objects: " +
                             StringConverter::toString(hasCapability(RSC_SEPARATE_SHADER_OBJECTS), true));
            pLog->log_message("   - redeclare GLSL interface block: " +
                             StringConverter::toString(hasCapability(RSC_GLSL_SSO_REDECLARE), true));
            pLog->log_message(
                " * Debugging/ profiling events: "
                + StringConverter::toString(hasCapability(RSC_DEBUG), true));
            pLog->log_message(
                " * Map buffer storage: "
                + StringConverter::toString(hasCapability(RSC_MAPBUFFER), true));
        }

        if (mCategoryRelevant[CAPS_CATEGORY_D3D9])
        {
            pLog->log_message(
                " * DirectX per stage constants: "
                + StringConverter::toString(hasCapability(RSC_PERSTAGECONSTANT), true));
            pLog->log_message(
                " * W-Buffer supported: "
                + StringConverter::toString(hasCapability(RSC_WBUFFER), true));
            pLog->log_message(" * Multiple Render Targets must have same bit depth: " +
                             StringConverter::toString(hasCapability(RSC_MRT_SAME_BIT_DEPTHS), true));
        }
    }
    //---------------------------------------------------------------------
    String RenderSystemCapabilities::msGPUVendorStrings[GPU_VENDOR_COUNT];
    //---------------------------------------------------------------------
    GPUVendor RenderSystemCapabilities::vendorFromString(const String& vendorString)
    {
        initVendorStrings();
        GPUVendor ret = GPU_UNKNOWN;
        String cmpString = vendorString;
        StringUtil::lower_case(&cmpString);
        for (int i = 0; i < GPU_VENDOR_COUNT; ++i)
        {
            // case insensitive (lower case)
            if (msGPUVendorStrings[i] == cmpString)
            {
                ret = static_cast<GPUVendor>(i);
                break;
            }
        }

        return ret;
        
    }
    //---------------------------------------------------------------------
    const String& RenderSystemCapabilities::vendorToString(GPUVendor v)
    {
        initVendorStrings();
        return msGPUVendorStrings[v];
    }
    //---------------------------------------------------------------------
    void RenderSystemCapabilities::initVendorStrings()
    {
        if (msGPUVendorStrings[0].empty())
        {
            // Always lower case!
            msGPUVendorStrings[GPU_UNKNOWN] = "unknown";
            msGPUVendorStrings[GPU_NVIDIA] = "nvidia";
            msGPUVendorStrings[GPU_AMD] = "amd";
            msGPUVendorStrings[GPU_INTEL] = "intel";
            msGPUVendorStrings[GPU_IMAGINATION_TECHNOLOGIES] = "imagination technologies";
            msGPUVendorStrings[GPU_APPLE] = "apple";    // iOS Simulator
            msGPUVendorStrings[GPU_NOKIA] = "nokia";
            msGPUVendorStrings[GPU_MS_SOFTWARE] = "microsoft"; // Microsoft software device
            msGPUVendorStrings[GPU_MS_WARP] = "ms warp";
            msGPUVendorStrings[GPU_ARM] = "arm";
            msGPUVendorStrings[GPU_QUALCOMM] = "qualcomm";
            msGPUVendorStrings[GPU_MOZILLA] = "mozilla";
            msGPUVendorStrings[GPU_WEBKIT] = "webkit";
        }
    }

}
