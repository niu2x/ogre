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
#include "OgreStableHeaders.h"

#if OGRE_NO_ZIP_ARCHIVE == 0

#include "deflate.h"
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS || OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#include "macUtils.h"
#endif

#define MINIZ_HEADER_FILE_ONLY
#include <miniz.h>

namespace Ogre
{
    static int window_bits(DeflateStream::StreamType t) {
        using StreamType = DeflateStream:: StreamType;
        return (t == StreamType::Deflate) ? -MAX_WBITS : (t == StreamType::GZip) ? 16 + MAX_WBITS : MAX_WBITS;
    }

    // memory implementations
    static void* OgreZalloc(void* opaque, size_t items, size_t size)
    {
        return OGRE_MALLOC(items * size, MEMCATEGORY_GENERAL);
    }
    static void OgreZfree(void* opaque, void* address)
    {
        OGRE_FREE(address, MEMCATEGORY_GENERAL);
    }
    #define OGRE_DEFLATE_TMP_SIZE 16384
    //---------------------------------------------------------------------
    DeflateStream::DeflateStream(const DataStreamPtr& compressedStream, const String& tmpFileName, size_t avail_in)
    : DataStream()
    ,access_(compressedStream->access_mode())
    , compressed_stream_(compressedStream)
    , temp_file_name_(tmpFileName)
    , z_stream_(0)
    , current_pos_(0)
    , avail_in_(avail_in)
    , tmp_(0)
    , stream_type_(StreamType::ZLib)
    {
        init();
    }
    //---------------------------------------------------------------------
    DeflateStream::DeflateStream(const String& name, const DataStreamPtr& compressedStream, const String& tmpFileName, size_t avail_in)
    : DataStream(name)
    ,access_(compressedStream->access_mode())
    , compressed_stream_(compressedStream)
    , temp_file_name_(tmpFileName)
    , z_stream_(0)
    , current_pos_(0)
    , avail_in_(avail_in)
    , tmp_(0)
    , stream_type_(StreamType::ZLib)
    {
        init();
    }
    //---------------------------------------------------------------------
    DeflateStream::DeflateStream(const String& name, const DataStreamPtr& compressedStream, StreamType streamType, const String& tmpFileName, size_t avail_in)
    : DataStream(name)
    ,access_(compressedStream->access_mode())
    , compressed_stream_(compressedStream)
    , temp_file_name_(tmpFileName)
    , z_stream_(0)
    , current_pos_(0)
    , avail_in_(avail_in)
    , tmp_(0)
    , stream_type_(streamType)
    {
        init();
    }
    //---------------------------------------------------------------------
    size_t DeflateStream::avail_in_for_single_pass()
    {
        size_t ret = OGRE_DEFLATE_TMP_SIZE;

        // if we are doing particial-uncompressing
        if(avail_in_>0)
        {
            if(avail_in_<ret)
                ret = avail_in_;
            avail_in_ -= ret;
        }

        return ret;
    }
    //---------------------------------------------------------------------
    void DeflateStream::init()
    {
        z_stream_ = OGRE_ALLOC_T(ZStream, 1, MEMCATEGORY_GENERAL);
        z_stream_->zalloc = OgreZalloc;
        z_stream_->zfree = OgreZfree;
        
        if (access_mode() == READ)
        {
            tmp_ = (unsigned char*)OGRE_MALLOC(OGRE_DEFLATE_TMP_SIZE, MEMCATEGORY_GENERAL);
            size_t restorePoint = compressed_stream_->tell();
            // read early chunk
            z_stream_->next_in = tmp_;
            z_stream_->avail_in = static_cast<uint>(compressed_stream_->read(tmp_, avail_in_for_single_pass()));
            
            int windowBits = window_bits(stream_type_);
            if (inflateInit2(z_stream_, windowBits) != Z_OK)
            {
                stream_type_ = StreamType::Invalid;
            }
            
            if (stream_type_ != StreamType::Invalid)
            {
                // in fact, inflateInit on some implementations doesn't try to read
                // anything. We need to at least read something to test
                Bytef testOut[4];
                size_t savedIn = z_stream_->avail_in;
                z_stream_->avail_out = 4;
                z_stream_->next_out = testOut;
                if (inflate(z_stream_, Z_SYNC_FLUSH) != Z_OK)
                    stream_type_ = StreamType:: Invalid;
                // restore for reading
                z_stream_->avail_in = static_cast<uint>(savedIn);
                z_stream_->next_in = tmp_;

                inflateReset(z_stream_);
            }

            if (stream_type_ == StreamType::Invalid)
            {
                // Not compressed data!
                // Fail gracefully, fall back on reading the underlying stream direct
                destroy();
                compressed_stream_->seek(restorePoint);
            }               
        }
        else 
        {
            if(temp_file_name_.empty())
            {
                // Write to temp file
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WINRT
                char* tmpname = _tempnam(".", "ogre");
                if (!tmpname)
                {
                    // Having no file name here will cause various problems later.
                    OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Temporary file name generation failed.", "DeflateStream::init");
                }
                else
                {
                    temp_file_name_ = tmpname;
                    free(tmpname);
                }
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS || OGRE_PLATFORM == OGRE_PLATFORM_APPLE
                temp_file_name_ = macTempFileName();
#else
                char tmpname[] = "/tmp/ogreXXXXXX";
                if (mkstemp(tmpname) == -1)
                    OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Temporary file name generation failed.", "DeflateStream::init");

                temp_file_name_ = tmpname;
#endif
            }

            tmp_write_stream_ = _openFileStream(temp_file_name_, std::ios::binary | std::ios::out);
            
        }

    }
    //---------------------------------------------------------------------
    void DeflateStream::destroy()
    {
        if (access_mode() == READ)
            inflateEnd(z_stream_);

        OGRE_FREE(z_stream_, MEMCATEGORY_GENERAL);
        z_stream_ = 0;
        OGRE_FREE(tmp_, MEMCATEGORY_GENERAL);
        tmp_ = 0;
    }
    //---------------------------------------------------------------------
    DeflateStream::~DeflateStream()
    {
        close();
    }
    //---------------------------------------------------------------------
    size_t DeflateStream::read(void* buf, size_t count)
    {
        if (stream_type_ == StreamType::Invalid)
        {
            return compressed_stream_->read(buf, count);
        }
        
        if (access_mode() & WRITE)
        {
            return tmp_write_stream_->read(buf, count);
        }
        else 
        {

            size_t restorePoint = compressed_stream_->tell();
            // read from cache first
            size_t cachereads = read_cache_.read(buf, count);
            
            size_t newReadUncompressed = 0;

            if (cachereads < count)
            {
                z_stream_->avail_out = static_cast<uint>(count - cachereads);
                z_stream_->next_out = (Bytef*)buf + cachereads;
                
                while (z_stream_->avail_out)
                {
                    // Pull next chunk of compressed data from the underlying stream
                    if (!z_stream_->avail_in && !compressed_stream_->eof())
                    {
                        z_stream_->avail_in = static_cast<uint>(compressed_stream_->read(tmp_, avail_in_for_single_pass()));
                        z_stream_->next_in = tmp_;
                    }
                    
                    if (z_stream_->avail_in || z_stream_->avail_out)
                    {
                        int availpre = z_stream_->avail_out;
                        status_ = inflate(z_stream_, Z_SYNC_FLUSH);
                        size_t readUncompressed = availpre - z_stream_->avail_out;
                        newReadUncompressed += readUncompressed;
                        if (status_ != Z_OK)
                        {
                            // End of data, or error
                            if (status_ != Z_STREAM_END)
                            {
                                compressed_stream_->seek(restorePoint);
                                OGRE_EXCEPT(Exception::ERR_INVALID_STATE, "Error in compressed stream");
                            }
                            else 
                            {
                                // back up the stream so that it can be used from the end onwards                                                   
                                long unusedCompressed = z_stream_->avail_in;
                                compressed_stream_->skip(-unusedCompressed);
                            }

                            break;
                        }
                    }
                }
            
                // Cache the last bytes read not from cache
                read_cache_.cache_data((char*)buf + cachereads, newReadUncompressed);
            }
            
            current_pos_ += newReadUncompressed + cachereads;
            
            return newReadUncompressed + cachereads;
        }
    }
    //---------------------------------------------------------------------
    size_t DeflateStream::write(const void* buf, size_t count)
    {
        if ((access_mode() & WRITE) == 0)
            OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
                        "Not a writable stream", "DeflateStream::write");
        
        return tmp_write_stream_->write(buf, count);
    }
    //---------------------------------------------------------------------
    void DeflateStream::compress_final()
    {
        // Prevent reenterancy
        if( !tmp_write_stream_ )
            return;
        
        // Close temp stream
        tmp_write_stream_->close();
        tmp_write_stream_.reset();
        
        // Copy & compress
        // We do this rather than compress directly because some code seeks
        // around while writing (e.g. to update size blocks) which is not
        // possible when compressing on the fly
        
        int ret, flush;
        char in[OGRE_DEFLATE_TMP_SIZE];
        char out[OGRE_DEFLATE_TMP_SIZE];
        
        int windowBits = window_bits(stream_type_);

        if (deflateInit2(z_stream_, Z_DEFAULT_COMPRESSION, Z_DEFLATED, windowBits, 8, Z_DEFAULT_STRATEGY) != Z_OK)
        {
            destroy();
            OGRE_EXCEPT(Exception::ERR_INVALID_STATE, 
                        "Error initialising deflate compressed stream!",
                        "DeflateStream::init");
        }
        
        std::ifstream inFile;
        inFile.open(temp_file_name_.c_str(), std::ios::in | std::ios::binary);
        
        do 
        {
            inFile.read(in, OGRE_DEFLATE_TMP_SIZE);
            z_stream_->avail_in = (uInt)inFile.gcount();
            if (inFile.bad()) 
            {
                deflateEnd(z_stream_);
                OGRE_EXCEPT(Exception::ERR_INVALID_STATE, 
                            "Error reading temp uncompressed stream!",
                            "DeflateStream::init");
            }
            flush = inFile.eof() ? Z_FINISH : Z_NO_FLUSH;
            z_stream_->next_in = (Bytef*)in;
            
            /* run deflate() on input until output buffer not full, finish
             compression if all of source has been read in */
            do 
            {
                z_stream_->avail_out = OGRE_DEFLATE_TMP_SIZE;
                z_stream_->next_out = (Bytef*)out;
                ret = deflate(z_stream_, flush);    /* no bad return value */
                assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
                size_t compressed = OGRE_DEFLATE_TMP_SIZE - z_stream_->avail_out;
                compressed_stream_->write(out, compressed);
            } while (z_stream_->avail_out == 0);
            assert(z_stream_->avail_in == 0);     /* all input will be used */
            
            /* done when last data in file processed */
        } while (flush != Z_FINISH);

        assert(ret == Z_STREAM_END);        /* stream will be complete */
        (void)ret;
        deflateEnd(z_stream_);

        inFile.close();
        remove(temp_file_name_.c_str());
                        
    }
    //---------------------------------------------------------------------
    void DeflateStream::skip(long count)
    {
        if (stream_type_ == StreamType::Invalid)
        {
            compressed_stream_->skip(count);
            return;
        }
        
        if (access_mode() & WRITE)
        {
            tmp_write_stream_->skip(count);
        }
        else 
        {
            if (count > 0)
            {
                if (!read_cache_.ff(count))
                {
                    OGRE_EXCEPT(Exception::ERR_INVALID_STATE, 
                                "You can only skip within the cache range in a deflate stream.",
                                "DeflateStream::skip");
                }
            }
            else if (count < 0)
            {
                if (!read_cache_.rewind((size_t)(-count)))
                {
                    OGRE_EXCEPT(Exception::ERR_INVALID_STATE, 
                                "You can only skip within the cache range in a deflate stream.",
                                "DeflateStream::skip");
                }
            }
        }       
        current_pos_ = static_cast<size_t>(static_cast<long>(current_pos_) + count);
        
        
    }
    //---------------------------------------------------------------------
    void DeflateStream::seek( size_t pos )
    {
        if (stream_type_ == StreamType:: Invalid)
        {
            compressed_stream_->seek(pos);
            return;
        }
        if (access_mode() & WRITE)
        {
            tmp_write_stream_->seek(pos);
        }
        else
        {
            if (pos == 0)
            {
                current_pos_ = 0;
                z_stream_->next_in = tmp_;
                compressed_stream_->seek(0);
                z_stream_->avail_in = static_cast<uint>(compressed_stream_->read(tmp_, avail_in_for_single_pass()));
                inflateReset(z_stream_);
                read_cache_.clear();
            }
            else 
            {
                skip(pos - tell());
            }
        }       
    }
    //---------------------------------------------------------------------
    size_t DeflateStream::tell(void) const
    {
        if (stream_type_ == StreamType:: Invalid)
        {
            return compressed_stream_->tell();
        }
        else if(access_mode() & WRITE) 
        {
            return tmp_write_stream_->tell();
        }
        else
        {
            return current_pos_;
        }

    }
    //---------------------------------------------------------------------
    bool DeflateStream::eof(void) const
    {
        if (access_mode() & WRITE)
            return tmp_write_stream_->eof();
        else 
        {
            if (stream_type_ == StreamType::Invalid)
                return compressed_stream_->eof();
            else
                return compressed_stream_->eof() && status_ == Z_STREAM_END;
        }
    }
    //---------------------------------------------------------------------
    void DeflateStream::close(void)
    {
        if (access_mode() & WRITE)
            compress_final();

        destroy();

        access_  = 0;

        // don't close underlying compressed stream in case used for something else
    }
    //---------------------------------------------------------------------
    
    
}

#endif
