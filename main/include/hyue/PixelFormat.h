#pragma once

#include <hyue/type.h>
#include <hyue/Color.h>
#include <hyue/Box.h>
#include <hyue/endian.h>

namespace hyue {

enum class PixelFormat {
    /// Unknown pixel format.
    UNKNOWN = 0,
    /// 8-bit pixel format, all bits luminance.
    L8,
    BYTE_L = L8,
    /// 16-bit pixel format, all bits luminance.
    L16,
    SHORT_L = L16,
    /// 8-bit pixel format, all bits alpha.
    A8,
    BYTE_A = A8,
    /// 2 byte pixel format, 1 byte luminance, 1 byte alpha
    BYTE_LA,
    /// 16-bit pixel format, 5 bits red, 6 bits green, 5 bits blue.
    R5G6B5,
    /// 16-bit pixel format, 5 bits red, 6 bits green, 5 bits blue.
    B5G6R5,
    /// 16-bit pixel format, 4 bits for alpha, red, green and blue.
    A4R4G4B4,
    /// 16-bit pixel format, 5 bits for blue, green, red and 1 for alpha.
    A1R5G5B5,
    /// 24-bit pixel format, 8 bits for red, green and blue.
    R8G8B8,
    /// 24-bit pixel format, 8 bits for blue, green and red.
    B8G8R8,
    /// 32-bit pixel format, 8 bits for alpha, red, green and blue.
    A8R8G8B8,
    /// 32-bit pixel format, 8 bits for blue, green, red and alpha.
    A8B8G8R8,
    /// 32-bit pixel format, 8 bits for blue, green, red and alpha.
    B8G8R8A8,
    /// 32-bit pixel format, 2 bits for alpha, 10 bits for red, green and blue.
    A2R10G10B10,
    /// 32-bit pixel format, 10 bits for blue, green and red, 2 bits for alpha.
    A2B10G10R10,
    /// DDS (DirectDraw Surface) DXT1 format
    DXT1,
    /// DDS (DirectDraw Surface) DXT2 format
    DXT2,
    /// DDS (DirectDraw Surface) DXT3 format
    DXT3,
    /// DDS (DirectDraw Surface) DXT4 format
    DXT4,
    /// DDS (DirectDraw Surface) DXT5 format
    DXT5,
    /// 48-bit pixel format, 16 bits (float) for red, 16 bits (float) for green, 16 bits (float) for blue
    FLOAT16_RGB,
    /// 64-bit pixel format, 16 bits (float) for red, 16 bits (float) for green, 16 bits (float) for blue, 16 bits
    /// (float) for alpha
    FLOAT16_RGBA,
    /// 96-bit pixel format, 32 bits (float) for red, 32 bits (float) for green, 32 bits (float) for blue
    FLOAT32_RGB,
    /// 128-bit pixel format, 32 bits (float) for red, 32 bits (float) for green, 32 bits (float) for blue, 32 bits
    /// (float) for alpha
    FLOAT32_RGBA,
    /// 32-bit pixel format, 8 bits for red, 8 bits for green, 8 bits for blue
    /// like Ogre::A8R8G8B8, but alpha will get discarded
    X8R8G8B8,
    /// 32-bit pixel format, 8 bits for blue, 8 bits for green, 8 bits for red
    /// like Ogre::A8B8G8R8, but alpha will get discarded
    X8B8G8R8,
    /// 32-bit pixel format, 8 bits for red, green, blue and alpha.
    R8G8B8A8,
    /// Depth texture format, with 16-bit unsigned integer
    DEPTH16,
    DEPTH = DEPTH16,
    /// 64-bit pixel format, 16 bits for red, green, blue and alpha
    SHORT_RGBA,
    /// 8-bit pixel format, 2 bits blue, 3 bits green, 3 bits red.
    R3G3B2,
    /// 16-bit pixel format, 16 bits (float) for red
    FLOAT16_R,
    /// 32-bit pixel format, 32 bits (float) for red
    FLOAT32_R,
    /// 32-bit pixel format, 16-bit green, 16-bit red
    SHORT_GR,
    /// 32-bit, 2-channel s10e5 floating point pixel format, 16-bit green, 16-bit red
    FLOAT16_GR,
    /// 64-bit, 2-channel floating point pixel format, 32-bit green, 32-bit red
    FLOAT32_GR,
    /// 48-bit pixel format, 16 bits for red, green and blue
    SHORT_RGB,
    /// PVRTC (PowerVR) RGB 2 bpp
    PVRTC_RGB2,
    /// PVRTC (PowerVR) RGBA 2 bpp
    PVRTC_RGBA2,
    /// PVRTC (PowerVR) RGB 4 bpp
    PVRTC_RGB4,
    /// PVRTC (PowerVR) RGBA 4 bpp
    PVRTC_RGBA4,
    /// PVRTC (PowerVR) Version 2, 2 bpp
    PVRTC2_2BPP,
    /// PVRTC (PowerVR) Version 2, 4 bpp
    PVRTC2_4BPP,
    /// 32-bit pixel format, 11 bits (float) for red, 11 bits (float) for green, 10 bits (float) for blue
    R11G11B10_FLOAT,
    /// 8-bit pixel format, 8 bits red (unsigned int).
    R8_UINT,
    /// 16-bit pixel format, 8 bits red (unsigned int), 8 bits blue (unsigned int).
    R8G8_UINT,
    /// 24-bit pixel format, 8 bits red (unsigned int), 8 bits blue (unsigned int), 8 bits green (unsigned int).
    R8G8B8_UINT,
    /// 32-bit pixel format, 8 bits red (unsigned int), 8 bits blue (unsigned int), 8 bits green (unsigned int), 8 bits
    /// alpha (unsigned int).
    R8G8B8A8_UINT,
    /// 16-bit pixel format, 16 bits red (unsigned int).
    R16_UINT,
    /// 32-bit pixel format, 16 bits red (unsigned int), 16 bits blue (unsigned int).
    R16G16_UINT,
    /// 48-bit pixel format, 16 bits red (unsigned int), 16 bits blue (unsigned int), 16 bits green (unsigned int).
    R16G16B16_UINT,
    /// 64-bit pixel format, 16 bits red (unsigned int), 16 bits blue (unsigned int), 16 bits green (unsigned int), 16
    /// bits alpha (unsigned int).
    R16G16B16A16_UINT,
    /// 32-bit pixel format, 32 bits red (unsigned int).
    R32_UINT,
    /// 64-bit pixel format, 32 bits red (unsigned int), 32 bits blue (unsigned int).
    R32G32_UINT,
    /// 96-bit pixel format, 32 bits red (unsigned int), 32 bits blue (unsigned int), 32 bits green (unsigned int).
    R32G32B32_UINT,
    /// 128-bit pixel format, 32 bits red (unsigned int), 32 bits blue (unsigned int), 32 bits green (unsigned int), 32
    /// bits alpha (unsigned int).
    R32G32B32A32_UINT,
    /// 8-bit pixel format, 8 bits red (signed int).
    R8_SINT,
    /// 16-bit pixel format, 8 bits red (signed int), 8 bits blue (signed int).
    R8G8_SINT,
    /// 24-bit pixel format, 8 bits red (signed int), 8 bits blue (signed int), 8 bits green (signed int).
    R8G8B8_SINT,
    /// 32-bit pixel format, 8 bits red (signed int), 8 bits blue (signed int), 8 bits green (signed int), 8 bits alpha
    /// (signed int).
    R8G8B8A8_SINT,
    /// 16-bit pixel format, 16 bits red (signed int).
    R16_SINT,
    /// 32-bit pixel format, 16 bits red (signed int), 16 bits blue (signed int).
    R16G16_SINT,
    /// 48-bit pixel format, 16 bits red (signed int), 16 bits blue (signed int), 16 bits green (signed int).
    R16G16B16_SINT,
    /// 64-bit pixel format, 16 bits red (signed int), 16 bits blue (signed int), 16 bits green (signed int), 16 bits
    /// alpha (signed int).
    R16G16B16A16_SINT,
    /// 32-bit pixel format, 32 bits red (signed int).
    R32_SINT,
    /// 64-bit pixel format, 32 bits red (signed int), 32 bits blue (signed int).
    R32G32_SINT,
    /// 96-bit pixel format, 32 bits red (signed int), 32 bits blue (signed int), 32 bits green (signed int).
    R32G32B32_SINT,
    /// 128-bit pixel format, 32 bits red (signed int), 32 bits blue (signed int), 32 bits green (signed int), 32 bits
    /// alpha (signed int).
    R32G32B32A32_SINT,
    /// 32-bit pixel format, 9 bits for blue, green, red plus a 5 bit exponent.
    R9G9B9E5_SHAREDEXP,
    /// DDS (DirectDraw Surface) BC4 format (unsigned normalised)
    BC4_UNORM,
    /// DDS (DirectDraw Surface) BC4 format (signed normalised)
    BC4_SNORM,
    /// DDS (DirectDraw Surface) BC5 format (unsigned normalised)
    BC5_UNORM,
    /// DDS (DirectDraw Surface) BC5 format (signed normalised)
    BC5_SNORM,
    /// DDS (DirectDraw Surface) BC6H format (unsigned 16 bit float)
    BC6H_UF16,
    /// DDS (DirectDraw Surface) BC6H format (signed 16 bit float)
    BC6H_SF16,
    /// DDS (DirectDraw Surface) BC7 format (unsigned normalised)
    BC7_UNORM,
    /// 8-bit pixel format, all bits red.
    R8,
    /// 16-bit pixel format, 8 bits red, 8 bits green.
    R8G8,
    RG8 = R8G8,
    /// 8-bit pixel format, 8 bits red (signed normalised int).
    R8_SNORM,
    /// 16-bit pixel format, 8 bits red (signed normalised int), 8 bits blue (signed normalised int).
    R8G8_SNORM,
    /// 24-bit pixel format, 8 bits red (signed normalised int), 8 bits blue (signed normalised int), 8 bits green
    /// (signed normalised int).
    R8G8B8_SNORM,
    /// 32-bit pixel format, 8 bits red (signed normalised int), 8 bits blue (signed normalised int), 8 bits green
    /// (signed normalised int), 8 bits alpha (signed normalised int).
    R8G8B8A8_SNORM,
    /// 16-bit pixel format, 16 bits red (signed normalised int).
    R16_SNORM,
    /// 32-bit pixel format, 16 bits red (signed normalised int), 16 bits blue (signed normalised int).
    R16G16_SNORM,
    /// 48-bit pixel format, 16 bits red (signed normalised int), 16 bits blue (signed normalised int), 16 bits green
    /// (signed normalised int).
    R16G16B16_SNORM,
    /// 64-bit pixel format, 16 bits red (signed normalised int), 16 bits blue (signed normalised int), 16 bits green
    /// (signed normalised int), 16 bits alpha (signed normalised int).
    R16G16B16A16_SNORM,
    /// ETC1 (Ericsson Texture Compression)
    ETC1_RGB8,
    /// ETC2 (Ericsson Texture Compression)
    ETC2_RGB8,
    /// ETC2 (Ericsson Texture Compression)
    ETC2_RGBA8,
    /// ETC2 (Ericsson Texture Compression)
    ETC2_RGB8A1,
    /// ATC (AMD_compressed_ATC_texture)
    ATC_RGB,
    /// ATC (AMD_compressed_ATC_texture)
    ATC_RGBA_EXPLICIT_ALPHA,
    /// ATC (AMD_compressed_ATC_texture)
    ATC_RGBA_INTERPOLATED_ALPHA,
    /// ASTC (ARM Adaptive Scalable Texture Compression RGBA, block size 4x4)
    ASTC_RGBA_4X4_LDR,
    /// ASTC (ARM Adaptive Scalable Texture Compression RGBA, block size 5x4)
    ASTC_RGBA_5X4_LDR,
    /// ASTC (ARM Adaptive Scalable Texture Compression RGBA, block size 5x5)
    ASTC_RGBA_5X5_LDR,
    /// ASTC (ARM Adaptive Scalable Texture Compression RGBA, block size 6x5)
    ASTC_RGBA_6X5_LDR,
    /// ASTC (ARM Adaptive Scalable Texture Compression RGBA, block size 6x6)
    ASTC_RGBA_6X6_LDR,
    /// ASTC (ARM Adaptive Scalable Texture Compression RGBA, block size 8x5)
    ASTC_RGBA_8X5_LDR,
    /// ASTC (ARM Adaptive Scalable Texture Compression RGBA, block size 8x6)
    ASTC_RGBA_8X6_LDR,
    /// ASTC (ARM Adaptive Scalable Texture Compression RGBA, block size 8x8)
    ASTC_RGBA_8X8_LDR,
    /// ASTC (ARM Adaptive Scalable Texture Compression RGBA, block size 10x5)
    ASTC_RGBA_10X5_LDR,
    /// ASTC (ARM Adaptive Scalable Texture Compression RGBA, block size 10x6)
    ASTC_RGBA_10X6_LDR,
    /// ASTC (ARM Adaptive Scalable Texture Compression RGBA, block size 10x8)
    ASTC_RGBA_10X8_LDR,
    /// ASTC (ARM Adaptive Scalable Texture Compression RGBA, block size 10x10)
    ASTC_RGBA_10X10_LDR,
    /// ASTC (ARM Adaptive Scalable Texture Compression RGBA, block size 12x10)
    ASTC_RGBA_12X10_LDR,
    /// ASTC (ARM Adaptive Scalable Texture Compression RGBA, block size 12x12)
    ASTC_RGBA_12X12_LDR,
    DEPTH32,
    /// Depth texture format with 32-bit floating point
    DEPTH32F,
    /// Depth texture format with 24-bit unsigned integer and 8-bit stencil
    DEPTH24_STENCIL8,
    /// Number of pixel formats currently defined
    COUNT,
// endianness aware aliases
#if HYUE_ENDIAN_BIG
    /// @copydoc R8G8B8

    // R is lower address
    BYTE_RGB = R8G8B8,
    /// @copydoc B8G8R8
    BYTE_BGR = B8G8R8,
    /// @copydoc B8G8R8A8
    BYTE_BGRA = B8G8R8A8,
    /// @copydoc R8G8B8A8
    BYTE_RGBA = R8G8B8A8,
#else
    /// @copydoc B8G8R8

    // R is lower address
    BYTE_RGB = B8G8R8,
    /// @copydoc R8G8B8
    BYTE_BGR = R8G8B8,
    /// @copydoc A8R8G8B8
    BYTE_BGRA = A8R8G8B8,
    /// @copydoc A8B8G8R8
    BYTE_RGBA = A8B8G8R8,
#endif
};

using PixelFormatList = std::vector<PixelFormat>;

/**
 * Flags defining some on/off properties of pixel formats
 */
enum PixelFormatFlags {
    /// This format has an alpha channel
    PFF_HAS_ALPHA = 1 << 0,
    /** This format is compressed. This invalidates the values in elemBytes,
        elemBits and the bit counts as these might not be fixed in a compressed format. */
    PFF_COMPRESSED = 1 << 1,
    /// This is a floating point format
    PFF_FLOAT = 1 << 2,
    /// This is a depth format (for depth textures)
    PFF_DEPTH = 1 << 3,
    /** Format is in native endian. Generally true for the 16, 24 and 32 bits
        formats which can be represented as machine integers. */
    PFF_NATIVEENDIAN = 1 << 4,
    /** This is an intensity format instead of a RGB one. The luminance
        replaces R,G and B. (but not A) */
    PFF_LUMINANCE = 1 << 5,
    /// This is an integer format
    PFF_INTEGER = 1 << 6,
};

/** Pixel component format */
enum class PixelComponentType {
    BYTE, /// Byte per component (8 bit fixed 0.0..1.0)
    SHORT, /// Short per component (16 bit fixed 0.0..1.0))
    FLOAT16, /// 16 bit float per component
    FLOAT32, /// 32 bit float per component
    SINT, /// Signed integer per component
    UINT, /// Unsigned integer per component
    COUNT /// Number of pixel types
};

/** A primitive describing a volume (3D), image (2D) or line (1D) of pixels in memory.
    In case of a rectangle, depth must be 1.
    Pixels are stored as a succession of "depth" slices, each containing "height" rows of
    "width" pixels.

    @copydetails Ogre::Box
*/

class HYUE_API PixelBox : public Box {
public:
    /// Parameter constructor for setting the members manually
    PixelBox() : data(NULL), row_pitch(0), slice_pitch(0), format(PixelFormat::UNKNOWN) { }
    ~PixelBox() { }
    /** Constructor providing extents in the form of a Box object. This constructor
        assumes the pixel data is laid out consecutively in memory. (this
        means row after row, slice after slice, with no space in between)
        @param extents      Extents of the region defined by data
        @param pixelFormat  Format of this buffer
        @param pixelData    Pointer to the actual data
    */
    PixelBox(const Box& extents, PixelFormat pixel_format, void* pixel_data = 0)
    : Box(extents)
    , data((uint8_t*)pixel_data)
    , format(pixel_format)
    {
        set_consecutive();
    }
    /** Constructor providing width, height and depth. This constructor
        assumes the pixel data is laid out consecutively in memory. (this
        means row after row, slice after slice, with no space in between)
        @param width        Width of the region
        @param height       Height of the region
        @param depth        Depth of the region
        @param pixelFormat  Format of this buffer
        @param pixelData    Pointer to the actual data
    */
    PixelBox(int width, int height, int depth, PixelFormat pixel_format, void* pixelData = 0)
    : Box(0, 0, 0, width, height, depth)
    , data((uint8_t*)pixelData)
    , format(pixel_format)
    {
        set_consecutive();
    }

    /** Set the row_pitch and slice_pitch so that the buffer is laid out consecutive
        in memory.
    */
    void set_consecutive()
    {
        row_pitch = get_width();
        slice_pitch = get_width() * get_height();
    }
    /** Get the number of elements between one past the rightmost pixel of
        one row and the leftmost pixel of the next row. (IE this is zero if rows
        are consecutive).
    */
    size_t get_row_skip() const { return row_pitch - get_width(); }
    /** Get the number of elements between one past the right bottom pixel of
        one slice and the left top pixel of the next slice. (IE this is zero if slices
        are consecutive).
    */
    size_t get_slice_skip() const { return slice_pitch - (get_height() * row_pitch); }

    /** Return whether this buffer is laid out consecutive in memory (ie the pitches
        are equal to the dimensions)
    */
    bool is_consecutive() const { return row_pitch == get_width() && slice_pitch == get_width() * get_height(); }
    /** Return the size (in bytes) this image would take if it was
        laid out consecutive in memory
    */
    size_t get_consecutive_size() const;
    /** Return a subvolume of this PixelBox.
        @param def  Defines the bounds of the subregion to return
        @param reset_prigin Whether to reset left/top/front of returned PixelBox to zero
            together with adjusting data pointer to compensate this, or do nothing
            so that returned PixelBox will have left/top/front of requested Box
        @return A pixel box describing the region and the data in it
        @remarks    This function does not copy any data, it just returns
            a PixelBox object with a data pointer pointing somewhere inside
            the data of object.
        @throws Exception(ERR_INVALIDPARAMS) if def is not fully contained
    */
    PixelBox get_sub_volume(const Box& def, bool reset_prigin = true) const;

    /** Return a data pointer pointing to top left front pixel of the pixel box.
        @remarks Non consecutive pixel boxes are supported.
     */
    uint8_t* get_top_left_front_pixel_ptr() const;

    /**
     * Get color value from a certain location in the PixelBox. The z coordinate
     * is only valid for cubemaps and volume textures. This uses the first (largest)
     * mipmap.
     */
    Color get_color(size_t x, size_t y, size_t z) const;

    /**
     * Set color value at a certain location in the PixelBox. The z coordinate
     * is only valid for cubemaps and volume textures. This uses the first (largest)
     * mipmap.
     */
    void set_color(const Color& cv, size_t x, size_t y, size_t z);

public:
    /// The data pointer
    uint8_t* data;
    /** Number of elements between the leftmost pixel of one row and the left
        pixel of the next. This value must always be equal to get_width() (consecutive)
        for compressed formats.
    */
    size_t row_pitch;
    /** Number of elements between the top left pixel of one (depth) slice and
        the top left pixel of the next. This can be a negative value. Must be a multiple of
        row_pitch. This value must always be equal to get_width()*get_height() (consecutive)
        for compressed formats.
    */
    size_t slice_pitch;
    /// The pixel format
    PixelFormat format;
};

/**
 * Some utility functions for packing and unpacking pixel data
 */
class HYUE_API PixelUtil {
public:
    /** Returns the size in bytes of an element of the given pixel format.
     @return
           The size in bytes of an element. See Remarks.

           Passing PF_UNKNOWN will result in returning a size of 0 bytes.
    */
    static uint8_t get_elem_bytes(PixelFormat format);

    /** Returns the size in bits of an element of the given pixel format.
      @return
           The size in bits of an element. See Remarks.

           Passing PF_UNKNOWN will result in returning a size of 0 bits.
    */
    static uint8_t get_elem_bits(PixelFormat format);

    /** Returns the size in memory of a region with the given extents and pixel
        format with consecutive memory layout.
        @param width
            The width of the area
        @param height
            The height of the area
        @param depth
            The depth of the area
        @param format
            The format of the area
        @return
            The size in bytes

            In case that the format is non-compressed, this simply returns
            width * height * depth * PixelUtil::getNumElemBytes(format). In the compressed
            case, this does serious magic.
    */
    static size_t get_memory_size(int width, int height, int depth, PixelFormat format);

    /** Returns the property flags for this pixel format
      @return
           A bitfield combination of PFF_HASALPHA, PFF_ISCOMPRESSED,
           PFF_FLOAT, PFF_DEPTH, PFF_NATIVEENDIAN, PFF_LUMINANCE

           This replaces the separate functions for formatHasAlpha, formatIsFloat, ...
    */
    static uint32_t get_flags(PixelFormat format);

    /** Shortcut method to determine if the format has an alpha component */
    static bool has_alpha(PixelFormat format);
    /** Shortcut method to determine if the format is floating point */
    static bool is_floating_point(PixelFormat format);
    /** Shortcut method to determine if the format is integer */
    static bool is_integer(PixelFormat format);
    /** Shortcut method to determine if the format is compressed */
    static bool is_compressed(PixelFormat format);
    /** Shortcut method to determine if the format is a depth format. */
    static bool is_depth(PixelFormat format);
    /** Shortcut method to determine if the format is in native endian format. */
    static bool is_native_endian(PixelFormat format);
    /** Shortcut method to determine if the format is a luminance format. */
    static bool is_luminance(PixelFormat format);

    /** Gives the number of bits (RGBA) for a format. See remarks.
      @remarks      For non-color formats (dxt, depth) this returns [0,0,0,0].
    */
    static void get_bit_depths(PixelFormat format, int rgba[4]);

    /** Gives the masks for the R, G, B and A component
      @note         Only valid for native endian formats
    */
    static void get_bit_masks(PixelFormat format, uint64_t rgba[4]);

    /** Gives the bit shifts for R, G, B and A component
    @note           Only valid for native endian formats
    */
    static void get_bit_shifts(PixelFormat format, unsigned char rgba[4]);

    /** Gets the name of an image format
     */
    static const String& getFormatName(PixelFormat srcformat);

    /** Returns whether the format can be packed or unpacked with the pack_color()
    and unpack_color() functions. This is generally not true for compressed
    formats as they are special. It can only be true for formats with a
    fixed element size.
    */
    static bool is_accessible(PixelFormat src_format);

    /** Returns the component type for a certain pixel format. Returns PCT_BYTE
        in case there is no clear component type like with compressed formats.
        This is one of PCT_BYTE, PCT_SHORT, PCT_FLOAT16, PCT_FLOAT32.
    */
    static PixelComponentType get_component_type(PixelFormat fmt);

    /** Returns the component count for a certain pixel format. Returns 3(no alpha) or
        4 (has alpha) in case there is no clear component type like with compressed formats.
     */
    static uint8_t get_component_count(PixelFormat fmt);

    /** Gets the format from given name.
        @param  name            The string of format name
        @param  accessible_only  If true, non-accessible format will treat as invalid format,
                                otherwise, all supported format are valid.
        @param  caseSensitive   Should be set true if string match should use case sensitivity.
        @return                The format match the format name, or PF_UNKNOWN if is invalid name.
    */
    static PixelFormat get_format_from_name(const String& name, bool accessible_only = false, bool case_sensitive = false);

    /** Returns the similar format but according with given bit depths.
        @param fmt      The original format.
        @param integerBits Preferred bit depth (pixel bits) for integer pixel format.
                        Available values: 0, 16 and 32, where 0 (the default) means as it is.
        @param floatBits Preferred bit depth (channel bits) for float pixel format.
                        Available values: 0, 16 and 32, where 0 (the default) means as it is.
        @return        The format that similar original format with bit depth according
                        with preferred bit depth, or original format if no conversion occurring.
    */
    static PixelFormat get_format_for_bit_depths(PixelFormat fmt, ushort integer_bits, ushort float_bits);

    /** Pack a color value to memory
        @param color   The color
        @param pf       Pixelformat in which to write the color
        @param dest     Destination memory location
    */
    static void pack_color(const Color& color, const PixelFormat pf, void* dest)
    {
        pack_color(color.r, color.g, color.b, color.a, pf, dest);
    }
    /** Pack a color value to memory
        @param r,g,b,a  The four color components, range 0.0f to 1.0f
                        (an exception to this case exists for floating point pixel
                        formats, which don't clamp to 0.0f..1.0f)
        @param pf       Pixelformat in which to write the color
        @param dest     Destination memory location
    */
    static void
    pack_color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a, const PixelFormat pf, void* dest);
    /// @overload
    static void
    pack_color(const float r, const float g, const float b, const float a, const PixelFormat pf, void* dest);

    /// @overload
    static void unpack_color(Color* color, PixelFormat pf, const void* src)
    {
        unpack_color(&color->r, &color->g, &color->b, &color->a, pf, src);
    }
    /** Unpack a color value from memory
        @param r,g,b,a  The four color channels are returned here
        @param pf       Pixelformat in which to read the color
        @param src      Source memory location
    */
    static void unpack_color(float* r, float* g, float* b, float* a, PixelFormat pf, const void* src);
    /** @overload
        @note This function returns the color components in 8 bit precision,
            this will lose precision when coming from #PF_A2R10G10B10 or floating
            point formats.
    */
    static void unpack_color(uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a, PixelFormat pf, const void* src);

    /** Convert consecutive pixels from one format to another. No dithering or filtering is being done.
        Converting from RGB to luminance takes the R channel.  In case the source and destination format match,
        just a copy is done.
        @param  src         Pointer to source region
        @param  srcFormat   Pixel format of source region
        @param  dst         Pointer to destination region
        @param  dstFormat   Pixel format of destination region
        @param  count       The number of pixels to convert
     */
    static void
    bulk_pixel_conversion(void* src, PixelFormat srcFormat, void* dst, PixelFormat dstFormat, int count)
    {
        PixelBox _src(count, 1, 1, srcFormat, src);
        PixelBox _dst(count, 1, 1, dstFormat, dst);
        bulk_pixel_conversion(&_src, &_dst);
    }

    /** Convert pixels from one format to another. No dithering or filtering is being done. Converting
        from RGB to luminance takes the R channel.
        @param  src         PixelBox containing the source pixels, pitches and format
        @param  dst         PixelBox containing the destination pixels, pitches and format
        @remarks The source and destination boxes must have the same
        dimensions. In case the source and destination format match, a plain copy is done.
    */
    static void bulk_pixel_conversion(const PixelBox* src, const PixelBox* dst);

    /** Flips pixels inplace in vertical direction.
        @param  box         PixelBox containing pixels, pitches and format
        @remarks Non consecutive pixel boxes are supported.
     */
    static void bulk_pixel_vertical_flip(const PixelBox* box);
};

// inline const String& to_string(PixelFormat v) { return PixelUtil::getFormatName(v); }
/** @} */
/** @} */

} // namespace hyue

