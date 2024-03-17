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

#include "font.h"
#include "OgreMaterialManager.h"
#include "OgreTextureManager.h"
#include "OgreTexture.h"
#include "log_manager.h"
#include "string_interface.h"
#include "OgreTextureUnitState.h"
#include "OgreTechnique.h"
#include "bitwise.h"
#include "OgreOverlayManager.h"

#include "utf8.h"
#include "OgreBillboardSet.h"
#include "OgreBillboard.h"

#ifdef HAVE_FREETYPE
    #define generic _generic // keyword for C++/CX
    #include <ft2build.h>
    #include FT_FREETYPE_H
    #include FT_GLYPH_H
    #undef generic
#else
    #define STBTT_STATIC
    #define STB_TRUETYPE_IMPLEMENTATION
    #include "stb_truetype.h"
#endif

namespace Ogre {
//---------------------------------------------------------------------
namespace {
class CmdType : public ParamCommand {
public:
    String get(const void* target) const override;
    void set(void* target, const String& val) override;
};
class CmdSource : public ParamCommand {
public:
    String get(const void* target) const override;
    void set(void* target, const String& val) override;
};
class CmdSize : public ParamCommand {
public:
    String get(const void* target) const override;
    void set(void* target, const String& val) override;
};
class CmdResolution : public ParamCommand {
public:
    String get(const void* target) const override;
    void set(void* target, const String& val) override;
};
class CmdCodePoints : public ParamCommand {
public:
    String get(const void* target) const override;
    void set(void* target, const String& val) override;
};

// Command object for setting / getting parameters
static CmdType msTypeCmd;
static CmdSource msSourceCmd;
static CmdSize msSizeCmd;
static CmdResolution msResolutionCmd;
static CmdCodePoints msCodePointsCmd;
} // namespace

std::vector<uint32> utftoc32(String str)
{
    std::vector<uint32> decoded;
    decoded.reserve(str.size());

    str.resize(str.size() + 3); // add padding for decoder
    auto it = str.c_str();
    auto end = str.c_str() + str.size() - 3;
    while (it < end) {
        uint32 cpId;
        int err = 0;
        it = utf8_decode(it, &cpId, &err);
        if (err)
            continue;
        decoded.push_back(cpId);
    }
    return decoded;
}

//---------------------------------------------------------------------
Font::Font(
    ResourceManager* creator,
    const String& name,
    ResourceHandle handle,
    const String& group,
    bool isManual,
    ManualResourceLoader* loader)
: Resource(creator, name, handle, group, isManual, loader)
, type_(FT_TRUETYPE)
, ttf_size_(0)
, ttf_resolution_(0)
, ttf_max_bearing_y_(0)
, antialias_colour_(true)
{

    if (create_param_dictionary("Font")) {
        ParamDictionary* dict = param_dictionary();
        dict->add_parameter(
            ParameterDef(
                "type",
                "'truetype' or 'image' based font",
                ParameterType::STRING),
            &msTypeCmd);
        dict->add_parameter(
            ParameterDef(
                "source",
                "Filename of the source of the font.",
                ParameterType::STRING),
            &msSourceCmd);
        dict->add_parameter(
            ParameterDef("size", "True type size", ParameterType::REAL),
            &msSizeCmd);
        dict->add_parameter(
            ParameterDef(
                "resolution",
                "True type resolution",
                ParameterType::UNSIGNED_INT),
            &msResolutionCmd);
        dict->add_parameter(
            ParameterDef(
                "code_points",
                "Add a range of code points",
                ParameterType::STRING),
            &msCodePointsCmd);
    }
}
//---------------------------------------------------------------------
Font::~Font()
{
    // have to call this here reather than in Resource destructor
    // since calling virtual methods in base destructors causes crash
    unload();
}
//---------------------------------------------------------------------
void Font::set_type(FontType ftype) { type_ = ftype; }
//---------------------------------------------------------------------
FontType Font::type(void) const { return type_; }
//---------------------------------------------------------------------
void Font::set_source(const String& source) { source_ = source; }
//---------------------------------------------------------------------
void Font::set_true_type_size(Real ttfSize) { ttf_size_ = ttfSize; }
//---------------------------------------------------------------------
void Font::set_true_type_resolution(uint ttfResolution)
{
    ttf_resolution_ = ttfResolution;
}
//---------------------------------------------------------------------
const String& Font::source(void) const { return source_; }
//---------------------------------------------------------------------
Real Font::true_type_size(void) const { return ttf_size_; }
//---------------------------------------------------------------------
uint Font::true_type_resolution(void) const { return ttf_resolution_; }
//---------------------------------------------------------------------
int Font::true_type_max_bearing_y() const { return ttf_max_bearing_y_; }
//---------------------------------------------------------------------
void Font::_set_material(const MaterialPtr& mat) { material_ = mat; }

void Font::put_text(
    BillboardSet* bbs,
    String text,
    float height,
    const ColourValue& colour)
{
    // ensure loaded
    load();
    // configure Billboard for display
    bbs->setMaterial(material_);
    bbs->setBillboardType(BBT_PERPENDICULAR_COMMON);
    bbs->setBillboardOrigin(BBO_CENTER_LEFT);
    bbs->setDefaultDimensions(0, 0);

    text.resize(text.size() + 3); // add padding for decoder
    auto it = text.c_str();
    auto end = text.c_str() + text.size() - 3;

    const auto& bbox = bbs->getBoundingBox();

    float left = 0;
    float top
        = bbox == AxisAlignedBox::box_null ? 0 : bbox.minimum().y - height;
    while (it < end) {
        uint32 cpId;
        int err = 0;
        it = utf8_decode(it, &cpId, &err);
        if (err)
            continue;

        if (cpId == '\n') {
            top -= height;
            left = 0;
            continue;
        }

        const auto& cp = get_glyph_info(cpId);

        left += cp.bearing * height;

        if (!cp.uv_rect.is_null()) {
            auto bb = bbs->createBillboard(Vector3(left, top, 0), colour);
            bb->setDimensions(cp.aspect_ratio * height, height);
            bb->setTexcoordRect(cp.uv_rect);
        }

        left += (cp.advance - cp.bearing) * height;
    }
}

//---------------------------------------------------------------------
void Font::load_impl()
{
    // Create a new material
    material_ = MaterialManager::singleton().create("Fonts/" + name(), group());

    if (!material_) {
        OGRE_EXCEPT(
            Exception::ERR_INTERNAL_ERROR,
            "Error creating new material!",
            "Font::load");
    }

    if (type_ == FT_TRUETYPE) {
        create_texture_from_font();
    } else {
        // Manually load since we need to load to get alpha
        texture_ = TextureManager::singleton()
                       .load(source_, group(), TEX_TYPE_2D, 0);
    }

    // Make sure material is aware of colour per vertex.
    auto pass = material_->getTechnique(0)->getPass(0);
    pass->setVertexColourTracking(TVC_DIFFUSE);

    // lighting and culling also do not make much sense
    pass->setCullingMode(CULL_NONE);
    pass->setLightingEnabled(false);
    material_->setReceiveShadows(false);
    // font quads should not occlude things
    pass->setDepthWriteEnabled(false);

    TextureUnitState* texLayer
        = material_->getTechnique(0)->getPass(0)->createTextureUnitState();
    texLayer->setTexture(texture_);
    // Clamp to avoid fuzzy edges
    texLayer->setTextureAddressingMode(TextureUnitState::TAM_CLAMP);
    // Allow min/mag filter, but no mip
    texLayer->setTextureFiltering(FO_LINEAR, FO_LINEAR, FO_NONE);

    // Set up blending
    if (texture_->hasAlpha()) {
        material_->setSceneBlending(
            antialias_colour_ ? SceneBlendFactor::ONE
                              : SceneBlendFactor::SOURCE_ALPHA,
            SceneBlendFactor::ONE_MINUS_SOURCE_ALPHA);
        material_->getTechnique(0)->getPass(0)->setTransparentSortingEnabled(
            false);
    } else {
        // Use add if no alpha (assume black background)
        material_->setSceneBlending(SceneBlendType::ADD);
    }
}
//---------------------------------------------------------------------
void Font::unload_impl()
{
    if (material_) {
        MaterialManager::singleton().remove(material_);
        material_.reset();
    }

    if (texture_) {
        TextureManager::singleton().remove(texture_);
        texture_.reset();
    }
}
//---------------------------------------------------------------------
void Font::create_texture_from_font(void)
{
    // Just create the texture here, and point it at ourselves for when
    // it wants to (re)load for real
    texture_ = TextureManager::singleton()
                   .create(name() + "Texture", group(), true, this);
    texture_->setTextureType(TEX_TYPE_2D);
    texture_->setNumMipmaps(0);
    texture_->load();
}
//---------------------------------------------------------------------
void Font::load_resource(Resource* res)
{
    // Locate ttf file, load it pre-buffered into memory by wrapping the
    // original DataStream in a MemoryDataStream
    DataStreamPtr dataStreamPtr = ResourceGroupManager::singleton()
                                      .openResource(source_, group(), this);
    MemoryDataStream ttfchunk(dataStreamPtr);

    // If codepoints not supplied, assume ASCII
    if (code_point_range_list_.empty()) {
        code_point_range_list_.push_back(CodePointRange(32, 126));
    }
    float vpScale = OverlayManager::singleton().getPixelRatio();
#ifdef HAVE_FREETYPE
    // ManualResourceLoader implementation - load the texture
    FT_Library ftLibrary;
    // Init freetype
    if (FT_Init_FreeType(&ftLibrary))
        OGRE_EXCEPT(
            Exception::ERR_INTERNAL_ERROR,
            "Could not init FreeType library!",
            "Font::Font");

    FT_Face face;

    // Load font
    if (FT_New_Memory_Face(
            ftLibrary,
            ttfchunk.getPtr(),
            (FT_Long)ttfchunk.size(),
            0,
            &face))
        OGRE_EXCEPT(
            Exception::ERR_INTERNAL_ERROR,
            "Could not open font face!",
            "Font::create_texture_from_font");

    // Convert our point size to freetype 26.6 fixed point format
    FT_F26Dot6 ftSize = (FT_F26Dot6)(ttf_size_ * (1 << 6));
    if (FT_Set_Char_Size(
            face,
            ftSize,
            0,
            ttf_resolution_ * vpScale,
            ttf_resolution_ * vpScale))
        OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Could not set char size!");

    // FILE *fo_def = stdout;

    FT_Pos max_height = 0, max_width = 0;

    // Calculate maximum width, height and bearing
    size_t glyphCount = 0;
    for (const CodePointRange& range : code_point_range_list_) {
        for (CodePoint cp = range.first; cp <= range.second;
             ++cp, ++glyphCount) {
            FT_Load_Char(face, cp, FT_LOAD_RENDER);

            max_height = std::max<FT_Pos>(
                2 * face->glyph->bitmap.rows
                    - (face->glyph->metrics.horiBearingY >> 6),
                max_height);
            ttf_max_bearing_y_ = std::max(
                int(face->glyph->metrics.horiBearingY >> 6),
                ttf_max_bearing_y_);
            max_width = std::max<FT_Pos>(face->glyph->bitmap.width, max_width);
        }
    }
#else
    stbtt_fontinfo font;
    stbtt_InitFont(&font, ttfchunk.getPtr(), 0);
    // 64 gives the same texture resolution as freetype.
    float scale = stbtt_ScaleForPixelHeight(
        &font,
        vpScale * ttf_size_ * ttf_resolution_ / 64);

    int max_width = 0, max_height = 0;
    // Calculate maximum width, height and bearing
    size_t glyphCount = 0;
    for (const CodePointRange& range : code_point_range_list_) {
        for (CodePoint cp = range.first; cp <= range.second;
             ++cp, ++glyphCount) {
            int idx = stbtt_FindGlyphIndex(&font, cp);
            if (!idx) // It is actually in the font?
                continue;
            TRect<int> r;
            stbtt_GetGlyphBitmapBox(
                &font,
                idx,
                scale,
                scale,
                &r.left,
                &r.top,
                &r.right,
                &r.bottom);
            max_height = std::max(r.height(), max_height);
            ttf_max_bearing_y_ = std::max(-r.top, ttf_max_bearing_y_);
            max_width = std::max(r.width(), max_width);
        }
    }

    max_height *= 1.125;
#endif
    uint char_spacer = 1;

    // Now work out how big our texture needs to be
    size_t rawSize
        = (max_width + char_spacer) * (max_height + char_spacer) * glyphCount;

    uint32 tex_side = static_cast<uint32>(Math::Sqrt((Real)rawSize));
    // Now round up to nearest power of two
    uint32 roundUpSize = Bitwise::first_po2_from(tex_side);

    // Would we benefit from using a non-square texture (2X width)
    uint32 finalWidth, finalHeight;
    if (roundUpSize * roundUpSize * 0.5 >= rawSize) {
        finalHeight = static_cast<uint32>(roundUpSize * 0.5);
    } else {
        finalHeight = roundUpSize;
    }
    finalWidth = roundUpSize;

    Real textureAspect = (Real)finalWidth / (Real)finalHeight;

    Image img(PF_BYTE_LA, finalWidth, finalHeight);
    // Reset content (transparent)
    img.setTo(ColourValue::ZERO);

    uint32 l = 0, m = 0;
    for (const CodePointRange& range : code_point_range_list_) {
        for (CodePoint cp = range.first; cp <= range.second; ++cp) {
            uchar* buffer;
            int buffer_h = 0, buffer_pitch = 0;
#ifdef HAVE_FREETYPE
            // Load & render glyph
            FT_Error ftResult = FT_Load_Char(face, cp, FT_LOAD_RENDER);
            if (ftResult) {
                // problem loading this glyph, continue
                LogManager::singleton().log_error(StringUtil::format(
                    "Charcode %u is not in font %s",
                    cp,
                    source_.c_str()));
                continue;
            }

            buffer = face->glyph->bitmap.buffer;
            OgreAssertDbg(
                buffer
                    || (!face->glyph->bitmap.width
                        && !face->glyph->bitmap.rows),
                "attempting to load NULL buffer");

            uint advance = face->glyph->advance.x >> 6;
            uint width = face->glyph->bitmap.width;
            buffer_pitch = face->glyph->bitmap.pitch;
            buffer_h = face->glyph->bitmap.rows;

            FT_Pos y_bearing
                = ttf_max_bearing_y_ - (face->glyph->metrics.horiBearingY >> 6);
            FT_Pos x_bearing = face->glyph->metrics.horiBearingX >> 6;
#else
            int idx = stbtt_FindGlyphIndex(&font, cp);
            if (!idx) {
                LogManager::singleton().log_warning(StringUtil::format(
                    "Charcode %u is not in font %s",
                    cp,
                    source_.c_str()));
                continue;
            }

            if (cp == ' ') // should figure out how advance works for stbtt..
                idx = stbtt_FindGlyphIndex(&font, '0');

            TRect<int> r;
            stbtt_GetGlyphBitmapBox(
                &font,
                idx,
                scale,
                scale,
                &r.left,
                &r.top,
                &r.right,
                &r.bottom);

            uint width = r.width();

            int y_bearing = ttf_max_bearing_y_ + r.top;
            int xoff = 0, yoff = 0;
            buffer = stbtt_GetCodepointBitmap(
                &font,
                scale,
                scale,
                cp,
                &buffer_pitch,
                &buffer_h,
                &xoff,
                &yoff);

            int advance = xoff + width, x_bearing = xoff;
            // should be multiplied with scale, but still does not seem to do
            // the right thing stbtt_GetGlyphHMetrics(&font, cp, &advance,
            // &x_bearing);
#endif
            // If at end of row
            if (finalWidth - 1 < l + width) {
                m += max_height + char_spacer;
                l = 0;
            }

            for (int j = 0; j < buffer_h; j++) {
                uchar* pSrc = buffer + j * buffer_pitch;
                uint32 row = j + m + y_bearing;
                uchar* pDest = img.getData(l, row);
                for (unsigned int k = 0; k < width; k++) {
                    if (antialias_colour_) {
                        // Use the same greyscale pixel for all components RGBA
                        *pDest++ = *pSrc;
                    } else {
                        // Always white whether 'on' or 'off' pixel, since alpha
                        // will turn off
                        *pDest++ = 0xFF;
                    }
                    // Always use the greyscale value for alpha
                    *pDest++ = *pSrc++;
                }
            }

            UVRect uvs(
                (Real)l / (Real)finalWidth, // u1
                (Real)m / (Real)finalHeight, // v1
                (Real)(l + width) / (Real)finalWidth, // u2
                (m + max_height) / (Real)finalHeight); // v2
            this->set_glyph_info(
                { cp,
                  uvs,
                  float(textureAspect * uvs.width() / uvs.height()),
                  float(x_bearing) / max_height,
                  float(advance) / max_height });

            // Advance a column
            if (width)
                l += (width + char_spacer);
        }
    }
#ifdef HAVE_FREETYPE
    FT_Done_FreeType(ftLibrary);
#endif
    Texture* tex = static_cast<Texture*>(res);
    // Call internal _loadImages, not loadImage since that's external and
    // will determine load status etc again, and this is a manual loader inside
    // load()
    tex->_loadImages({ &img });
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
String CmdType::get(const void* target) const
{
    const Font* f = static_cast<const Font*>(target);
    if (f->type() == FT_TRUETYPE) {
        return "truetype";
    } else {
        return "image";
    }
}
void CmdType::set(void* target, const String& val)
{
    Font* f = static_cast<Font*>(target);
    if (val == "truetype") {
        f->set_type(FT_TRUETYPE);
    } else {
        f->set_type(FT_IMAGE);
    }
}
//-----------------------------------------------------------------------
String CmdSource::get(const void* target) const
{
    const Font* f = static_cast<const Font*>(target);
    return f->source();
}
void CmdSource::set(void* target, const String& val)
{
    Font* f = static_cast<Font*>(target);
    f->set_source(val);
}
//-----------------------------------------------------------------------
String CmdSize::get(const void* target) const
{
    const Font* f = static_cast<const Font*>(target);
    return StringConverter::to_string(f->true_type_size());
}
void CmdSize::set(void* target, const String& val)
{
    Font* f = static_cast<Font*>(target);
    f->set_true_type_size(StringConverter::parse_real(val));
}
//-----------------------------------------------------------------------
String CmdResolution::get(const void* target) const
{
    const Font* f = static_cast<const Font*>(target);
    return StringConverter::to_string(f->true_type_resolution());
}
void CmdResolution::set(void* target, const String& val)
{
    Font* f = static_cast<Font*>(target);
    f->set_true_type_resolution(StringConverter::parse_uint32(val));
}
//-----------------------------------------------------------------------
String CmdCodePoints::get(const void* target) const
{
    const Font* f = static_cast<const Font*>(target);
    StringStream str;
    for (const auto& i : f->code_point_range_list()) {
        str << i.first << "-" << i.second << " ";
    }
    return str.str();
}
void CmdCodePoints::set(void* target, const String& val)
{
    // Format is "code_points start1-end1 start2-end2"
    Font* f = static_cast<Font*>(target);

    StringVector vec = StringUtil::split(val, " \t");
    for (auto& item : vec) {
        StringVector itemVec = StringUtil::split(item, "-");
        if (itemVec.size() == 2) {
            f->add_code_point_range(
                { StringConverter::parse_uint32(itemVec[0]),
                  StringConverter::parse_uint32(itemVec[1]) });
        }
    }
}

} // namespace Ogre
