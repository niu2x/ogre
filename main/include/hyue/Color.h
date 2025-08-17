#pragma once

#include <cstdint>
#include <ostream>

#include <hyue/export.h>
#include <hyue/endian.h>

namespace hyue {
/**
 * R在高位(0xFF000000)、A在低位(0x000000FF)
 */
using RGBA = uint32_t;
using ARGB = uint32_t;
using ABGR = uint32_t;
using BGRA = uint32_t;

class HYUE_API Color
{
public:
    static const Color &ZERO;
    static const Color &BLACK;
    static const Color &WHITE;
    static const Color &RED;
    static const Color &GREEN;
    static const Color &BLUE;

    explicit constexpr Color( float red = 1.0f,
                float green = 1.0f,
                float blue = 1.0f,
                float alpha = 1.0f ) : r(red), g(green), b(blue), a(alpha)
    { }

    explicit Color(const uint8_t* byte) : r(byte[0]), g(byte[1]), b(byte[2]), a(byte[3])
    {
        *this /= 255;
    }

    bool operator==(const Color& rhs) const
    {
        return (r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a);
    }

    bool operator!=(const Color& rhs) const { return !(*this == rhs); }

    RGBA get_RGBA(void) const;

    ARGB get_ARGB(void) const;

    BGRA get_BGRA(void) const;

    ABGR get_ABGR(void) const;

    RGBA get_BYTE() const
    {
#if HYUE_ENDIAN_BIG
        return get_RGBA();
#else
        return get_ABGR();
#endif
    }

    void set_RGBA(RGBA val);

    void set_ARGB(ARGB val);

    void set_BGRA(BGRA val);

    void set_ABGR(ABGR val);

    void saturate(void)
    {
        if (r < 0)
            r = 0;
        else if (r > 1)
            r = 1;

        if (g < 0)
            g = 0;
        else if (g > 1)
            g = 1;

        if (b < 0)
            b = 0;
        else if (b > 1)
            b = 1;

        if (a < 0)
            a = 0;
        else if (a > 1)
            a = 1;
    }

    Color saturate_copy() const
    {
        Color ret = *this;
        ret.saturate();
        return ret;
    }

    float operator [] ( const size_t i ) const
    {
        return *(&r+i);
    }

    float& operator [] ( const size_t i )
    {
        return *(&r+i);
    }

    float* get_ptr()
    {
        return &r;
    }

    const float* get_ptr() const
    {
        return &r;
    }

    
    Color operator + ( const Color& rk_vector ) const
    {
        Color k_sum;

        k_sum.r = r + rk_vector.r;
        k_sum.g = g + rk_vector.g;
        k_sum.b = b + rk_vector.b;
        k_sum.a = a + rk_vector.a;

        return k_sum;
    }

    Color operator - ( const Color& rk_vector ) const
    {
        Color k_diff;

        k_diff.r = r - rk_vector.r;
        k_diff.g = g - rk_vector.g;
        k_diff.b = b - rk_vector.b;
        k_diff.a = a - rk_vector.a;

        return k_diff;
    }

    Color operator * (const float f_scalar ) const
    {
        Color k_prod;

        k_prod.r = f_scalar*r;
        k_prod.g = f_scalar*g;
        k_prod.b = f_scalar*b;
        k_prod.a = f_scalar*a;

        return k_prod;
    }

    Color operator * ( const Color& rhs) const
    {
        Color k_prod;

        k_prod.r = rhs.r * r;
        k_prod.g = rhs.g * g;
        k_prod.b = rhs.b * b;
        k_prod.a = rhs.a * a;

        return k_prod;
    }

    Color operator / ( const Color& rhs) const
    {
        Color k_prod;

        k_prod.r = r / rhs.r;
        k_prod.g = g / rhs.g;
        k_prod.b = b / rhs.b;
        k_prod.a = a / rhs.a;

        return k_prod;
    }

    Color operator / (const float f_scalar ) const
    {
        Color k_div;
        float f_inv = 1.0f / f_scalar;
        k_div.r = r * f_inv;
        k_div.g = g * f_inv;
        k_div.b = b * f_inv;
        k_div.a = a * f_inv;

        return k_div;
    }

    friend Color operator * (const float f_scalar, const Color& rk_vector )
    {
        Color k_prod;

        k_prod.r = f_scalar * rk_vector.r;
        k_prod.g = f_scalar * rk_vector.g;
        k_prod.b = f_scalar * rk_vector.b;
        k_prod.a = f_scalar * rk_vector.a;

        return k_prod;
    }

    // arithmetic updates
    Color& operator += ( const Color& rk_vector )
    {
        r += rk_vector.r;
        g += rk_vector.g;
        b += rk_vector.b;
        a += rk_vector.a;

        return *this;
    }

    Color& operator -= ( const Color& rk_vector )
    {
        r -= rk_vector.r;
        g -= rk_vector.g;
        b -= rk_vector.b;
        a -= rk_vector.a;

        return *this;
    }

    Color& operator *= (const float f_scalar )
    {
        r *= f_scalar;
        g *= f_scalar;
        b *= f_scalar;
        a *= f_scalar;
        return *this;
    }

    Color& operator /= (const float f_scalar )
    {
        float f_inv = 1.0f / f_scalar;

        r *= f_inv;
        g *= f_inv;
        b *= f_inv;
        a *= f_inv;

        return *this;
    }

    void set_HSB(float hue, float saturation, float brightness);

    void get_HSB(float* hue, float* saturation, float* brightness) const;

    inline friend std::ostream& operator << ( std::ostream& o, const Color& c )
    {
        o << "Color(" << c.r << ", " << c.g << ", " << c.b << ", " << c.a << ")";
        return o;
    }

public:
    float r,g,b,a;
};


}