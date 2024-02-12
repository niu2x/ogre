/*
 -----------------------------------------------------------------------------
 This source file is part of OGRE
 (Object-oriented Graphics Rendering Engine)
 For the latest info, see http://www.ogre3d.org/
 
 Copyright (c) 2000-2014 Torus Knot Software Ltd
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 -----------------------------------------------------------------------------
 */
#ifndef __OGRE_DEFLATE_H__
#define __OGRE_DEFLATE_H__

#if OGRE_NO_ZIP_ARCHIVE == 0

#include "OgrePrerequisites.h"
#include "data_stream.h"
#include "OgreHeaderPrefix.h"

/// forward decls
struct mz_stream_s;
using ZStream = struct mz_stream_s;

namespace Ogre
{
    /** Template version of cache based on static array.
     'cache_size' defines size of cache in bytes. */
    template <size_t cache_size>
    class StaticCache
    {
    public:
        /// Constructor
        StaticCache()
        {
            valid_bytes_ = 0;
            pos_ = 0;
            memset(buf_, 0, cache_size);
        }

        /** Cache data pointed by 'buf'. If 'count' is greater than cache size, we cache only last bytes.
         Returns number of bytes written to cache. */
        size_t cache_data(const void* buf, size_t count)
        {
            assert(avail() == 0 && "It is assumed that you cache data only after you have read everything.");

            if (count < cache_size)
            {
                // number of bytes written is less than total size of cache
                if (count + valid_bytes_ <= cache_size)
                {
                    // just append
                    memcpy(buf_ + valid_bytes_, buf, count);
                    valid_bytes_ += count;
                }
                else
                {
                    size_t begOff = count - (cache_size - valid_bytes_);
                    // override old cache content in the beginning
                    memmove(buf_, buf_ + begOff, valid_bytes_ - begOff);
                    // append new data
                    memcpy(buf_ + cache_size - count, buf, count);
                    valid_bytes_ = cache_size;
                }
                pos_ = valid_bytes_;
                return count;
            }
            else
            {
                // discard all
                memcpy(buf_, (const char*)buf + count - cache_size, cache_size);
                valid_bytes_ = pos_ = cache_size;
                return cache_size;
            }
        }
        /** Read data from cache to 'buf' (maximum 'count' bytes). Returns number of bytes read from cache. */
        size_t read(void* buf, size_t count)
        {
            size_t rb = avail();
            rb = (rb < count) ? rb : count;
            memcpy(buf, buf_ + pos_, rb);
            pos_ += rb;
            return rb;
        }

        /** Step back in cached stream by 'count' bytes. Returns 'true' if cache contains resulting position. */
        bool rewind(size_t count)
        {
            if (pos_ < count)
            {
                clear();
                return false;
            }
            else
            {
                pos_ -= count;
                return true;
            }
        }
        /** Step forward in cached stream by 'count' bytes. Returns 'true' if cache contains resulting position. */
        bool ff(size_t count)
        {
            if (avail() < count)
            {
                clear();
                return false;
            }
            else
            {
                pos_ += count;
                return true;
            }
        }

        /** Returns number of bytes available for reading in cache after rewinding. */
        size_t avail() const
        {
            return valid_bytes_ - pos_;
        }

        /** Clear the cache */
        void clear()
        {
            valid_bytes_ = 0;
            pos_ = 0;
        }
    private:
        /// Static buffer
        char buf_[cache_size];

        /// Number of bytes valid in cache (written from the beginning of static buffer)
        size_t valid_bytes_;
        /// Current read position
        size_t pos_;
    };

    /** Stream which compresses / uncompresses data using the 'deflate' compression
        algorithm.

        This stream is designed to wrap another stream for the actual source / destination
        of the compressed data, it has no concrete source / data itself. The idea is
        that you pass uncompressed data through this stream, and the underlying
        stream reads/writes compressed data to the final source.
    @note
        This is an alternative to using a compressed archive since it is able to 
        compress & decompress regardless of the actual source of the stream.
        You should avoid using this with already compressed archives.
        Also note that this cannot be used as a read / write stream, only a read-only
        or write-only stream.
    */
    class  DeflateStream : public DataStream
    {
    public:
        /** Requested stream type. All are essentially the same deflate stream with varying wrapping.
            ZLib is used by default.
        */
        enum class StreamType
        {
            Invalid = -1, /// Unexpected stream type or uncompressed data
            Deflate = 0,  /// no header, no checksum, rfc1951
            ZLib = 1,     /// 2 byte header, 4 byte footer with adler32 checksum, rfc1950
            GZip = 2,     /// 10 byte header, 8 byte footer with crc32 checksum and unpacked size, rfc1952
        };
    
    public:
        /** Constructor for creating unnamed stream wrapping another stream.
         @param compressedStream The stream that this stream will use when reading / 
            writing compressed data. The access mode from this stream will be matched.
         @param tmpFileName Path/Filename to be used for temporary storage of incoming data
         @param avail_in Available data length to be uncompressed. With it we can uncompress
            DataStream partly.
        */
        DeflateStream(const DataStreamPtr& compressedStream, const String& tmpFileName = "",
            size_t avail_in = 0);
        /** Constructor for creating named stream wrapping another stream.
         @param name The name to give this stream
         @param compressedStream The stream that this stream will use when reading / 
            writing compressed data. The access mode from this stream will be matched.
         @param tmpFileName Path/Filename to be used for temporary storage of incoming data
         @param avail_in Available data length to be uncompressed. With it we can uncompress
            DataStream partly.
         */
        DeflateStream(const String& name, const DataStreamPtr& compressedStream, const String& tmpFileName="",
            size_t avail_in = 0);
        /** Constructor for creating named stream wrapping another stream.
         @param name The name to give this stream
         @param compressedStream The stream that this stream will use when reading / 
            writing compressed data. The access mode from this stream will be matched.
         @param streamType The type of compressed stream
         @param tmpFileName Path/Filename to be used for temporary storage of incoming data
         @param avail_in Available data length to be uncompressed. With it we can uncompress
            DataStream partly.
         */
        DeflateStream(const String& name, const DataStreamPtr& compressedStream, StreamType streamType, const String& tmpFileName="",
            size_t avail_in = 0);
        
        ~DeflateStream();
        
        /** Returns whether the compressed stream is valid deflated data.

            If you pass this class a READ stream which is not compressed with the 
            deflate algorithm, this method returns false and all read commands
            will actually be executed as passthroughs as a fallback. 
        */
        bool compressed_stream_valid() const { return stream_type_ != StreamType:: Invalid; }
        
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
        void seek( size_t pos ) override;
        
        /** @copydoc DataStream::tell
         */
        size_t tell(void) const override;
        
        /** @copydoc DataStream::eof
         */
        bool eof(void) const override;
        
        /** @copydoc DataStream::close
         */
        void close(void) override;

        uint16_t access_mode() const override {
            return access_;
        }

    private:
        DataStreamPtr compressed_stream_;
        DataStreamPtr tmp_write_stream_;
        String temp_file_name_;
        ZStream* z_stream_;
        int status_;
        size_t current_pos_;
        size_t avail_in_;
        uint16_t access_;
        
        /// Cache for read data in case skipping around
        StaticCache<16 * XDOG_STREAM_TEMP_SIZE> read_cache_;
        
        /// Intermediate buffer for read / write
        uint8_t *tmp_;
        
        /// Whether the underlying stream is valid compressed data
        StreamType stream_type_;
        
        void init();
        void destroy();
        void compress_final();

        size_t avail_in_for_single_pass();
        
    };
}

#include "OgreHeaderSuffix.h"

#endif

#endif
