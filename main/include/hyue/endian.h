#pragma once

#include <boost/predef/other/endian.h>

#if BOOST_ENDIAN_BIG_BYTE
    #define HYUE_ENDIAN_BIG 1
#elif BOOST_ENDIAN_LITTLE_BYTE
    #define HYUE_ENDIAN_LITTLE 1
#else
    #error "Unknown Endian"
#endif