#include <hyue/String.h>

#include <algorithm>
#include <cstring>

#define strnicmp strncasecmp

namespace hyue {

String StringUtils::to_lower(const String &str) {
    String copy = str;
    lower(&copy);
    return copy;
}

String StringUtils::to_upper(const String &str) {
    String copy = str;
    upper(&copy);
    return copy;
}

void StringUtils::lower(String *str) {
    std::transform(
        str->begin(),
        str->end(),
        str->begin(),
        tolower);
}

void StringUtils::upper(String *str) {
    std::transform(
        str->begin(),
        str->end(),
        str->begin(),
        toupper);

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




}