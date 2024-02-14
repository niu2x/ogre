//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#ifndef XDOG_MURMUR_HASH3_H
#define XDOG_MURMUR_HASH3_H

//-----------------------------------------------------------------------------
// Platform-specific functions and macros
#include "OgrePlatform.h"

#include <cstddef>
#include <stdint.h>

//-----------------------------------------------------------------------------

namespace Ogre
{
    void murmur_hash3_x86_32  ( const void * key, size_t len, uint32_t seed, void * out );

    void murmur_hash3_x86_128 ( const void * key, size_t len, uint32_t seed, void * out );

    void murmur_hash3_x64_128 ( const void * key, size_t len, uint32_t seed, void * out );

    inline void murmur_hash3_128( const void * key, size_t len, uint32_t seed, void * out ) {
#if XDOG_ARCH_TYPE == OGRE_ARCHITECTURE_64
        murmur_hash3_x64_128(key, len, seed, out);
#else
        murmur_hash3_x86_128(key, len, seed, out);
#endif
    }
}

//-----------------------------------------------------------------------------

#endif // XDOG_MURMUR_HASH3_H
