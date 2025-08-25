#pragma once

#include <cstddef>
#include <cstdint>

#include <limits>
#include <memory>

#include <iosfwd>

#include <filesystem>

#include <string>

#include <any>
#include <vector>
#include <map>
#include <list>

#include <boost/predef.h>

#include <hyue/export.h>

#define HYUE_ENDIAN_BIG    BOOST_ENDIAN_BIG_BYTE
#define HYUE_ENDIAN_LITTLE BOOST_ENDIAN_LITTLE_BYTE

#if !(HYUE_ENDIAN_BIG || HYUE_ENDIAN_LITTLE)
    #error "unknown endian"
#endif

#define HYUE_OS_WINDOWS BOOST_OS_WINDOWS

namespace hyue {

using String = std::string;
using StringVector = std::vector<String>;

using Any = std::any;
using std::any_cast;

template<class T>
using SharedPtr = std::shared_ptr<T>;

namespace std_fs = std::filesystem;
using FilePath = std::filesystem::path;

} // namespace hyue
