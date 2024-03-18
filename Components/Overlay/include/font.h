/*-------------------------------------------------------------------------
This source file is a part of OGRE
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
THE SOFTWARE
-------------------------------------------------------------------------*/

#ifndef _Font_H__
#define _Font_H__

#include "OgreOverlayPrerequisites.h"
#include "resource.h"
#include "OgreCommon.h"

#include "colour_value.h"
#include "exception.h"

namespace Ogre {
class BillboardSet;

/** \addtogroup Optional
 *  @{
 */
/** \addtogroup Overlays
 *  @{
 */

/// decode UTF8 encoded bytestream to uint32 codepoints
std::vector<uint32> utftoc32(String str);

/** Enumerates the types of Font usable in the engine. */
enum FontType {
    /// Generated from a truetype (.ttf) font
    FT_TRUETYPE = 1,
    /// Loaded from an image created by an artist
    FT_IMAGE = 2
};

/// Information about the position and size of a glyph in a texture
struct GlyphInfo {
    using CodePoint = uint32;
    using UVRect = FloatRect;

    CodePoint code_point;
    UVRect uv_rect;
    float aspect_ratio; // width/ height
    float bearing; // bearingX/ height
    float advance; // advanceX/ height
};

/** Class representing a font in the system.

This class is simply a way of getting a font texture into the OGRE system and
to easily retrieve the texture coordinates required to accurately render them.
Fonts can either be loaded from precreated textures, or the texture can be
generated using a truetype font. You can either create the texture manually in
code, or you can use a .fontdef script to define it (probably more practical
since you can reuse the definition more easily)
@note
This class extends both Resource and ManualResourceLoader since it is
both a resource in it's own right, but it also provides the manual load
implementation for the Texture it creates.
*/
class Font final : public Resource, public ManualResourceLoader {
private:
    /// The type of font
    FontType type_;

    /// Source of the font (either an image name or a truetype font)
    String source_;

    /// Size of the truetype font, in points
    Real ttf_size_;
    /// Resolution (dpi) of truetype font
    uint ttf_resolution_;
    /// Max distance to baseline of this (truetype) font
    int ttf_max_bearing_y_;

public:
    using CodePoint = GlyphInfo::CodePoint;
    using UVRect = GlyphInfo::UVRect;
    /// A range of code points, inclusive on both ends
    using CodePointRange = std::pair<CodePoint, CodePoint>;
    using CodePointRangeList = std::vector<CodePointRange>;

private:
    /// Map from unicode code point to texture coordinates
    using CodePointMap = std::map<CodePoint, GlyphInfo>;
    CodePointMap code_point_map_;

    /// The material which is generated for this font
    MaterialPtr material_;

    /// Texture pointer
    TexturePtr texture_;

    /// For TRUE_TYPE font only
    bool antialias_colour_;

    /// Range of code points to generate glyphs for (truetype only)
    CodePointRangeList code_point_range_list_;

    /// Internal method for loading from ttf
    void create_texture_from_font(void);

    void load_impl() override;
    void unload_impl() override;
    size_t calculate_size(void) const override
    {
        return 0;
    } // permanent resource is in the texture
public:
    /** Constructor.
    @see Resource
    */
    Font(
        ResourceManager* creator,
        const String& name,
        ResourceHandle handle,
        const String& group,
        bool isManual = false,
        ManualResourceLoader* loader = 0);

    ~Font();

    /** Sets the type of font. Must be set before loading. */
    void set_type(FontType ftype);

    /** Gets the type of font. */
    FontType type(void) const;

    /** Sets the source of the font.

        If you have created a font of type FT_IMAGE, this method tells the
        Font which image to use as the source for the characters. So the
    parameter should be the name of an appropriate image file. Note that
    when using an image as a font source, you will also need to tell the
    font where each character is located using setGlyphTexCoords (for each
    character).
    @par
        If you have created a font of type FT_TRUETYPE, this method tells
    the Font which .ttf file to use to generate the text. You will also need
    to call setTrueTypeSize and setTrueTypeResolution, and call
    add_code_point_range as many times as required to define the range of
    glyphs you want to be available.
    @param source An image file or a truetype font, depending on the type of
    this font
    */
    void set_source(const String& source);

    /** Gets the source this font (either an image or a truetype font).
     */
    const String& source(void) const;

    /** Sets the size of a truetype font (only required for FT_TRUETYPE).
    @param ttfSize The size of the font in points. Note that the
        size of the font does not affect how big it is on the screen, just how
    large it is in the texture and thus how detailed it is.
    */
    void set_true_type_size(Real ttfSize);
    /** Gets the resolution (dpi) of the font used to generate the texture
    (only required for FT_TRUETYPE).
    @param ttfResolution The resolution in dpi
    */
    void set_true_type_resolution(uint ttfResolution);

    /** Gets the point size of the font used to generate the texture.

        Only applicable for FT_TRUETYPE Font objects.
        Note that the size of the font does not affect how big it is on the
       screen, just how large it is in the texture and thus how detailed it is.
    */
    Real true_type_size(void) const;
    /** Gets the resolution (dpi) of the font used to generate the texture.

        Only applicable for FT_TRUETYPE Font objects.
    */
    uint true_type_resolution(void) const;
    /** Gets the maximum baseline distance of all glyphs used in the texture.

        Only applicable for FT_TRUETYPE Font objects.
        The baseline is the vertical origin of horizontal based glyphs.  The
    bearingY attribute is the distance from the baseline (origin) to the top of
    the glyph's bounding box.
    @note
        This value is only available after the font has been loaded.
    */
    int true_type_max_bearing_y() const;

    /** Returns the texture coordinates of the associated glyph.
        @remarks Parameter is a short to allow both ASCII and wide chars.
        @param id The code point (unicode)
        @return A rectangle with the UV coordinates, or null UVs if the
            code point was not present
    */
    const UVRect& get_glyph_tex_coords(CodePoint id) const
    {
        return get_glyph_info(id).uv_rect;
    }

    /** Sets the texture coordinates of a glyph.

        You only need to call this if you're setting up a font loaded from a
    texture manually.
    @note
        Also sets the aspect ratio (width / height) of this character.
    textureAspect is the width/height of the texture (may be non-square)
    */
    void set_glyph_info_from_tex_coords(
        CodePoint id,
        const UVRect& rect,
        float textureAspect = 1.0)
    {
        auto glyphAspect = textureAspect * rect.width() / rect.height();
        set_glyph_info({ id, rect, glyphAspect, 0, glyphAspect });
    }

    void set_glyph_info(const GlyphInfo& info)
    {
        code_point_map_[info.code_point] = info;
    }

    /** Gets the aspect ratio (width / height) of this character. */
    float get_glyph_aspect_ratio(CodePoint id) const
    {
        return get_glyph_info(id).aspect_ratio;
    }
    /** Sets the aspect ratio (width / height) of this character.

        You only need to call this if you're setting up a font loaded from a
        texture manually.
    */
    void set_glyph_aspect_ratio(CodePoint id, Real ratio)
    {
        CodePointMap::iterator i = code_point_map_.find(id);
        if (i != code_point_map_.end()) {
            i->second.aspect_ratio = ratio;
        }
    }

    /** Gets the information available for a glyph corresponding to a
        given code point, or throws an exception if it doesn't exist;
    */
    const GlyphInfo& get_glyph_info(CodePoint id) const
    {
        CodePointMap::const_iterator i = code_point_map_.find(id);
        if (i == code_point_map_.end()) {
            // Try a fallback first.
            i = code_point_map_.find(static_cast<CodePoint>('?'));

            if (i == code_point_map_.end()) {
                OGRE_EXCEPT(
                    Exception::ERR_ITEM_NOT_FOUND,
                    StringUtil::format(
                        "Code point %d and fallback 63 not found in font %s",
                        id,
                        name().c_str()));
            }
        }
        return i->second;
    }

    /** Adds a range of code points to the list of code point ranges to generate
        glyphs for, if this is a truetype based font.

        In order to save texture space, only the glyphs which are actually
        needed by the application are generated into the texture. Before this
        object is loaded you must call this method as many times as necessary
        to define the code point range that you need.
    */
    void add_code_point_range(const CodePointRange& range)
    {
        code_point_range_list_.push_back(range);
    }

    /** Clear the list of code point ranges.
     */
    void clear_code_point_ranges() { code_point_range_list_.clear(); }
    /** Get a const reference to the list of code point ranges to be used to
        generate glyphs from a truetype font.
    */
    const CodePointRangeList& code_point_range_list() const
    {
        return code_point_range_list_;
    }
    /** Gets the material generated for this font, as a weak reference.

        This will only be valid after the Font has been loaded.
    */
    inline const MaterialPtr& material() const { return material_; }

    /**
     * Write a text into a BillboardSet for positioning in Space
     *
     * Text is laid out in the x-y plane, running into x+ and using y+ as up
     * @param bbs the target BillboardSet
     * @param text text to write
     * @param height character height in world units
     * @param colour text colour
     */
    void put_text(
        BillboardSet* bbs,
        String text,
        float height,
        const ColorValue& colour = ColorValue::White);

    /** Sets whether the colour of this font is multiplied with alpha as it is
       generated from a true type font.

        This is valid only for a FT_TRUETYPE font. If you are planning on using
        alpha blending to draw your font, then set this to
        false (which is the default), otherwise the darkening of the font will
       combine with the fading out of the alpha around the edges and make your
       font look thinner than it should. However, if you intend to use
       premultipled alpha or a colour blending mode (add or modulate for
       example) then set this to true, in order to soften your font edges.
    */
    inline void set_antialias_colour(bool enabled)
    {
        antialias_colour_ = enabled;
    }

    /** Gets whether or not the colour of this font is antialiased as it is
    generated from a true type font.
    */
    inline bool antialias_colour(void) const { return antialias_colour_; }

    /** Implementation of ManualResourceLoader::loadResource, called
        when the Texture that this font creates needs to (re)load.
    */
    void load_resource(Resource* resource) override;

    /** Manually set the material used for this font.

        This should only be used when the font is being loaded from a
        ManualResourceLoader.
    */
    void _set_material(const MaterialPtr& mat);
};

typedef SharedPtr<Font> FontPtr;
/** @} */
/** @} */
} // namespace Ogre

#endif
