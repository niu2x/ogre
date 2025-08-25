#include <hyue/StringUtils.h>

#include <stdarg.h>

#include <algorithm>
#include <cstring>

#include <boost/algorithm/string.hpp>

#include <hyue/panic.h>

#define strnicmp strncasecmp

namespace hyue {

String StringUtils::lower(const String& str) { return boost::to_lower_copy(str); }

String StringUtils::upper(const String& str) { return boost::to_upper_copy(str); }

void StringUtils::lower(String* str) { boost::to_lower(*str); }

void StringUtils::upper(String* str) { boost::to_upper(*str); }

void StringUtils::trim(String* str, bool left, bool right)
{
    static const String delims = " \t\r\n";
    if (right)
        str->erase(str->find_last_not_of(delims) + 1); // trim right
    if (left)
        str->erase(0, str->find_first_not_of(delims)); // trim left
}

String StringUtils::trim(const String& str, bool left, bool right)
{
    String copy = str;
    trim(&copy, left, right);
    return copy;
}

bool StringUtils::starts_with(const String& str, const String& pattern, bool ignore_case)
{
    if (pattern.empty())
        return false;

    if (ignore_case) {
        return strnicmp(str.c_str(), pattern.c_str(), pattern.size()) == 0;
    }

    return strncmp(str.c_str(), pattern.c_str(), pattern.size()) == 0;
}

bool StringUtils::ends_with(const String& str, const String& pattern, bool ignore_case)
{
    if (pattern.empty())
        return false;

    if (str.size() < pattern.size())
        return false;

    size_t offset = str.size() - pattern.size();

    if (ignore_case) {
        return strnicmp(str.c_str() + offset, pattern.c_str(), pattern.size()) == 0;
    }

    return strncmp(str.c_str() + offset, pattern.c_str(), pattern.size()) == 0;
}

StringVector StringUtils::split(const String& str, const String& delims, bool compress_token)
{
    StringVector split_vec;
    auto compress_mode = compress_token ? boost::token_compress_on : boost::token_compress_off;
    boost::split(split_vec, str, boost::is_any_of(delims), compress_mode);
    return split_vec;
}

void StringUtils::split_filename(const String& filename, String* dir, String* basename)
{
    FilePath path = filename;
    if (dir)
        *dir = path.parent_path();
    if (basename)
        *basename = path.filename();
}

// String StringUtils::standardise_dir_path(const String& init)
// {
//     String path = init;
//     std::replace( path.begin(), path.end(), '\\', '/' );
//     if( path[path.length() - 1] != '/' )
//         path += '/';
//     return path;
// }

String StringUtils::normalize_file_path(const String& init)
{
    const char* buffer_src = init.c_str();
    int path_len = (int)init.size();
    int index_src = 0;
    int index_dst = 0;
    int meta_path_area = 0;

    char reserved_buf[1024];
    char* buffer_dst = reserved_buf;
    bool is_dest_allocated = false;
    if (path_len > 1023) {
        // if source path is to long ensure we don't do a buffer overrun by allocating some
        // new memory
        is_dest_allocated = true;
        buffer_dst = new char[path_len + 1];
    }

    // The outer loop loops over directories
    while (index_src < path_len) {
        if (index_src && ((buffer_src[index_src] == '\\') || (buffer_src[index_src] == '/'))) {
            // check if we have a directory delimiter if so skip it (we should already
            // have written such a delimiter by this point
            ++index_src;
            continue;
        } else {
            // check if there is a directory to skip of type ".\"
            if ((buffer_src[index_src] == '.')
                && ((buffer_src[index_src + 1] == '\\') || (buffer_src[index_src + 1] == '/'))) {
                index_src += 2;
                continue;
            }

            // check if there is a directory to skip of type "..\"
            else if ((buffer_src[index_src] == '.') && (buffer_src[index_src + 1] == '.')
                     && ((buffer_src[index_src + 2] == '\\') || (buffer_src[index_src + 2] == '/'))) {
                if (index_dst > meta_path_area) {
                    // skip a directory backward in the destination path
                    do {
                        --index_dst;
                    } while ((index_dst > meta_path_area) && (buffer_dst[index_dst - 1] != '/'));
                    index_src += 3;
                    continue;
                } else {
                    // we are about to write "..\" to the destination buffer
                    // ensure we will not remove this in future "skip directories"
                    meta_path_area += 3;
                }
            }
        }

        // transfer the current directory name from the source to the destination
        while (index_src < path_len) {
            char cur_char = buffer_src[index_src];
            if ((cur_char == '\\') || (cur_char == '/'))
                cur_char = '/';
            buffer_dst[index_dst] = cur_char;
            ++index_dst;
            ++index_src;
            if (cur_char == '/')
                break;
        }
    }
    buffer_dst[index_dst] = 0;

    String normalized(buffer_dst);
    if (is_dest_allocated) {
        delete[] buffer_dst;
    }

    return normalized;
}

String StringUtils::format(const char* fmt, ...)
{
    // try to use a stack buffer and fall back to heap for large strings
    char sbuf[1024];
    size_t bsize = sizeof(sbuf);
    std::vector<char> hbuf;
    char* pbuf = sbuf;

    while (true) {
        va_list va;
        va_start(va, fmt);
        int len = vsnprintf(pbuf, bsize, fmt, va);
        va_end(va);

        HYUE_ASSERT(len >= 0, "Check format string for errors");
        if (size_t(len) >= bsize) {
            hbuf.resize(len + 1);
            pbuf = hbuf.data();
            bsize = hbuf.size();
            continue;
        }
        pbuf[bsize - 1] = 0;
        return String(pbuf, len);
    }
}

} // namespace hyue