#pragma once

#include <cstddef>
#include <cstdint>

#include <limits>
#include <memory>

#include <iosfwd>

#include <string>
#include <any>

#include <vector>
#include <map>
#include <list>

#include <boost/predef/other/endian.h>

#include <hyue/export.h>

#if BOOST_ENDIAN_BIG_BYTE
    #define HYUE_ENDIAN_BIG 1
#elif BOOST_ENDIAN_LITTLE_BYTE
    #define HYUE_ENDIAN_LITTLE 1
#else
    #error "Unknown Endian"
#endif

namespace hyue {

using String = std::string;
using StringVector = std::vector<String>;
using StringVectorPtr = std::shared_ptr<StringVector>;

using Any = std::any;
using std::any_cast;

template<class T>
using SharedPtr = std::shared_ptr<T>;

} // namespace hyue