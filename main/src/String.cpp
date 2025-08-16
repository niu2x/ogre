#include <hyue/String.h>

#include <boost/algorithm/string.hpp>

#include <algorithm>
#include <cstring>


#define strnicmp strncasecmp

namespace hyue {

String StringUtils::lower(const String &str) {
    return boost::to_lower_copy(str);
}

String StringUtils::upper(const String &str) {
    return boost::to_upper_copy(str);
}

void StringUtils::lower(String *str) {
    boost::to_lower(*str);
}

void StringUtils::upper(String *str) {
    boost::to_upper(*str);
}

void StringUtils::trim(String *str, bool left, bool right) {
    static const String delims = " \t\r\n";
    if(right)
        str->erase(str->find_last_not_of(delims)+1); // trim right
    if(left)
        str->erase(0, str->find_first_not_of(delims)); // trim left
}

String StringUtils::trim(const String &str, bool left, bool right) {
    String copy = str;
    trim(&copy, left, right);
    return copy;
}

bool StringUtils::starts_with(const String &str, const String &pattern, bool ignore_case) {
    if (pattern.empty())
        return false;

    if (ignore_case)
    {
        return strnicmp(str.c_str(), pattern.c_str(), pattern.size()) == 0;
    }

    return strncmp(str.c_str(), pattern.c_str(), pattern.size()) == 0;
}

bool StringUtils::ends_with(const String &str, const String &pattern, bool ignore_case) {
    if (pattern.empty())
        return false;

    if(str.size() < pattern.size())
        return false;

    size_t offset = str.size() - pattern.size();

    if (ignore_case)
    {
        return strnicmp(str.c_str() + offset, pattern.c_str(), pattern.size()) == 0;
    }

    return strncmp(str.c_str() + offset, pattern.c_str(), pattern.size()) == 0;
}

StringVector StringUtils::split(const String& str, const String& delims, bool compress_token) {
    StringVector split_vec;
    boost::split(split_vec, str, boost::is_any_of(delims), boost::token_compress_on);
    return split_vec;
}


String StringUtils::standardise_dir_path(const String& init)
{
    String path = init;
    std::replace( path.begin(), path.end(), '\\', '/' );
    if( path[path.length() - 1] != '/' )
        path += '/';
    return path;
}



}