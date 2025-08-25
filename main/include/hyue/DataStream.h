#pragma once

#include <hyue/type.h>

namespace hyue {

class HYUE_API DataStream {
public:
    enum AccessMode { READ = 1, WRITE = 2 };

    /// Constructor for creating unnamed streams
    DataStream(uint16_t access_mode = READ);
    /// Constructor for creating named streams
    DataStream(const String& name, uint16_t access_mode = READ);

    virtual ~DataStream() = 0;

    /// Returns the name of the stream, if it has one.
    const String& get_name(void) const { return name_; }
    /// Gets the access mode of the stream
    uint16_t get_access_mode() const { return access_; }
    /** Reports whether this stream is readable. */
    virtual bool is_readable() const { return (access_ & READ) != 0; }
    /** Reports whether this stream is writeable. */
    virtual bool is_writeable() const { return (access_ & WRITE) != 0; }

    // Streaming operators
    template <typename T>
    DataStream& operator>>(T& val)
    {
        read(reinterpret_cast<void*>(&val), sizeof(T));
        return *this;
    }

    /** Read the requisite number of bytes from the stream,
        stopping at the end of the file.
    @param buf Reference to a buffer pointer
    @param count Number of bytes to read
    @return The number of bytes read
    */
    virtual size_t read(void* buf, size_t count) = 0;
    /** Write the requisite number of bytes from the stream (only applicable to
        streams that are not read-only)
    @param buf Pointer to a buffer containing the bytes to write
    @param count Number of bytes to write
    @return The number of bytes written
    */
    virtual size_t write(const void* buf, size_t count);

    /** Get a single line from the stream.

        The delimiter character is not included in the data
        returned, and it is skipped over so the next read will occur
        after it. The buffer contents will include a
        terminating character.
    @note
        If you used this function, you <b>must</b> open the stream in <b>binary mode</b>,
        otherwise, it'll produce unexpected results.
    @param buf Reference to a buffer pointer
    @param max_count The maximum length of data to be read, excluding the terminating character
    @param delim The delimiter to stop at
    @return The number of bytes read, excluding the terminating character
    */
    virtual size_t read_line(char* buf, size_t max_count, const String& delim = "\n");

    /** Returns a String containing the next line of data, optionally
        trimmed for whitespace.

        This is a convenience method for text streams only, allowing you to
        retrieve a String object containing the next line of data. The data
        is read up to the next newline character and the result trimmed if
        required.
    @note
        If you used this function, you <b>must</b> open the stream in <b>binary mode</b>,
        otherwise, it'll produce unexpected results.
    @param
        trimAfter If true, the line is trimmed for whitespace (as in
        String.trim(true,true))
    */
    virtual String get_line(bool trim_after = true);

    /** Returns a String containing the entire stream.

        This is a convenience method for text streams only, allowing you to
        retrieve a String object containing all the data in the stream.
    */
    virtual String get_as_string(void);

    /** Skip a single line from the stream.
    @note
        If you used this function, you <b>must</b> open the stream in <b>binary mode</b>,
        otherwise, it'll produce unexpected results.
    @par
        delim The delimiter(s) to stop at
    @return The number of bytes skipped
    */
    virtual size_t skip_line(const String& delim = "\n");

    /** Skip a defined number of bytes. This can also be a negative value, in which case
    the file pointer rewinds a defined number of bytes. */
    virtual void skip(long count) = 0;

    /** Repositions the read point to a specified byte.
     */
    virtual void seek(size_t pos) = 0;

    /** Returns the current byte offset from beginning */
    virtual size_t tell(void) const = 0;

    /** Returns true if the stream has reached the end.
     */
    virtual bool is_eof(void) const = 0;

    /** Returns the total size of the data to be read from the stream,
        or 0 if this is indeterminate for this stream.
    */
    size_t get_size(void) const
    {
        return size_;
    }

    /** Close the stream; this makes further operations invalid. */
    virtual void close(void) = 0;

protected:
    /// The name (e.g. resource name) that can be used to identify the source for this data (optional)
    String name_;
    /// Size of the data in the stream (may be 0 if size cannot be determined)
    size_t size_;
    /// What type of access is allowed (AccessMode)
    uint16_t access_;
};

using DataStreamPtr = SharedPtr<DataStream>;

/// List of DataStream items
using DataStreamList = std::list<DataStreamPtr>;

/** Common subclass of DataStream for handling data from chunks of memory.
 */
class HYUE_API MemoryDataStream : public DataStream {
public:
    MemoryDataStream(void* p_mem, size_t size, bool free_on_close = false, bool read_only = false);

    MemoryDataStream(const String& name, void* p_mem, size_t size, bool free_on_close = false, bool read_only = false);

    /** Create a stream which pre-buffers the contents of another stream.

        This constructor can be used to intentionally read in the entire
        contents of another stream, copying them to the internal buffer
        and thus making them available in memory as a single unit.
    @param source_stream Another DataStream which will provide the source
        of data
    @param free_on_close If true, the memory associated will be destroyed
        when the stream is destroyed.
    @param read_only Whether to make the stream on this memory read-only once created
    */
    MemoryDataStream(DataStream* source_stream, bool free_on_close = true, bool read_only = false);
    /** Create a named stream which pre-buffers the contents of
        another stream.

        This constructor can be used to intentionally read in the entire
        contents of another stream, copying them to the internal buffer
        and thus making them available in memory as a single unit.
    @param name The name to give the stream
    @param source_stream Another DataStream which will provide the source
        of data
    @param free_on_close If true, the memory associated will be destroyed
        when the stream is destroyed.
    @param read_only Whether to make the stream on this memory read-only once created
    */
    MemoryDataStream(const String& name, DataStream* source_stream, bool free_on_close = true, bool read_only = false);
    /** Create a stream with a brand new empty memory chunk.
    @param size The size of the memory chunk to create in bytes
    @param free_on_close If true, the memory associated will be destroyed
        when the stream is destroyed.
    @param read_only Whether to make the stream on this memory read-only once created
    */
    MemoryDataStream(size_t size, bool free_on_close = true, bool read_only = false);
    /** Create a named stream with a brand new empty memory chunk.
    @param name The name to give the stream
    @param size The size of the memory chunk to create in bytes
    @param free_on_close If true, the memory associated will be destroyed
        when the stream is destroyed.
    @param read_only Whether to make the stream on this memory read-only once created
    */
    MemoryDataStream(const String& name, size_t size, bool free_on_close = true, bool read_only = false);

    ~MemoryDataStream();

    /** Get a pointer to the start of the memory block this stream holds. */
    uint8_t* get_ptr(void) { return data_; }

    /** Get a pointer to the current position in the memory block this stream holds. */
    uint8_t* get_current_ptr(void) { return pos_; }

    /** @copydoc DataStream::read
     */
    size_t read(void* buf, size_t count) override;

    /** @copydoc DataStream::write
     */
    size_t write(const void* buf, size_t count) override;

    /** @copydoc DataStream::read_line
     */
    size_t read_line(char* buf, size_t maxCount, const String& delim = "\n") override;

    /** @copydoc DataStream::skip_line
     */
    size_t skip_line(const String& delim = "\n") override;

    /** @copydoc DataStream::skip
     */
    void skip(long count) override;

    /** @copydoc DataStream::seek
     */
    void seek(size_t pos) override;

    /** @copydoc DataStream::tell
     */
    size_t tell(void) const override;

    /** @copydoc DataStream::eof
     */
    bool is_eof(void) const override;

    /** @copydoc DataStream::close
     */
    void close(void) override;

    /** Sets whether or not to free the encapsulated memory on close. */
    void set_free_on_close(bool free) { free_on_close_ = free; }

private:
    /// Pointer to the start of the data area
    uint8_t* data_;
    /// Pointer to the current position in the memory
    uint8_t* pos_;
    /// Pointer to the end of the memory
    uint8_t* end_;
    /// Do we delete the memory on close
    bool free_on_close_;
};

using MemoryDataStreamPtr = SharedPtr<MemoryDataStream>;

/** Common subclass of DataStream for handling data from
    std::basic_istream.
*/
class HYUE_API FileStreamDataStream : public DataStream {
public:
    /** Construct a read-only stream from an STL stream
    @param s Pointer to source stream
    @param free_on_close Whether to delete the underlying stream on
        destruction of this class
    */
    FileStreamDataStream(std::ifstream* s, bool free_on_close = true);
    /** Construct a read-write stream from an STL stream
    @param s Pointer to source stream
    @param free_on_close Whether to delete the underlying stream on
    destruction of this class
    */
    FileStreamDataStream(std::fstream* s, bool free_on_close = true);

    /** Construct named read-only stream from an STL stream
    @param name The name to give this stream
    @param s Pointer to source stream
    @param free_on_close Whether to delete the underlying stream on
        destruction of this class
    */
    FileStreamDataStream(const String& name, std::ifstream* s, bool free_on_close = true);

    /** Construct named read-write stream from an STL stream
    @param name The name to give this stream
    @param s Pointer to source stream
    @param free_on_close Whether to delete the underlying stream on
    destruction of this class
    */
    FileStreamDataStream(const String& name, std::fstream* s, bool free_on_close = true);


    FileStreamDataStream(const String& name, std::ifstream* s, size_t size, bool free_on_close = true);

 
    FileStreamDataStream(const String& name, std::fstream* s, size_t size, bool free_on_close = true);

    ~FileStreamDataStream();

    /** @copydoc DataStream::read
     */
    size_t read(void* buf, size_t count) override;

    /** @copydoc DataStream::write
     */
    size_t write(const void* buf, size_t count) override;

    /** @copydoc DataStream::read_line
     */
    size_t read_line(char* buf, size_t maxCount, const String& delim = "\n") override;

    /** @copydoc DataStream::skip
     */
    void skip(long count) override;

    /** @copydoc DataStream::seek
     */
    void seek(size_t pos) override;

    /** @copydoc DataStream::tell
     */
    size_t tell(void) const override;

    /** @copydoc DataStream::eof
     */
    bool is_eof(void) const override;

    /** @copydoc DataStream::close
     */
    void close(void) override;

private:
    /// Reference to source stream (read)
    std::istream* in_stream_;
    /// Reference to source file stream (read-only)
    std::ifstream* read_only_stream_;
    /// Reference to source file stream (read-write)
    std::fstream* stream_;

    bool free_on_close_;

    void determine_access();
};

/** Common subclass of DataStream for handling data from C-style file
    handles.

    Use of this class is generally discouraged; if you want to wrap file
    access in a DataStream, you should definitely be using the C++ friendly
    FileStreamDataStream. However, since there are quite a few applications
    and libraries still wedded to the old FILE handle access, this stream
    wrapper provides some backwards compatibility.
*/
class HYUE_API FileHandleDataStream : public DataStream {

public:
    /// Create stream from a C file handle
    FileHandleDataStream(FILE* handle, uint16_t accessMode = READ);
    /// Create named stream from a C file handle
    FileHandleDataStream(const String& name, FILE* handle, uint16_t accessMode = READ);
    ~FileHandleDataStream();

    /** @copydoc DataStream::read
     */
    size_t read(void* buf, size_t count) override;

    /** @copydoc DataStream::write
     */
    size_t write(const void* buf, size_t count) override;

    /** @copydoc DataStream::skip
     */
    void skip(long count) override;

    /** @copydoc DataStream::seek
     */
    void seek(size_t pos) override;

    /** @copydoc DataStream::tell
     */
    size_t tell(void) const override;

    /** @copydoc DataStream::eof
     */
    bool is_eof(void) const override;

    /** @copydoc DataStream::close
     */
    void close(void) override;

private:
    FILE* file_handle_;
};
/** @} */
/** @} */
} // namespace hyue
