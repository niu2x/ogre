#pragma once

#include <hyue/type.h>

namespace hyue {

struct HYUE_API StringUtils {

    static String lower(const String&);
    static void lower(String*);

    static String upper(const String&);
    static void upper(String*);

    static void trim(String*, bool left = true, bool right = true);
    static String trim(const String&, bool left = true, bool right = true);

    static bool starts_with(const String& str, const String& pattern, bool ignore_case = false);

    static bool ends_with(const String& str, const String& pattern, bool ignore_case = false);

    // If compress_token argument is true, adjacent separators are merged together. Otherwise, every two separators
    // delimit a token.
    static StringVector split(const String& str, const String& delims, bool compress_token = false);

    // static String standardise_dir_path(const String& init);

    static String normalize_file_path(const String& init);

    static String format(const char* fmt, ...);

    static void split_filename(const String& filename, String* dir, String* basename);
};

} // namespace hyue