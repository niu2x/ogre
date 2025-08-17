#include <hyue/Color.h>

#include <cmath>

#include <hyue/math.h>

namespace hyue {

namespace {

constexpr Color g_color_zero(0.0, 0.0, 0.0, 0.0);
constexpr Color g_color_black(0.0, 0.0, 0.0);
constexpr Color g_color_white(1.0, 1.0, 1.0);
constexpr Color g_color_red(1.0, 0.0, 0.0);
constexpr Color g_color_green(0.0, 1.0, 0.0);
constexpr Color g_color_blue(0.0, 0.0, 1.0);

} 

const Color &Color::ZERO = g_color_zero;
const Color &Color::BLACK = g_color_black;
const Color &Color::WHITE = g_color_white;
const Color &Color::RED = g_color_red;
const Color &Color::GREEN = g_color_green;
const Color &Color::BLUE = g_color_blue;

//---------------------------------------------------------------------
#if HYUE_ENDIAN_BIG
ABGR Color::get_ABGR(void) const
#else
RGBA Color::get_RGBA(void) const
#endif
{
    uint8_t val8;
    uint32_t val32 = 0;

    // Red
    val8 = static_cast<uint8_t>(r * 255);
    val32 = val8 << 24;

    // Green
    val8 = static_cast<uint8_t>(g * 255);
    val32 += val8 << 16;

    // Blue
    val8 = static_cast<uint8_t>(b * 255);
    val32 += val8 << 8;

    // Alpha
    val8 = static_cast<uint8_t>(a * 255);
    val32 += val8;

    return val32;
}
//---------------------------------------------------------------------
#if HYUE_ENDIAN_BIG
BGRA Color::get_BGRA(void) const
#else
ARGB Color::get_ARGB(void) const
#endif
{
    uint8_t val8;
    uint32_t val32 = 0;

    // Convert to 32bit pattern
    // (ARGB = 8888)

    // Alpha
    val8 = static_cast<uint8_t>(a * 255);
    val32 = val8 << 24;

    // Red
    val8 = static_cast<uint8_t>(r * 255);
    val32 += val8 << 16;

    // Green
    val8 = static_cast<uint8_t>(g * 255);
    val32 += val8 << 8;

    // Blue
    val8 = static_cast<uint8_t>(b * 255);
    val32 += val8;


    return val32;
}
//---------------------------------------------------------------------
#if HYUE_ENDIAN_BIG
ARGB Color::get_ARGB(void) const
#else
BGRA Color::get_BGRA(void) const
#endif
{
    uint8_t val8;
    uint32_t val32 = 0;

    // Convert to 32bit pattern
    // (ARGB = 8888)

    // Blue
    val8 = static_cast<uint8_t>(b * 255);
    val32 = val8 << 24;

    // Green
    val8 = static_cast<uint8_t>(g * 255);
    val32 += val8 << 16;

    // Red
    val8 = static_cast<uint8_t>(r * 255);
    val32 += val8 << 8;

    // Alpha
    val8 = static_cast<uint8_t>(a * 255);
    val32 += val8;


    return val32;
}
//---------------------------------------------------------------------
#if HYUE_ENDIAN_BIG
RGBA Color::get_RGBA(void) const
#else
ABGR Color::get_ABGR(void) const
#endif
{
    uint8_t val8;
    uint32_t val32 = 0;

    // Convert to 32bit pattern
    // (ABRG = 8888)

    // Alpha
    val8 = static_cast<uint8_t>(a * 255);
    val32 = val8 << 24;

    // Blue
    val8 = static_cast<uint8_t>(b * 255);
    val32 += val8 << 16;

    // Green
    val8 = static_cast<uint8_t>(g * 255);
    val32 += val8 << 8;

    // Red
    val8 = static_cast<uint8_t>(r * 255);
    val32 += val8;


    return val32;
}
//---------------------------------------------------------------------
#if HYUE_ENDIAN_BIG
void Color::set_ABGR(ABGR val32)
#else
void Color::set_RGBA(RGBA val32)
#endif
{
    // Convert from 32bit pattern
    // (RGBA = 8888)

    // Red
    r = float((val32 >> 24) & 0xFF) / 255.0f;

    // Green
    g = float((val32 >> 16) & 0xFF) / 255.0f;

    // Blue
    b = float((val32 >> 8) & 0xFF) / 255.0f;

    // Alpha
    a = float(val32 & 0xFF) / 255.0f;
}
//---------------------------------------------------------------------
#if HYUE_ENDIAN_BIG
void Color::set_BGRA(BGRA val32)
#else
void Color::set_ARGB(ARGB val32)
#endif
{
    // Convert from 32bit pattern
    // (ARGB = 8888)

    // Alpha
    a = float((val32 >> 24) & 0xFF) / 255.0f;

    // Red
    r = float((val32 >> 16) & 0xFF) / 255.0f;

    // Green
    g = float((val32 >> 8) & 0xFF) / 255.0f;

    // Blue
    b = float(val32 & 0xFF) / 255.0f;
}
//---------------------------------------------------------------------
#if HYUE_ENDIAN_BIG
void Color::set_ARGB(ARGB val32)
#else
void Color::set_BGRA(BGRA val32)
#endif
{
    // Convert from 32bit pattern
    // (ARGB = 8888)

    // Blue
    b = float((val32 >> 24) & 0xFF) / 255.0f;

    // Green
    g = float((val32 >> 16) & 0xFF) / 255.0f;

    // Red
    r = float((val32 >> 8) & 0xFF) / 255.0f;

    // Alpha
    a = float(val32 & 0xFF) / 255.0f;
}
//---------------------------------------------------------------------
#if HYUE_ENDIAN_BIG
void Color::set_RGBA(RGBA val32)
#else
void Color::set_ABGR(ABGR val32)
#endif
{
    // Convert from 32bit pattern
    // (ABGR = 8888)

    // Alpha
    a = float((val32 >> 24) & 0xFF) / 255.0f;

    // Blue
    b = float((val32 >> 16) & 0xFF) / 255.0f;

    // Green
    g = float((val32 >> 8) & 0xFF) / 255.0f;

    // Red
    r = float(val32 & 0xFF) / 255.0f;
}
//---------------------------------------------------------------------
void Color::set_HSB(float hue, float saturation, float brightness)
{
    hue = std::fmod(hue, 1.0f);

    saturation = math::saturate(saturation);
    brightness = math::saturate(brightness);

    if (brightness == 0.0f)
    {   
        // early exit, this has to be black
        r = g = b = 0.0f;
        return;
    }

    if (saturation == 0.0f)
    {   
        // early exit, this has to be grey

        r = g = b = brightness;
        return;
    }


    float hue_domain  = hue * 6.0f;
    if (hue_domain >= 6.0f)
    {
        // wrap around, and allow mathematical errors
        hue_domain = 0.0f;
    }
    uint16_t domain = (uint16_t)hue_domain;
    float f1 = brightness * (1 - saturation);
    float f2 = brightness * (1 - saturation * (hue_domain - domain));
    float f3 = brightness * (1 - saturation * (1 - (hue_domain - domain)));

    switch (domain)
    {
    case 0:
        // red domain; green ascends
        r = brightness;
        g = f3;
        b = f1;
        break;
    case 1:
        // yellow domain; red descends
        r = f2;
        g = brightness;
        b = f1;
        break;
    case 2:
        // green domain; blue ascends
        r = f1;
        g = brightness;
        b = f3;
        break;
    case 3:
        // cyan domain; green descends
        r = f1;
        g = f2;
        b = brightness;
        break;
    case 4:
        // blue domain; red ascends
        r = f3;
        g = f1;
        b = brightness;
        break;
    case 5:
        // magenta domain; blue descends
        r = brightness;
        g = f1;
        b = f2;
        break;
    }


}
//---------------------------------------------------------------------
void Color::get_HSB(float* hue, float* saturation, float* brightness) const
{
    float v_min = std::min(r, std::min(g, b));
    float v_max = std::max(r, std::max(g, b));
    float delta = v_max - v_min;

    *brightness = v_max;

    if (math::equal(delta, 0.0f, 1e-6f))
    {
        // grey
        *hue = 0;
        *saturation = 0;
    }
    else                                    
    {
        // a colour
        *saturation = delta / v_max;

        float delta_R = (((v_max - r) / 6.0f) + (delta / 2.0f)) / delta;
        float delta_G = (((v_max - g) / 6.0f) + (delta / 2.0f)) / delta;
        float delta_B = (((v_max - b) / 6.0f) + (delta / 2.0f)) / delta;

        if (math::equal(r, v_max))
            *hue = delta_B - delta_G;
        else if (math::equal(g, v_max))
            *hue = 0.3333333f + delta_R - delta_B;
        else if (math::equal(b, v_max)) 
            *hue = 0.6666667f + delta_G - delta_R;

        if (*hue < 0.0f)
            *hue += 1.0f;
        if (*hue > 1.0f)
            *hue -= 1.0f;
    }
}


}



