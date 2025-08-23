#include <hyue/DataStream.h>

#include <string.h>

#include <hyue/StringUtils.h>
#include <hyue/panic.h>

#define HYUE_STREAM_TEMP_SIZE 128

namespace hyue {

//-----------------------------------------------------------------------
// DataStream
//-----------------------------------------------------------------------
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

//-----------------------------------------------------------------------
// MemoryDataStream
//-----------------------------------------------------------------------
MemoryDataStream::MemoryDataStream(void* p_mem, size_t in_size, bool free_on_close, bool read_only)
: DataStream(static_cast<uint16_t>(read_only ? READ : (READ | WRITE)))

{
    data_ = pos_ = static_cast<uint8_t*>(p_mem);
    size_ = in_size;
    end_ = data_ + size_;
    free_on_close_ = free_on_close;
}

MemoryDataStream::MemoryDataStream(const String& name, void* p_mem, size_t in_size, bool free_on_close, bool read_only)
: DataStream(name, static_cast<uint16_t>(read_only ? READ : (READ | WRITE)))
{
    data_ = pos_ = static_cast<uint8_t*>(p_mem);
    size_ = in_size;
    end_ = data_ + size_;
    free_on_close_ = free_on_close;
}

MemoryDataStream::MemoryDataStream(DataStream* source_stream, bool free_on_close, bool read_only)
: DataStream(static_cast<uint16_t>(read_only ? READ : (READ | WRITE)))
{
    // Copy data from incoming stream
    size_ = source_stream->get_size();
    if (size_ == 0 && !source_stream->is_eof()) {
        // size of source is unknown, read all of it into memory
        String contents = source_stream->get_as_string();
        size_ = contents.size();
        data_ = new uint8_t[size_];
        pos_ = data_;
        memcpy(data_, contents.data(), size_);
        end_ = data_ + size_;
    } else {
        data_ = new uint8_t[size_];
        pos_ = data_;
        end_ = data_ + source_stream->read(data_, size_);
    }
    free_on_close_ = free_on_close;
}
//-----------------------------------------------------------------------
MemoryDataStream::MemoryDataStream(const String& name, DataStream* source_stream, bool free_on_close, bool read_only)
: DataStream(name, static_cast<uint16_t>(read_only ? READ : (READ | WRITE)))
{
    // Copy data from incoming stream
    size_ = source_stream->get_size();
    if (size_ == 0 && !source_stream->is_eof()) {
        // size of source is unknown, read all of it into memory
        String contents = source_stream->get_as_string();
        size_ = contents.size();
        data_ = new uint8_t[size_];
        pos_ = data_;
        memcpy(data_, contents.data(), size_);
        end_ = data_ + size_;
    } else {
        data_ = new uint8_t[size_];
        pos_ = data_;
        end_ = data_ + source_stream->read(data_, size_);
    }
    free_on_close_ = free_on_close;
}

//-----------------------------------------------------------------------
MemoryDataStream::MemoryDataStream(size_t in_size, bool free_on_close, bool read_only)
: DataStream(static_cast<uint16_t>(read_only ? READ : (READ | WRITE)))
{
    size_ = in_size;
    data_ = new uint8_t[size_];
    pos_ = data_;
    end_ = data_ + size_;
    free_on_close_ = free_on_close;
}
//-----------------------------------------------------------------------
MemoryDataStream::MemoryDataStream(const String& name, size_t in_size, bool free_on_close, bool read_only)
: DataStream(name, static_cast<uint16_t>(read_only ? READ : (READ | WRITE)))
{
    size_ = in_size;
    data_ = new uint8_t[size_];
    pos_ = data_;
    end_ = data_ + size_;
    free_on_close_ = free_on_close;
}

MemoryDataStream::~MemoryDataStream()
{
    close();
}

size_t MemoryDataStream::read(void* buf, size_t count)
{
    size_t cnt = count;
    // Read over end of memory?
    if (pos_ + cnt > end_)
        cnt = end_ - pos_;
    if (cnt == 0)
        return 0;

    memcpy(buf, pos_, cnt);
    pos_ += cnt;
    return cnt;
}

//---------------------------------------------------------------------
size_t MemoryDataStream::write(const void* buf, size_t count)
{
    size_t written = 0;
    if (is_writeable()) {
        written = count;
        // we only allow writing within the extents of allocated memory
        // check for buffer overrun & disallow
        if (pos_ + written > end_)
            written = end_ - pos_;
        if (written == 0)
            return 0;

        memcpy(pos_, buf, written);
        pos_ += written;
    }
    return written;
}

size_t MemoryDataStream::read_line(char* buf, size_t max_count, const String& delim)
{
    // Deal with both Unix & Windows LFs
    bool trim_CR = false;
    if (delim.find_first_of('\n') != String::npos) {
        trim_CR = true;
    }

    size_t p = 0;

    // Make sure pos can never go past the end of the data
    while (p < max_count && pos_ < end_) {
        if (delim.find(*pos_) != String::npos) {
            // Trim off trailing CR if this was a CR/LF entry
            if (trim_CR && p && buf[p - 1] == '\r') {
                // terminate 1 character early
                --p;
            }

            // Found terminator, skip and break out
            ++pos_;
            break;
        }

        buf[p++] = *pos_++;
    }

    // terminate
    buf[p] = '\0';
    return p;
}

size_t MemoryDataStream::skip_line(const String& delim)
{
    size_t p = 0;

    // Make sure pos can never go past the end of the data
    while (pos_ < end_) {
        ++p;
        if (delim.find(*pos_++) != String::npos) {
            // Found terminator, break out
            break;
        }
    }

    return p;
}

//-----------------------------------------------------------------------
void MemoryDataStream::skip(long count)
{
    size_t new_pos = (size_t)((pos_ - data_) + count);
    HYUE_ASSERT(data_ + new_pos <= end_, "");
    pos_ = data_ + new_pos;
}
//-----------------------------------------------------------------------
void MemoryDataStream::seek(size_t pos)
{
    HYUE_ASSERT(data_ + pos <= end_, "");
    pos_ = data_ + pos;
}
//-----------------------------------------------------------------------
size_t MemoryDataStream::tell(void) const
{
    // data_ is start, pos_ is current location
    return pos_ - data_;
}

bool MemoryDataStream::is_eof(void) const
{
    return pos_ >= end_;
}

void MemoryDataStream::close(void)
{
    access_ = 0;
    if (free_on_close_ && data_) {
        delete[] data_;
        data_ = 0;
    }
}

} // namespace hyue
