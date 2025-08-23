#include <hyue/DataStream.h>

#include <string.h>

#include <hyue/StringUtils.h>

#define HYUE_STREAM_TEMP_SIZE 128

namespace hyue {

DataStream::~DataStream()
{
}

DataStream::DataStream(uint16_t access_mode)
: size_(0),
  access_(access_mode)
{
}
/// Constructor for creating named streams
DataStream::DataStream(const String& name, uint16_t access_mode)
: name_(name),
  size_(0),
  access_(access_mode)
{
}

size_t DataStream::write(const void* buf, size_t count)
{
    (void)buf;
    (void)count;
    // default to not supported
    return 0;
}

String DataStream::get_line(bool trim_after)
{
    char tmp_buf[HYUE_STREAM_TEMP_SIZE];

    String ret_string;
    size_t read_count;
    // Keep looping while not hitting delimiter
    while ((read_count = read(tmp_buf, HYUE_STREAM_TEMP_SIZE - 1)) != 0) {
        // Terminate string
        tmp_buf[read_count] = '\0';

        char* p = strchr(tmp_buf, '\n');
        if (p != 0) {
            // Reposition backwards
            skip((long)(p + 1 - tmp_buf - read_count));
            *p = '\0';
        }

        ret_string += tmp_buf;

        if (p != 0) {
            // Trim off trailing CR if this was a CR/LF entry
            if (ret_string.length() && ret_string[ret_string.length() - 1] == '\r') {
                ret_string.erase(ret_string.length() - 1, 1);
            }

            // Found terminator, break out
            break;
        }
    }

    if (trim_after) {
        StringUtils::trim(&ret_string);
    }

    return ret_string;
}

size_t DataStream::read_line(char* buf, size_t max_count, const String& delim)
{
    // Deal with both Unix & Windows LFs
    bool trim_CR = false;
    if (delim.find_first_of('\n') != String::npos) {
        trim_CR = true;
    }

    char tmp_buf[HYUE_STREAM_TEMP_SIZE];
    size_t chunk_size = std::min(max_count, (size_t)HYUE_STREAM_TEMP_SIZE - 1);
    size_t total_count = 0;
    size_t read_count;
    while (chunk_size && (read_count = read(tmp_buf, chunk_size)) != 0) {
        // Terminate
        tmp_buf[read_count] = '\0';

        // Find first delimiter
        size_t pos = strcspn(tmp_buf, delim.c_str());

        if (pos < read_count) {
            // Found terminator, reposition backwards
            skip((long)(pos + 1 - read_count));
        }

        // Are we genuinely copying?
        if (buf) {
            memcpy(buf + total_count, tmp_buf, pos);
        }
        total_count += pos;

        if (pos < read_count) {
            // Trim off trailing CR if this was a CR/LF entry
            if (trim_CR && total_count && buf && buf[total_count - 1] == '\r') {
                --total_count;
            }

            // Found terminator, break out
            break;
        }

        // Adjust chunk_size for next time
        chunk_size = std::min(max_count - total_count, (size_t)HYUE_STREAM_TEMP_SIZE - 1);
    }

    // Terminate
    if (buf)
        buf[total_count] = '\0';

    return total_count;
}

//-----------------------------------------------------------------------
size_t DataStream::skip_line(const String& delim)
{
    char tmp_buf[HYUE_STREAM_TEMP_SIZE];
    size_t total = 0;
    size_t read_count;
    // Keep looping while not hitting delimiter
    while ((read_count = read(tmp_buf, HYUE_STREAM_TEMP_SIZE - 1)) != 0) {
        // Terminate string
        tmp_buf[read_count] = '\0';

        // Find first delimiter
        size_t pos = strcspn(tmp_buf, delim.c_str());

        if (pos < read_count) {
            // Found terminator, reposition backwards
            skip((long)(pos + 1 - read_count));

            total += pos + 1;

            // break out
            break;
        }

        total += read_count;
    }

    return total;
}

String DataStream::get_as_string(void)
{
    // Read the entire buffer - ideally in one read, but if the size of
    // the buffer is unknown, do multiple fixed size reads.
    size_t buf_size = (size_ > 0 ? size_ : 4096);
    char* p_buf = new char[buf_size];
    // Ensure read from begin of stream
    seek(0);
    String result;
    while (!is_eof()) {
        size_t nr = read(p_buf, buf_size);
        result.append(p_buf, nr);
    }
    delete[] p_buf;
    return result;
}

} // namespace hyue
