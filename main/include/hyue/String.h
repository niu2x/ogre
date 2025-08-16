#pragma once

#include <string>

namespace hyue {

using String = std::string;

struct StringUtils {

    static String to_lower(const String &);
    static String to_upper(const String &);
    static void lower(String *);
    static void upper(String *);

    static bool starts_with(const String &str, const String &pattern, bool ignore_case=false);
    static bool ends_with(const String &str, const String &pattern, bool ignore_case=false);

};

}