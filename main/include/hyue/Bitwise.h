#pragma once

#include <hyue/type.h>
#include <hyue/endian.h>

#ifdef bswap16
    #undef bswap16
    #undef bswap32
    #undef bswap64
#endif

namespace hyue {

class HYUE_API Bitwise {
public:
    /** Returns value with reversed bytes order.
     */
    static inline uint16_t bswap16(uint16_t arg)
    {
        return __builtin_bswap16(arg);
        // return ((arg << 8) & 0xFF00) | ((arg >> 8) & 0x00FF);
    }
    /** Returns value with reversed bytes order.
     */
    static inline uint32_t bswap32(uint32_t arg)
    {
        return __builtin_bswap32(arg);
        // return ((arg & 0x000000FF) << 24) | ((arg & 0x0000FF00) << 8) | ((arg >> 8) & 0x0000FF00) | ((arg >> 24) &
        // 0x000000FF);
    }
    /** Returns value with reversed bytes order.
     */
    static inline uint64_t bswap64(uint64_t arg)
    {
        return __builtin_bswap64(arg);
        // union {
        //     uint64_t sv;
        //     uint32_t ul[2];
        // } tmp, result;
        // tmp.sv = arg;
        // result.ul[0] = bswap32(tmp.ul[1]);
        // result.ul[1] = bswap32(tmp.ul[0]);
        // return result.sv;
    }

    /** Reverses byte order of buffer. Use bswap16/32/64 instead if possible.
     */
    static inline void bswap_buffer(void* p_data, size_t size)
    {
        char swap_byte;
        for (char *p0 = (char*)p_data, *p1 = p0 + size - 1; p0 < p1; ++p0, --p1) {
            swap_byte = *p0;
            *p0 = *p1;
            *p1 = swap_byte;
        }
    }
    /** Reverses byte order of chunks in buffer, where 'size' is size of one chunk.
     */
    static inline void bswap_chunks(void* p_data, size_t size, size_t count)
    {
        for (size_t c = 0; c < count; ++c) {
            char swap_byte;
            for (char *p0 = (char*)p_data + c * size, *p1 = p0 + size - 1; p0 < p1; ++p0, --p1) {
                swap_byte = *p0;
                *p0 = *p1;
                *p1 = swap_byte;
            }
        }
    }

    /** Returns the most significant bit set in a value.
     */
    static inline int get_most_significant_bit_set(uint32_t value)
    {
        //                                     0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, B, C, D, E, F
        static const unsigned char msb[16] = { 0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4 };

        unsigned int result = 0;
        if (value & 0xFFFF0000) {
            result += 16;
            value >>= 16;
        }
        if (value & 0x0000FF00) {
            result += 8;
            value >>= 8;
        }
        if (value & 0x000000F0) {
            result += 4;
            value >>= 4;
        }
        result += msb[value];
        return result - 1;
    }
    /** Returns the closest power-of-two number greater or equal to value.
        @note 0 and 1 are powers of two, so
            firstPO2From(0)==0 and firstPO2From(1)==1.
    */
    static inline uint32_t get_first_pow2_from(uint32_t n)
    {
        --n;
        n |= n >> 16;
        n |= n >> 8;
        n |= n >> 4;
        n |= n >> 2;
        n |= n >> 1;
        ++n;
        return n;
    }
    /** Determines whether the number is power-of-two or not.
        @note 0 and 1 are treat as power of two.
    */
    template <typename T>
    static inline bool is_pow2(T n)
    {
        return (n & (n - 1)) == 0;
    }
    /** Returns the number of bits a pattern must be shifted right by to
        remove right-hand zeros.
    */
    template <typename T>
    static inline unsigned int get_bit_shift(T mask)
    {
        if (mask == 0)
            return 0;

        unsigned int result = 0;
        while ((mask & 1) == 0) {
            ++result;
            mask >>= 1;
        }
        return result;
    }

    /** Takes a value with a given src bit mask, and produces another
        value with a desired bit mask.

        This routine is useful for colour conversion.
    */
    template <typename SrcT, typename DestT>
    static inline DestT convert_bit_pattern(SrcT src_value, SrcT src_bitmask, DestT dest_bitmask)
    {
        // Mask off irrelevant source value bits (if any)
        src_value = src_value & src_bitmask;

        // Shift source down to bottom of DWORD
        auto src_bit_shift = get_bit_shift(src_bitmask);
        src_value >>= src_bit_shift;

        // Get max value possible in source from srcMask
        const SrcT src_max = src_bitmask >> src_bit_shift;

        // Get max available in dest
        auto dest_bit_shift = get_bit_shift(dest_bitmask);
        const DestT dest_max = dest_bitmask >> dest_bit_shift;

        // Scale source value into destination, and shift back
        DestT destValue = (src_value * dest_max) / src_max;
        return (destValue << dest_bit_shift);
    }

    /**
     * Convert N bit colour channel value to P bits. It fills P bits with the
     * bit pattern repeated. (this is /((1<<n)-1) in fixed point)
     */
    static inline unsigned int fixed_to_fixed(uint32_t value, unsigned int n, unsigned int p)
    {
        if (n > p) {
            // Less bits required than available; this is easy
            value >>= n - p;
        } else if (n < p) {
            // More bits required than are there, do the fill
            // Use old fashioned division, probably better than a loop
            if (value == 0)
                value = 0;
            else if (value == (static_cast<unsigned int>(1) << n) - 1)
                value = (1 << p) - 1;
            else
                value = value * (1 << p) / ((1 << n) - 1);
        }
        return value;
    }

    /**
     * Convert floating point colour channel value between 0.0 and 1.0 (otherwise clamped)
     * to integer of a certain number of bits. Works for any value of bits between 0 and 31.
     */
    static inline unsigned int float_to_fixed(const float value, const unsigned int bits)
    {
        if (value <= 0.0f)
            return 0;
        else if (value >= 1.0f)
            return (1 << bits) - 1;
        else
            return (unsigned int)(value * float(1 << bits));
    }

    /**
     * Fixed point to float
     */
    static inline float fixed_to_float(unsigned value, unsigned int bits)
    {
        return (float)value / (float)((1 << bits) - 1);
    }

    /**
     * Write a n*8 bits integer value to memory in native endian.
     */
    static inline void int_write(void* dest, const int n, const uint32_t value)
    {
        switch (n) {
            case 1:
                ((uint8_t*)dest)[0] = (uint8_t)value;
                break;
            case 2:
                ((uint16_t*)dest)[0] = (uint16_t)value;
                break;
            case 3:
#if HYUE_ENDIAN_BIG
                ((uint8_t*)dest)[0] = (uint8_t)((value >> 16) & 0xFF);
                ((uint8_t*)dest)[1] = (uint8_t)((value >> 8) & 0xFF);
                ((uint8_t*)dest)[2] = (uint8_t)(value & 0xFF);
#else
                ((uint8_t*)dest)[2] = (uint8_t)((value >> 16) & 0xFF);
                ((uint8_t*)dest)[1] = (uint8_t)((value >> 8) & 0xFF);
                ((uint8_t*)dest)[0] = (uint8_t)(value & 0xFF);
#endif
                break;
            case 4:
                ((uint32_t*)dest)[0] = (uint32_t)value;
                break;
        }
    }
    /**
     * Read a n*8 bits integer value to memory in native endian.
     */
    static inline uint32_t int_read(const void* src, int n)
    {
        switch (n) {
            case 1:
                return ((const uint8_t*)src)[0];
            case 2:
                return ((const uint16_t*)src)[0];
            case 3:
#if HYUE_ENDIAN_BIG
                return ((uint32_t)((const uint8_t*)src)[0] << 16) | ((uint32_t)((const uint8_t*)src)[1] << 8)
                       | ((uint32_t)((const uint8_t*)src)[2]);
#else
                return ((uint32_t)((const uint8_t*)src)[0]) | ((uint32_t)((const uint8_t*)src)[1] << 8)
                       | ((uint32_t)((const uint8_t*)src)[2] << 16);
#endif
            case 4:
                return ((const uint32_t*)src)[0];
        }
        return 0; // ?
    }

    /** Convert a float32 to a float16 (NV_half_float)
        Courtesy of meshoptimizer
    */
    static inline uint16_t float_to_half(float i)
    {
        union {
            float f;
            uint32_t i;
        } v;
        v.f = i;
        return float_to_half_u32(v.i);
    }

    /**
     * Convert a float16 (NV_half_float) to a float32
     * Courtesy of meshoptimizer
     */
    static inline float half_to_float(uint16_t y)
    {
        union {
            float f;
            uint32_t i;
        } v;
        v.i = half_to_float_u16(y);
        return v.f;
    }

private:
    /** Converts float in uint32_t format to a a half in uint16_t format
     */
    static inline uint16_t float_to_half_u32(uint32_t ui)
    {
        int s = (ui >> 16) & 0x8000;
        int em = ui & 0x7fffffff;

        // bias exponent and round to nearest; 112 is relative exponent bias (127-15)
        int h = (em - (112 << 23) + (1 << 12)) >> 13;

        // underflow: flush to zero; 113 encodes exponent -14
        h = (em < (113 << 23)) ? 0 : h;

        // overflow: infinity; 143 encodes exponent 16
        h = (em >= (143 << 23)) ? 0x7c00 : h;

        // NaN; note that we convert all types of NaN to qNaN
        h = (em > (255 << 23)) ? 0x7e00 : h;

        return (unsigned short)(s | h);
    }

    /** Converts a half in uint16_t format to a float
        in uint32_t format
     */
    static inline uint32_t half_to_float_u16(uint16_t h)
    {
        unsigned int s = unsigned(h & 0x8000) << 16;
        int em = h & 0x7fff;

        // bias exponent and pad mantissa with 0; 112 is relative exponent bias (127-15)
        int r = (em + (112 << 10)) << 13;

        // denormal: flush to zero
        r = (em < (1 << 10)) ? 0 : r;

        // infinity/NaN; note that we preserve NaN payload as a byproduct of unifying inf/nan cases
        // 112 is an exponent bias fixup; since we already applied it once, applying it twice converts 31 to 255
        r += (em >= (31 << 10)) ? (112 << 23) : 0;

        return s | r;
    }
};
/** @} */
/** @} */

} // namespace hyue
