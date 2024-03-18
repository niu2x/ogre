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
#include "pixel_format.h"
#include "pixel_format_description.h"

namespace {
#include "OgrePixelConversions.h"
}

namespace Ogre {

//-----------------------------------------------------------------------
size_t PixelBox::get_consecutive_size() const
{
    return PixelUtil::get_memory_size(width(), height(), depth(), format);
}
PixelBox
PixelBox::get_sub_volume(const Box& def, bool resetOrigin /* = true */) const
{
    OgreAssert(contains(def), "");

    if (PixelUtil::is_compressed(format)
        && (def.left != left || def.top != top || def.right != right
            || def.bottom != bottom))
        OGRE_EXCEPT(
            Exception::ERR_INVALIDPARAMS,
            "Cannot return subvolume of compressed PixelBuffer with less than "
            "slice granularity",
            "PixelBox::getSubVolume");

    // Calculate new pixelbox and optionally reset origin.
    PixelBox rval(def, format, data);
    rval.row_pitch = row_pitch;
    rval.slice_pitch = slice_pitch;

    if (resetOrigin) {
        if (PixelUtil::is_compressed(format)) {
            if (rval.front > 0) {
                rval.data = (uint8*)rval.data
                    + rval.front
                        * PixelUtil::get_memory_size(
                            width(),
                            height(),
                            1,
                            format);
                rval.back -= rval.front;
                rval.front = 0;
            }
        } else {
            rval.data = rval.get_top_left_front_pixel_ptr();
            rval.right -= rval.left;
            rval.bottom -= rval.top;
            rval.back -= rval.front;
            rval.front = rval.top = rval.left = 0;
        }
    }

    return rval;
}
uchar* PixelBox::get_top_left_front_pixel_ptr() const
{
    return data
        + (left + top * row_pitch + front * slice_pitch)
        * PixelUtil::get_num_elem_bytes(format);
}
//-----------------------------------------------------------------------
/**
 * Directly get the description record for provided pixel format. For debug
 * builds, this checks the bounds of fmt with an assertion.
 */
static inline const PixelFormatDescription&
get_description_for(const PixelFormat fmt)
{
    const int ord = (int)fmt;
    assert(ord >= 0 && ord < PixelFormat::COUNT);
    return pixel_formats[ord];
}
//-----------------------------------------------------------------------
uint8 PixelUtil::get_num_elem_bytes(PixelFormat format)
{
    return get_description_for(format).elem_bytes;
}
//-----------------------------------------------------------------------
static size_t astc_slice_size(
    uint32_t width,
    uint32_t height,
    uint32_t blockWidth,
    uint32_t blockHeight)
{
    return ((width + blockWidth - 1) / blockWidth)
        * ((height + blockHeight - 1) / blockHeight) * 16;
}
size_t PixelUtil::get_memory_size(
    uint32_t width,
    uint32_t height,
    uint32_t depth,
    PixelFormat format)
{
    if (is_compressed(format)) {
        switch (format) {
            // DXT formats work by dividing the image into 4x4 blocks, then
            // encoding each 4x4 block with a certain number of bytes.
            case PixelFormat::DXT1:
                return ((width + 3) / 4) * ((height + 3) / 4) * 8 * depth;
            case PixelFormat::DXT2:
            case PixelFormat::DXT3:
            case PixelFormat::DXT4:
            case PixelFormat::DXT5:
                return ((width + 3) / 4) * ((height + 3) / 4) * 16 * depth;
            case PixelFormat::BC4_SNORM:
            case PixelFormat::BC4_UNORM:
                return ((width + 3) / 4) * ((height + 3) / 4) * 8 * depth;
            case PixelFormat::BC5_SNORM:
            case PixelFormat::BC5_UNORM:
            case PixelFormat::BC6H_SF16:
            case PixelFormat::BC6H_UF16:
            case PixelFormat::BC7_UNORM:
                return ((width + 3) / 4) * ((height + 3) / 4) * 16 * depth;

            // Size calculations from the PVRTC OpenGL extension spec
            // http://www.khronos.org/registry/gles/extensions/IMG/IMG_texture_compression_pvrtc.txt
            // Basically, 32 bytes is the minimum texture size.  Smaller
            // textures are padded up to 32 bytes
            case PixelFormat::PVRTC_RGB2:
            case PixelFormat::PVRTC_RGBA2:
            case PixelFormat::PVRTC2_2BPP:
                return (std::max((int)width, 16) * std::max((int)height, 8) * 2
                        + 7)
                    / 8;
            case PixelFormat::PVRTC_RGB4:
            case PixelFormat::PVRTC_RGBA4:
            case PixelFormat::PVRTC2_4BPP:
                return (std::max((int)width, 8) * std::max((int)height, 8) * 4
                        + 7)
                    / 8;

            // see
            // https://registry.khronos.org/OpenGL-Refpages/es3/html/glCompressedTexImage2D.xhtml
            case PixelFormat::ETC1_RGB8:
            case PixelFormat::ETC2_RGB8:
            case PixelFormat::ETC2_RGB8A1:
            case PixelFormat::ATC_RGB:
                return ((width + 3) / 4) * ((height + 3) / 4) * 8;
            case PixelFormat::ETC2_RGBA8:
            case PixelFormat::ATC_RGBA_EXPLICIT_ALPHA:
            case PixelFormat::ATC_RGBA_INTERPOLATED_ALPHA:
                return ((width + 3) / 4) * ((height + 3) / 4) * 16;

            case PixelFormat::ASTC_RGBA_4X4_LDR:
                return astc_slice_size(width, height, 4, 4) * depth;
            case PixelFormat::ASTC_RGBA_5X4_LDR:
                return astc_slice_size(width, height, 5, 4) * depth;
            case PixelFormat::ASTC_RGBA_5X5_LDR:
                return astc_slice_size(width, height, 5, 5) * depth;
            case PixelFormat::ASTC_RGBA_6X5_LDR:
                return astc_slice_size(width, height, 6, 5) * depth;
            case PixelFormat::ASTC_RGBA_6X6_LDR:
                return astc_slice_size(width, height, 6, 6) * depth;
            case PixelFormat::ASTC_RGBA_8X5_LDR:
                return astc_slice_size(width, height, 8, 5) * depth;
            case PixelFormat::ASTC_RGBA_8X6_LDR:
                return astc_slice_size(width, height, 8, 6) * depth;
            case PixelFormat::ASTC_RGBA_8X8_LDR:
                return astc_slice_size(width, height, 8, 8) * depth;
            case PixelFormat::ASTC_RGBA_10X5_LDR:
                return astc_slice_size(width, height, 10, 5) * depth;
            case PixelFormat::ASTC_RGBA_10X6_LDR:
                return astc_slice_size(width, height, 10, 6) * depth;
            case PixelFormat::ASTC_RGBA_10X8_LDR:
                return astc_slice_size(width, height, 10, 8) * depth;
            case PixelFormat::ASTC_RGBA_10X10_LDR:
                return astc_slice_size(width, height, 10, 10) * depth;
            case PixelFormat::ASTC_RGBA_12X10_LDR:
                return astc_slice_size(width, height, 12, 10) * depth;
            case PixelFormat::ASTC_RGBA_12X12_LDR:
                return astc_slice_size(width, height, 12, 12) * depth;
            default:
                OGRE_EXCEPT(
                    Exception::ERR_INVALIDPARAMS,
                    "Invalid compressed pixel format",
                    "PixelUtil::get_memory_size");
        }
    } else {
        return width * height * depth * get_num_elem_bytes(format);
    }
}
//-----------------------------------------------------------------------
uint8 PixelUtil::get_num_elem_bits(PixelFormat format)
{
    return get_description_for(format).elem_bytes * 8;
}
//-----------------------------------------------------------------------
uint32_t PixelUtil::get_flags(PixelFormat format)
{
    return get_description_for(format).flags;
}
//-----------------------------------------------------------------------
bool PixelUtil::has_alpha(PixelFormat format)
{
    return (PixelUtil::get_flags(format) & PFF_HAS_ALPHA);
}
//-----------------------------------------------------------------------
bool PixelUtil::is_floating_point(PixelFormat format)
{
    return (PixelUtil::get_flags(format) & PFF_FLOAT);
}
//-----------------------------------------------------------------------
bool PixelUtil::is_integer(PixelFormat format)
{
    return (PixelUtil::get_flags(format) & PFF_INTEGER);
}
//-----------------------------------------------------------------------
bool PixelUtil::is_compressed(PixelFormat format)
{
    return (PixelUtil::get_flags(format) & PFF_COMPRESSED);
}
//-----------------------------------------------------------------------
bool PixelUtil::is_depth(PixelFormat format)
{
    return (PixelUtil::get_flags(format) & PFF_DEPTH);
}
//-----------------------------------------------------------------------
bool PixelUtil::is_native_endian(PixelFormat format)
{
    return (PixelUtil::get_flags(format) & PFF_NATIVE_ENDIAN);
}
//-----------------------------------------------------------------------
bool PixelUtil::is_luminance(PixelFormat format)
{
    return (PixelUtil::get_flags(format) & PFF_LUMINANCE);
}
//-----------------------------------------------------------------------
void PixelUtil::get_bit_depths(PixelFormat format, int rgba[4])
{
    const PixelFormatDescription& des = get_description_for(format);
    rgba[0] = des.rbits;
    rgba[1] = des.gbits;
    rgba[2] = des.bbits;
    rgba[3] = des.abits;
}
//-----------------------------------------------------------------------
void PixelUtil::get_bit_masks(PixelFormat format, uint64_t rgba[4])
{
    const PixelFormatDescription& des = get_description_for(format);
    rgba[0] = des.rmask;
    rgba[1] = des.gmask;
    rgba[2] = des.bmask;
    rgba[3] = des.amask;
}
//---------------------------------------------------------------------
void PixelUtil::get_bit_shifts(PixelFormat format, unsigned char rgba[4])
{
    const PixelFormatDescription& des = get_description_for(format);
    rgba[0] = des.rshift;
    rgba[1] = des.gshift;
    rgba[2] = des.bshift;
    rgba[3] = des.ashift;
}
//-----------------------------------------------------------------------
const String& PixelUtil::get_format_name(PixelFormat srcformat)
{
    return get_description_for(srcformat).name;
}
//-----------------------------------------------------------------------
bool PixelUtil::is_accessible(PixelFormat srcformat)
{
    return (srcformat != PixelFormat::UNKNOWN) && !is_compressed(srcformat);
}
//-----------------------------------------------------------------------
PixelComponentType PixelUtil::get_component_type(PixelFormat fmt)
{
    const PixelFormatDescription& des = get_description_for(fmt);
    return des.component_type;
}
//-----------------------------------------------------------------------
uint8_t PixelUtil::get_component_count(PixelFormat fmt)
{
    const PixelFormatDescription& des = get_description_for(fmt);
    return des.component_count;
}
//-----------------------------------------------------------------------
PixelFormat PixelUtil::get_format_from_name(
    const String& name,
    bool accessible_only,
    bool case_sensitive)
{
    String tmp = name;
    if (!case_sensitive) {
        // We are stored upper-case format names.
        StringUtil::upper_case(&tmp);
    }

    for (int i = 0; i < (int)PixelFormat::COUNT; ++i) {
        PixelFormat pf = static_cast<PixelFormat>(i);
        if (!accessible_only || is_accessible(pf)) {
            if (tmp == get_format_name(pf))
                return pf;
        }
    }

    // allow look-up by alias name
    if (tmp == "PixelFormat::BYTE_RGB")
        return PixelFormat::BYTE_RGB;
    if (tmp == "PixelFormat::BYTE_RGBA")
        return PixelFormat::BYTE_RGBA;
    if (tmp == "PixelFormat::BYTE_BGR")
        return PixelFormat::BYTE_BGR;
    if (tmp == "PixelFormat::BYTE_BGRA")
        return PixelFormat::BYTE_BGRA;

    return PixelFormat::UNKNOWN;
}
//-----------------------------------------------------------------------
PixelFormat PixelUtil::get_format_for_bit_depths(
    PixelFormat fmt,
    ushort integer_bits,
    ushort float_bits)
{
    switch (integer_bits) {
        case 16:
            switch (fmt) {
                case PixelFormat::R8G8B8:
                case PixelFormat::X8R8G8B8:
                    return PixelFormat::R5G6B5;

                case PixelFormat::B8G8R8:
                case PixelFormat::X8B8G8R8:
                    return PixelFormat::B5G6R5;

                case PixelFormat::A8R8G8B8:
                case PixelFormat::R8G8B8A8:
                case PixelFormat::A8B8G8R8:
                case PixelFormat::B8G8R8A8:
                    return PixelFormat::A4R4G4B4;

                case PixelFormat::A2R10G10B10:
                case PixelFormat::A2B10G10R10:
                    return PixelFormat::A1R5G5B5;

                default:
                    // use original image format
                    break;
            }
            break;

        case 32:
            switch (fmt) {
                case PixelFormat::R5G6B5:
                    return PixelFormat::X8R8G8B8;

                case PixelFormat::B5G6R5:
                    return PixelFormat::X8B8G8R8;

                case PixelFormat::A4R4G4B4:
                    return PixelFormat::A8R8G8B8;

                case PixelFormat::A1R5G5B5:
                    return PixelFormat::A2R10G10B10;

                default:
                    // use original image format
                    break;
            }
            break;

        default:
            // use original image format
            break;
    }

    switch (float_bits) {
        case 16:
            switch (fmt) {
                case PixelFormat::FLOAT32_R:
                    return PixelFormat::FLOAT16_R;

                case PixelFormat::FLOAT32_RGB:
                    return PixelFormat::FLOAT16_RGB;

                case PixelFormat::FLOAT32_RGBA:
                    return PixelFormat::FLOAT16_RGBA;

                default:
                    // use original image format
                    break;
            }
            break;

        case 32:
            switch (fmt) {
                case PixelFormat::FLOAT16_R:
                    return PixelFormat::FLOAT32_R;

                case PixelFormat::FLOAT16_RGB:
                    return PixelFormat::FLOAT32_RGB;

                case PixelFormat::FLOAT16_RGBA:
                    return PixelFormat::FLOAT32_RGBA;

                default:
                    // use original image format
                    break;
            }
            break;

        default:
            // use original image format
            break;
    }

    return fmt;
}
//-----------------------------------------------------------------------
/*************************************************************************
 * Pixel packing/unpacking utilities
 */
void PixelUtil::pack_color(
    const uint8_t r,
    const uint8_t g,
    const uint8_t b,
    const uint8_t a,
    const PixelFormat pf,
    void* dest)
{
    const PixelFormatDescription& des = get_description_for(pf);

    if (des.flags & PFF_NATIVE_ENDIAN) {
        // Shortcut for integer formats packing
        uint32_t value
            = ((Bitwise::fixed_to_fixed(r, 8, des.rbits) << des.rshift)
               & des.rmask)
            | ((Bitwise::fixed_to_fixed(g, 8, des.gbits) << des.gshift)
               & des.gmask)
            | ((Bitwise::fixed_to_fixed(b, 8, des.bbits) << des.bshift)
               & des.bmask)
            | ((Bitwise::fixed_to_fixed(a, 8, des.abits) << des.ashift)
               & des.amask);
        // And write to memory
        Bitwise::int_write(dest, des.elem_bytes, value);
    } else {
        // Convert to float
        pack_color(
            (float)r / 255.0f,
            (float)g / 255.0f,
            (float)b / 255.0f,
            (float)a / 255.0f,
            pf,
            dest);
    }
}
//-----------------------------------------------------------------------
void PixelUtil::pack_color(
    const float r,
    const float g,
    const float b,
    const float a,
    const PixelFormat pf,
    void* dest)
{
    // Catch-it-all here
    const PixelFormatDescription& des = get_description_for(pf);

    if (des.flags & PFF_NATIVE_ENDIAN) {
        // Do the packing
        // std::cerr << dest << " " << r << " " << g <<  " " << b << " " << a <<
        // std::endl;
        const unsigned int value
            = ((Bitwise::float_to_fixed(r, des.rbits) << des.rshift)
               & des.rmask)
            | ((Bitwise::float_to_fixed(g, des.gbits) << des.gshift)
               & des.gmask)
            | ((Bitwise::float_to_fixed(b, des.bbits) << des.bshift)
               & des.bmask)
            | ((Bitwise::float_to_fixed(a, des.abits) << des.ashift)
               & des.amask);
        // And write to memory
        Bitwise::int_write(dest, des.elem_bytes, value);
    } else {
        switch (pf) {
            case PixelFormat::FLOAT32_R:
                ((float*)dest)[0] = r;
                break;
            case PixelFormat::FLOAT32_GR:
                ((float*)dest)[0] = g;
                ((float*)dest)[1] = r;
                break;
            case PixelFormat::FLOAT32_RGB:
                ((float*)dest)[0] = r;
                ((float*)dest)[1] = g;
                ((float*)dest)[2] = b;
                break;
            case PixelFormat::FLOAT32_RGBA:
                ((float*)dest)[0] = r;
                ((float*)dest)[1] = g;
                ((float*)dest)[2] = b;
                ((float*)dest)[3] = a;
                break;
            case PixelFormat::DEPTH16:
            case PixelFormat::FLOAT16_R:
                ((uint16*)dest)[0] = Bitwise::float_to_half(r);
                break;
            case PixelFormat::FLOAT16_GR:
                ((uint16*)dest)[0] = Bitwise::float_to_half(g);
                ((uint16*)dest)[1] = Bitwise::float_to_half(r);
                break;
            case PixelFormat::FLOAT16_RGB:
                ((uint16*)dest)[0] = Bitwise::float_to_half(r);
                ((uint16*)dest)[1] = Bitwise::float_to_half(g);
                ((uint16*)dest)[2] = Bitwise::float_to_half(b);
                break;
            case PixelFormat::FLOAT16_RGBA:
                ((uint16*)dest)[0] = Bitwise::float_to_half(r);
                ((uint16*)dest)[1] = Bitwise::float_to_half(g);
                ((uint16*)dest)[2] = Bitwise::float_to_half(b);
                ((uint16*)dest)[3] = Bitwise::float_to_half(a);
                break;
            case PixelFormat::SHORT_RGB:
                ((uint16*)dest)[0] = (uint16)Bitwise::float_to_fixed(r, 16);
                ((uint16*)dest)[1] = (uint16)Bitwise::float_to_fixed(g, 16);
                ((uint16*)dest)[2] = (uint16)Bitwise::float_to_fixed(b, 16);
                break;
            case PixelFormat::SHORT_RGBA:
                ((uint16*)dest)[0] = (uint16)Bitwise::float_to_fixed(r, 16);
                ((uint16*)dest)[1] = (uint16)Bitwise::float_to_fixed(g, 16);
                ((uint16*)dest)[2] = (uint16)Bitwise::float_to_fixed(b, 16);
                ((uint16*)dest)[3] = (uint16)Bitwise::float_to_fixed(a, 16);
                break;
            case PixelFormat::BYTE_LA:
                ((uint8*)dest)[0] = (uint8)Bitwise::float_to_fixed(r, 8);
                ((uint8*)dest)[1] = (uint8)Bitwise::float_to_fixed(a, 8);
                break;
            case PixelFormat::A8:
                ((uint8*)dest)[0] = (uint8)Bitwise::float_to_fixed(r, 8);
                break;
            default:
                // Not yet supported
                OGRE_EXCEPT(
                    Exception::ERR_NOT_IMPLEMENTED,
                    "pack to " + get_format_name(pf) + " not implemented",
                    "PixelUtil::pack_color");
                break;
        }
    }
}

#define UNPACK_FIXED_TO_FIXED(value, mask, shift, bits)                        \
    (uint8_t) Bitwise::fixed_to_fixed(((value) & (mask)) >> (shift), (bits), 8)

#define UNPACK_FIXED_TO_FLOAT(value, mask, shift, bits)                        \
    (uint8_t) Bitwise::fixed_to_float(((value) & (mask)) >> (shift), (bits))

//-----------------------------------------------------------------------
void PixelUtil::unpack_color(
    uint8_t* r,
    uint8_t* g,
    uint8_t* b,
    uint8_t* a,
    PixelFormat pf,
    const void* src)
{
    const PixelFormatDescription& des = get_description_for(pf);
    if (des.flags & PFF_NATIVE_ENDIAN) {
        // Shortcut for integer formats unpacking
        const uint32_t value = Bitwise::int_read(src, des.elem_bytes);
        if (des.flags & PFF_LUMINANCE) {
            // Luminance format -- only rbits used
            *r = *g = *b = UNPACK_FIXED_TO_FIXED(
                value,
                des.rmask,
                des.rshift,
                des.rbits);
        } else {
            *r = UNPACK_FIXED_TO_FIXED(value, des.rmask, des.rshift, des.rbits);
            *g = UNPACK_FIXED_TO_FIXED(value, des.gmask, des.gshift, des.gbits);
            *b = UNPACK_FIXED_TO_FIXED(value, des.bmask, des.bshift, des.bbits);
        }
        if (des.flags & PFF_HAS_ALPHA) {
            *a = UNPACK_FIXED_TO_FIXED(value, des.amask, des.ashift, des.abits);
        } else {
            *a = 255; // No alpha, default a component to full
        }
    } else {
        // Do the operation with the more generic floating point
        float rr = 0, gg = 0, bb = 0, aa = 0;
        unpack_color(&rr, &gg, &bb, &aa, pf, src);
        *r = (uint8)Bitwise::float_to_fixed(rr, 8);
        *g = (uint8)Bitwise::float_to_fixed(gg, 8);
        *b = (uint8)Bitwise::float_to_fixed(bb, 8);
        *a = (uint8)Bitwise::float_to_fixed(aa, 8);
    }
}
//-----------------------------------------------------------------------
void PixelUtil::unpack_color(
    float* r,
    float* g,
    float* b,
    float* a,
    PixelFormat pf,
    const void* src)
{
    const PixelFormatDescription& des = get_description_for(pf);
    if (des.flags & PFF_NATIVE_ENDIAN) {
        // Shortcut for integer formats unpacking
        const uint32_t value = Bitwise::int_read(src, des.elem_bytes);
        if (des.flags & PFF_LUMINANCE) {
            // Luminance format -- only rbits used
            *r = *g = *b = UNPACK_FIXED_TO_FLOAT(
                value,
                des.rmask,
                des.rshift,
                des.rbits);
        } else {
            *r = UNPACK_FIXED_TO_FLOAT(value, des.rmask, des.rshift, des.rbits);
            *g = UNPACK_FIXED_TO_FLOAT(value, des.gmask, des.gshift, des.gbits);
            *b = UNPACK_FIXED_TO_FLOAT(value, des.bmask, des.bshift, des.bbits);
        }
        if (des.flags & PFF_HAS_ALPHA) {
            *a = UNPACK_FIXED_TO_FLOAT(value, des.amask, des.ashift, des.abits);
        } else {
            *a = 1.0f; // No alpha, default a component to full
        }
    } else {
        switch (pf) {
            case PixelFormat::FLOAT32_R:
                *r = *g = *b = ((const float*)src)[0];
                *a = 1.0f;
                break;
            case PixelFormat::FLOAT32_GR:
                *g = ((const float*)src)[0];
                *r = *b = ((const float*)src)[1];
                *a = 1.0f;
                break;
            case PixelFormat::FLOAT32_RGB:
                *r = ((const float*)src)[0];
                *g = ((const float*)src)[1];
                *b = ((const float*)src)[2];
                *a = 1.0f;
                break;
            case PixelFormat::FLOAT32_RGBA:
                *r = ((const float*)src)[0];
                *g = ((const float*)src)[1];
                *b = ((const float*)src)[2];
                *a = ((const float*)src)[3];
                break;
            case PixelFormat::FLOAT16_R:
                *r = *g = *b = Bitwise::half_to_float(((const uint16*)src)[0]);
                *a = 1.0f;
                break;
            case PixelFormat::FLOAT16_GR:
                *g = Bitwise::half_to_float(((const uint16*)src)[0]);
                *r = *b = Bitwise::half_to_float(((const uint16*)src)[1]);
                *a = 1.0f;
                break;
            case PixelFormat::FLOAT16_RGB:
                *r = Bitwise::half_to_float(((const uint16*)src)[0]);
                *g = Bitwise::half_to_float(((const uint16*)src)[1]);
                *b = Bitwise::half_to_float(((const uint16*)src)[2]);
                *a = 1.0f;
                break;
            case PixelFormat::FLOAT16_RGBA:
                *r = Bitwise::half_to_float(((const uint16*)src)[0]);
                *g = Bitwise::half_to_float(((const uint16*)src)[1]);
                *b = Bitwise::half_to_float(((const uint16*)src)[2]);
                *a = Bitwise::half_to_float(((const uint16*)src)[3]);
                break;
            case PixelFormat::SHORT_RGB:
                *r = Bitwise::fixed_to_float(((const uint16*)src)[0], 16);
                *g = Bitwise::fixed_to_float(((const uint16*)src)[1], 16);
                *b = Bitwise::fixed_to_float(((const uint16*)src)[2], 16);
                *a = 1.0f;
                break;
            case PixelFormat::SHORT_RGBA:
                *r = Bitwise::fixed_to_float(((const uint16*)src)[0], 16);
                *g = Bitwise::fixed_to_float(((const uint16*)src)[1], 16);
                *b = Bitwise::fixed_to_float(((const uint16*)src)[2], 16);
                *a = Bitwise::fixed_to_float(((const uint16*)src)[3], 16);
                break;
            case PixelFormat::BYTE_LA:
                *r = *g = *b
                    = Bitwise::fixed_to_float(((const uint8*)src)[0], 8);
                *a = Bitwise::fixed_to_float(((const uint8*)src)[1], 8);
                break;
            default:
                // Not yet supported
                OGRE_EXCEPT(
                    Exception::ERR_NOT_IMPLEMENTED,
                    "unpack from " + get_format_name(pf) + " not implemented",
                    "PixelUtil::unpack_color");
                break;
        }
    }
}
//-----------------------------------------------------------------------
/* Convert pixels from one format to another */
void PixelUtil::bulk_pixel_conversion(const PixelBox& src, const PixelBox& dst)
{
    OgreAssert(src.size() == dst.size(), "");

    // Check for compressed formats, we don't support
    // decompression, compression or recoding
    if (PixelUtil::is_compressed(src.format)
        || PixelUtil::is_compressed(dst.format)) {
        OgreAssert(
            src.format == dst.format && src.is_consecutive()
                && dst.is_consecutive(),
            "This method can not be used to compress or "
            "decompress images");
        // we can copy with slice granularity, useful for
        // Tex2DArray handling
        size_t bytes_per_slice
            = get_memory_size(src.width(), src.height(), 1, src.format);
        memcpy(
            dst.data + bytes_per_slice * dst.front,
            src.data + bytes_per_slice * src.front,
            bytes_per_slice * src.depth());
        return;
    }

    // The easy case
    if (src.format == dst.format) {
        auto* src_ptr = src.get_top_left_front_pixel_ptr();
        auto* dstptr = dst.get_top_left_front_pixel_ptr();

        // Everything consecutive?
        if (src.is_consecutive() && dst.is_consecutive()) {
            memcpy(dstptr, src_ptr, src.get_consecutive_size());
            return;
        }

        const size_t src_pixel_size = PixelUtil::get_num_elem_bytes(src.format);
        const size_t dst_pixel_size = PixelUtil::get_num_elem_bytes(dst.format);

        // Calculate pitches+skips in bytes
        const size_t src_row_pitch_bytes = src.row_pitch * src_pixel_size;
        // const size_t src_row_skip_bytes =
        // src.get_row_skip()*src_pixel_size;
        const size_t src_slice_skip_bytes
            = src.get_slice_skip() * src_pixel_size;

        const size_t dst_row_pitch_bytes = dst.row_pitch * dst_pixel_size;
        // const size_t dstRowSkipBytes =
        // dst.get_row_skip()*dst_pixel_size;
        const size_t dst_slice_skip_bytes
            = dst.get_slice_skip() * dst_pixel_size;

        // Otherwise, copy per row
        const size_t rowSize = src.width() * src_pixel_size;
        for (size_t z = src.front; z < src.back; z++) {
            for (size_t y = src.top; y < src.bottom; y++) {
                memcpy(dstptr, src_ptr, rowSize);
                src_ptr += src_row_pitch_bytes;
                dstptr += dst_row_pitch_bytes;
            }
            src_ptr += src_slice_skip_bytes;
            dstptr += dst_slice_skip_bytes;
        }
        return;
    }
    // Converting to PixelFormat::X8R8G8B8 is exactly the same
    // as converting to PixelFormat::A8R8G8B8. (same with
    // PixelFormat::X8B8G8R8 and PixelFormat::A8B8G8R8)
    if (dst.format == PixelFormat::X8R8G8B8
        || dst.format == PixelFormat::X8B8G8R8) {
        // Do the same conversion, with PixelFormat::A8R8G8B8,
        // which has a lot of optimized conversions
        PixelBox tempdst = dst;
        tempdst.format = dst.format == PixelFormat::X8R8G8B8
            ? PixelFormat::A8R8G8B8
            : PixelFormat::A8B8G8R8;
        bulk_pixel_conversion(src, tempdst);
        return;
    }
    // Converting from PixelFormat::X8R8G8B8 is exactly the same
    // as converting from PixelFormat::A8R8G8B8, given that the
    // destination format does not have alpha.
    if ((src.format == PixelFormat::X8R8G8B8
         || src.format == PixelFormat::X8B8G8R8)
        && !has_alpha(dst.format)) {
        // Do the same conversion, with PixelFormat::A8R8G8B8,
        // which has a lot of optimized conversions
        PixelBox tempsrc = src;
        tempsrc.format = src.format == PixelFormat::X8R8G8B8
            ? PixelFormat::A8R8G8B8
            : PixelFormat::A8B8G8R8;
        bulk_pixel_conversion(tempsrc, dst);
        return;
    }

    // NB VC6 can't handle the templates required for optimised
    // conversion, tough
#if OGRE_COMPILER != OGRE_COMPILER_MSVC || OGRE_COMP_VER >= 1300
    // Is there a specialized, inlined, conversion?
    if (doOptimizedConversion(src, dst)) {
        // If so, good
        return;
    }
#endif

    const size_t src_pixel_size = PixelUtil::get_num_elem_bytes(src.format);
    const size_t dst_pixel_size = PixelUtil::get_num_elem_bytes(dst.format);
    uint8* src_ptr = src.get_top_left_front_pixel_ptr();
    uint8* dstptr = dst.get_top_left_front_pixel_ptr();

    // Old way, not taking into account box dimensions
    // uint8 *src_ptr = static_cast<uint8*>(src.data), *dstptr =
    // static_cast<uint8*>(dst.data);

    // Calculate pitches+skips in bytes
    const size_t src_row_skip_bytes = src.get_row_skip() * src_pixel_size;
    const size_t src_slice_skip_bytes = src.get_slice_skip() * src_pixel_size;
    const size_t dstRowSkipBytes = dst.get_row_skip() * dst_pixel_size;
    const size_t dst_slice_skip_bytes = dst.get_slice_skip() * dst_pixel_size;

    // The brute force fallback
    float r = 0, g = 0, b = 0, a = 1;
    for (size_t z = src.front; z < src.back; z++) {
        for (size_t y = src.top; y < src.bottom; y++) {
            for (size_t x = src.left; x < src.right; x++) {
                unpack_color(&r, &g, &b, &a, src.format, src_ptr);
                pack_color(r, g, b, a, dst.format, dstptr);
                src_ptr += src_pixel_size;
                dstptr += dst_pixel_size;
            }
            src_ptr += src_row_skip_bytes;
            dstptr += dstRowSkipBytes;
        }
        src_ptr += src_slice_skip_bytes;
        dstptr += dst_slice_skip_bytes;
    }
}
//-----------------------------------------------------------------------
void PixelUtil::bulk_pixel_vertical_flip(const PixelBox& box)
{
    // Check for compressed formats, we don't support
    // decompression, compression or recoding
    OgreAssert(
        !PixelUtil::is_compressed(box.format),
        "This method can not be used for compressed formats");

    const size_t pixelSize = PixelUtil::get_num_elem_bytes(box.format);
    const size_t copySize = box.width() * pixelSize;

    // Calculate pitches in bytes
    const size_t rowPitchBytes = box.row_pitch * pixelSize;
    const size_t slicePitchBytes = box.slice_pitch * pixelSize;

    uint8* base_src_ptr = box.get_top_left_front_pixel_ptr();
    uint8* base_dst_ptr
        = base_src_ptr + (box.bottom - box.top - 1) * rowPitchBytes;
    uint8* tmpptr = (uint8*)OGRE_MALLOC_SIMD(copySize, MEMCATEGORY_GENERAL);

    // swap rows
    const size_t halfRowCount = (box.bottom - box.top) >> 1;
    for (size_t z = box.front; z < box.back; z++) {
        uint8* src_ptr = base_src_ptr;
        uint8* dstptr = base_dst_ptr;
        for (size_t y = 0; y < halfRowCount; y++) {
            // swap rows
            memcpy(tmpptr, dstptr, copySize);
            memcpy(dstptr, src_ptr, copySize);
            memcpy(src_ptr, tmpptr, copySize);
            src_ptr += rowPitchBytes;
            dstptr -= rowPitchBytes;
        }
        base_src_ptr += slicePitchBytes;
        base_dst_ptr += slicePitchBytes;
    }

    OGRE_FREE_SIMD(tmpptr, MEMCATEGORY_GENERAL);
}

ColorValue PixelBox::get_color(size_t x, size_t y, size_t z) const
{
    ColorValue cv;

    size_t pixelSize = PixelUtil::get_num_elem_bytes(format);
    size_t pixelOffset = pixelSize * (z * slice_pitch + y * row_pitch + x);
    PixelUtil::unpack_color(&cv, format, (unsigned char*)data + pixelOffset);

    return cv;
}

void PixelBox::set_color(
    const Ogre::ColorValue& cv,
    size_t x,
    size_t y,
    size_t z)
{
    size_t pixelSize = PixelUtil::get_num_elem_bytes(format);
    size_t pixelOffset = pixelSize * (z * slice_pitch + y * row_pitch + x);
    PixelUtil::pack_color(cv, format, (unsigned char*)data + pixelOffset);
}

} // namespace Ogre
