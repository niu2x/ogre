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

namespace Ogre {

    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    template <typename T> DataStream& DataStream::operator >>(const T& val)
    {
        read(static_cast<void*>(&val), sizeof(T));
        return *this;
    }
    //-----------------------------------------------------------------------
    String DataStream::get_line(bool trimAfter)
    {
        char tmpBuf[XDOG_STREAM_TEMP_SIZE];
        String retString;
        size_t readCount;
        // Keep looping while not hitting delimiter
        while ((readCount = read(tmpBuf, XDOG_STREAM_TEMP_SIZE-1)) != 0)
        {
            // Terminate string
            tmpBuf[readCount] = '\0';

            char* p = strchr(tmpBuf, '\n');
            if (p != 0)
            {
                // Reposition backwards
                skip((long)(p + 1 - tmpBuf - readCount));
                *p = '\0';
            }

            retString += tmpBuf;

            if (p != 0)
            {
                // Trim off trailing CR if this was a CR/LF entry
                if (retString.length() && retString[retString.length()-1] == '\r')
                {
                    retString.erase(retString.length()-1, 1);
                }

                // Found terminator, break out
                break;
            }
        }

        if (trimAfter)
        {
            StringUtil::trim(retString);
        }

        return retString;
    }
    //-----------------------------------------------------------------------
    size_t DataStream::read_line(char* buf, size_t maxCount, const String& delim)
    {
        // Deal with both Unix & Windows LFs
        bool trimCR = false;
        if (delim.find_first_of('\n') != String::npos)
        {
            trimCR = true;
        }

        char tmpBuf[XDOG_STREAM_TEMP_SIZE];
        size_t chunkSize = std::min(maxCount, (size_t)XDOG_STREAM_TEMP_SIZE-1);
        size_t totalCount = 0;
        size_t readCount; 
        while (chunkSize && (readCount = read(tmpBuf, chunkSize)) != 0)
        {
            // Terminate
            tmpBuf[readCount] = '\0';

            // Find first delimiter
            size_t pos = strcspn(tmpBuf, delim.c_str());

            if (pos < readCount)
            {
                // Found terminator, reposition backwards
                skip((long)(pos + 1 - readCount));
            }

            // Are we genuinely copying?
            if (buf)
            {
                memcpy(buf+totalCount, tmpBuf, pos);
            }
            totalCount += pos;

            if (pos < readCount)
            {
                // Trim off trailing CR if this was a CR/LF entry
                if (trimCR && totalCount && buf && buf[totalCount-1] == '\r')
                {
                    --totalCount;
                }

                // Found terminator, break out
                break;
            }

            // Adjust chunkSize for next time
            chunkSize = std::min(maxCount-totalCount, (size_t)XDOG_STREAM_TEMP_SIZE-1);
        }

        // Terminate
        if(buf)
            buf[totalCount] = '\0';

        return totalCount;
    }
    //-----------------------------------------------------------------------
    size_t DataStream::skip_line(const String& delim)
    {
        char tmpBuf[XDOG_STREAM_TEMP_SIZE];
        size_t total = 0;
        size_t readCount;
        // Keep looping while not hitting delimiter
        while ((readCount = read(tmpBuf, XDOG_STREAM_TEMP_SIZE-1)) != 0)
        {
            // Terminate string
            tmpBuf[readCount] = '\0';

            // Find first delimiter
            size_t pos = strcspn(tmpBuf, delim.c_str());

            if (pos < readCount)
            {
                // Found terminator, reposition backwards
                skip((long)(pos + 1 - readCount));

                total += pos + 1;

                // break out
                break;
            }

            total += readCount;
        }

        return total;
    }
    //-----------------------------------------------------------------------
    String DataStream::as_string(void)
    {
        // Read the entire buffer - ideally in one read, but if the size of
        // the buffer is unknown, do multiple fixed size reads.
        
        auto data_len = size();

        size_t bufSize = (data_len > 0 ? data_len : 4096);
        char* pBuf = OGRE_ALLOC_T(char, bufSize, MEMCATEGORY_GENERAL);
        // Ensure read from begin of stream
        seek(0);
        String result;
        while (!eof())
        {
            size_t nr = read(pBuf, bufSize);
            result.append(pBuf, nr);
        }
        OGRE_FREE(pBuf, MEMCATEGORY_GENERAL);
        return result;
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    MemoryDataStream::MemoryDataStream(void* p_mem, size_t in_size, bool free_on_close, bool read_only)
        : MemoryDataStream("", p_mem, in_size, free_on_close, read_only)
    {
    }
    //-----------------------------------------------------------------------
    MemoryDataStream::MemoryDataStream(const String& name, void* p_mem, size_t in_size, 
        bool free_on_close, bool read_only)
        : DataStream(name)
    {
        data_ = pos_ = static_cast<uint8_t*>(p_mem);
        size_ = in_size;
        end_ = data_ + size_;
        free_on_close_ = free_on_close;
        access_ = static_cast<uint16_t>(read_only ? READ : (READ | WRITE));
        assert(end_ >= pos_);
    }

        static uint8_t empty[1] = {};


    //-----------------------------------------------------------------------
    MemoryDataStream::MemoryDataStream(DataStream* source, 
        bool free_on_close, bool read_only)
        : MemoryDataStream("", source, free_on_close, read_only)
    {

    }
    //-----------------------------------------------------------------------
    MemoryDataStream::MemoryDataStream(const String& name, DataStream* source,
        bool free_on_close, bool read_only)
        : DataStream(name)
    {
        // Copy data from incoming stream
        data_ = end_ = empty;
        size_ = 0;
        access_ = static_cast<uint16_t>(read_only ? READ : (READ | WRITE));
        copy_from(source, free_on_close);
    }

    void MemoryDataStream::copy_from(DataStream *source, bool free_on_close) {
        size_ = source->size();
        if (size_ == 0 && !source->eof())
        {
            String contents = source->as_string();
            size_ = contents.size();
            data_ = OGRE_ALLOC_T(uchar, size_, MEMCATEGORY_GENERAL);
            pos_ = data_;
            memcpy(data_, contents.data(), size_);
            end_ = data_ + size_;
        }
        else
        {
            data_ = OGRE_ALLOC_T(uchar, size_, MEMCATEGORY_GENERAL);
            pos_ = data_;
            end_ = data_ + source->read(data_, size_);
        }
        free_on_close_ = free_on_close;
        assert(end_ >= pos_);
    }


    MemoryDataStream::MemoryDataStream(size_t in_size, bool free_on_close, bool read_only)
        : DataStream()
    {
        size_ = in_size;
        data_ = OGRE_ALLOC_T(uchar, size_, MEMCATEGORY_GENERAL);
        pos_ = data_;
        end_ = data_ + size_;
        free_on_close_ = free_on_close;
        access_ = static_cast<uint16_t>(read_only ? READ : (READ | WRITE));
        assert(end_ >= pos_);
    }
    //-----------------------------------------------------------------------
    MemoryDataStream::MemoryDataStream(const String& name, size_t in_size, 
        bool free_on_close, bool read_only)
        : DataStream(name)
    {
        size_ = in_size;
        data_ = OGRE_ALLOC_T(uchar, size_, MEMCATEGORY_GENERAL);
        pos_ = data_;
        end_ = data_ + size_;
        free_on_close_ = free_on_close;
        access_ = static_cast<uint16_t>(read_only ? READ : (READ | WRITE));
        assert(end_ >= pos_);
    }
    //-----------------------------------------------------------------------
    MemoryDataStream::~MemoryDataStream()
    {
        close();
    }
    //-----------------------------------------------------------------------
    size_t MemoryDataStream::read(void* buf, size_t count)
    {
        size_t cnt = count;
        // Read over end of memory?
        if (pos_ + cnt > end_)
            cnt = end_ - pos_;
        if (cnt == 0)
            return 0;

        assert (cnt<=count);

        memcpy(buf, pos_, cnt);
        pos_ += cnt;
        return cnt;
    }
    //---------------------------------------------------------------------
    size_t MemoryDataStream::write(const void* buf, size_t count)
    {
        size_t written = 0;
        if (writable())
        {
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
    //-----------------------------------------------------------------------
    size_t MemoryDataStream::read_line(char* buf, size_t maxCount, 
        const String& delim)
    {
        // Deal with both Unix & Windows LFs
        bool trimCR = false;
        if (delim.find_first_of('\n') != String::npos)
        {
            trimCR = true;
        }

        size_t pos = 0;

        // Make sure pos can never go past the end of the data 
        while (pos < maxCount && pos_ < end_)
        {
            if (delim.find(*pos_) != String::npos)
            {
                // Trim off trailing CR if this was a CR/LF entry
                if (trimCR && pos && buf[pos-1] == '\r')
                {
                    // terminate 1 character early
                    --pos;
                }

                // Found terminator, skip and break out
                ++pos_;
                break;
            }

            buf[pos++] = *pos_++;
        }

        // terminate
        buf[pos] = '\0';

        return pos;
    }
    //-----------------------------------------------------------------------
    size_t MemoryDataStream::skip_line(const String& delim)
    {
        size_t pos = 0;

        // Make sure pos can never go past the end of the data 
        while (pos_ < end_)
        {
            ++pos;
            if (delim.find(*pos_++) != String::npos)
            {
                // Found terminator, break out
                break;
            }
        }

        return pos;

    }
    //-----------------------------------------------------------------------
    void MemoryDataStream::skip(long count)
    {
        size_t newpos = (size_t)( ( pos_ - data_ ) + count );
        assert( data_ + newpos <= end_ );        

        pos_ = data_ + newpos;
    }
    //-----------------------------------------------------------------------
    void MemoryDataStream::seek( size_t pos )
    {
        assert( data_ + pos <= end_ );
        pos_ = data_ + pos;
    }
    //-----------------------------------------------------------------------
    size_t MemoryDataStream::tell(void) const
    {
        //data_ is start, pos_ is current location
        return pos_ - data_;
    }
    //-----------------------------------------------------------------------
    bool MemoryDataStream::eof(void) const
    {
        return pos_ >= end_;
    }
    //-----------------------------------------------------------------------
    void MemoryDataStream::close(void)    
    {
        access_ = 0;

        if (free_on_close_ && data_)
        {
            OGRE_FREE(data_, MEMCATEGORY_GENERAL);
            data_ = 0;
        }
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    FileStreamDataStream::FileStreamDataStream(std::ifstream* s, bool free_on_close)
        : DataStream(), s_read_(s), s_ro_(s), s_rw_(0), free_on_close_(free_on_close), access_(0)
    {
        // calculate the size
        s_read_->seekg(0, std::ios_base::end);
        size_ = (size_t)s_read_->tellg();
        s_read_->seekg(0, std::ios_base::beg);
        determine_access();
    }
    //-----------------------------------------------------------------------
    FileStreamDataStream::FileStreamDataStream(const String& name, 
        std::ifstream* s, bool free_on_close)
        : DataStream(name), s_read_(s), s_ro_(s), s_rw_(0), free_on_close_(free_on_close), access_(0)
    {
        // calculate the size
        s_read_->seekg(0, std::ios_base::end);
        size_ = (size_t)s_read_->tellg();
        s_read_->seekg(0, std::ios_base::beg);
        determine_access();
    }
    //-----------------------------------------------------------------------
    FileStreamDataStream::FileStreamDataStream(const String& name, 
        std::ifstream* s, size_t inSize, bool freeOnClose)
        : DataStream(name), s_read_(s), s_ro_(s), s_rw_(0), free_on_close_(freeOnClose), access_(0)
    {
        // Size is passed in
        size_ = inSize;
        determine_access();
    }
    //---------------------------------------------------------------------
    FileStreamDataStream::FileStreamDataStream(std::fstream* s, bool freeOnClose)
        : DataStream(), s_read_(s), s_ro_(0), s_rw_(s), free_on_close_(freeOnClose), access_(0)
    {
        // writeable!
        // calculate the size
        s_read_->seekg(0, std::ios_base::end);
        size_ = (size_t)s_read_->tellg();
        s_read_->seekg(0, std::ios_base::beg);
        determine_access();

    }
    //-----------------------------------------------------------------------
    FileStreamDataStream::FileStreamDataStream(const String& name, 
        std::fstream* s, bool freeOnClose)
        : DataStream(name), s_read_(s), s_ro_(0), s_rw_(s), free_on_close_(freeOnClose), access_(0)
    {
        // writeable!
        // calculate the size
        s_read_->seekg(0, std::ios_base::end);
        size_ = (size_t)s_read_->tellg();
        s_read_->seekg(0, std::ios_base::beg);
        determine_access();
    }
    //-----------------------------------------------------------------------
    FileStreamDataStream::FileStreamDataStream(const String& name, 
        std::fstream* s, size_t inSize, bool freeOnClose)
        : DataStream(name), s_read_(s), s_ro_(0), s_rw_(s), free_on_close_(freeOnClose), access_(0)
    {
        // writeable!
        // Size is passed in
        size_ = inSize;
        determine_access();
    }
    //---------------------------------------------------------------------
    void FileStreamDataStream::determine_access()
    {
        access_ = 0;
        if (s_read_)
            access_ |= READ;
        if (s_rw_)
            access_ |= WRITE;
    }
    //-----------------------------------------------------------------------
    FileStreamDataStream::~FileStreamDataStream()
    {
        close();
    }
    //-----------------------------------------------------------------------
    size_t FileStreamDataStream::read(void* buf, size_t count)
    {
        s_read_->read(static_cast<char*>(buf), static_cast<std::streamsize>(count));
        return (size_t)s_read_->gcount();
    }
    //-----------------------------------------------------------------------
    size_t FileStreamDataStream::write(const void* buf, size_t count)
    {
        size_t written = 0;
        if (writable() && s_rw_)
        {
            s_rw_->write(static_cast<const char*>(buf), static_cast<std::streamsize>(count));
            written = count;
        }
        return written;
    }
    //-----------------------------------------------------------------------
    size_t FileStreamDataStream::read_line(char* buf, size_t maxCount, 
        const String& delim)
    {
        if (delim.empty())
        {
            OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "No delimiter provided",
                "FileStreamDataStream::read_line");
        }
        if (delim.size() > 1)
        {
            LogManager::singleton().log_warning(
                "FileStreamDataStream::read_line - using only first delimiter");
        }
        // Deal with both Unix & Windows LFs
        bool trimCR = false;
        if (delim.at(0) == '\n') 
        {
            trimCR = true;
        }
        // maxCount + 1 since count excludes terminator in getline
        s_read_->getline(buf, static_cast<std::streamsize>(maxCount+1), delim.at(0));
        size_t ret = (size_t)s_read_->gcount();
        // three options
        // 1) we had an eof before we read a whole line
        // 2) we ran out of buffer space
        // 3) we read a whole line - in this case the delim character is taken from the stream but not written in the buffer so the read data is of length ret-1 and thus ends at index ret-2
        // in all cases the buffer will be null terminated for us

        if (s_read_->eof()) 
        {
            // no problem
        }
        else if (s_read_->fail())
        {
            // Did we fail because of maxCount hit? No - no terminating character
            // in included in the count in this case
            if (ret == maxCount)
            {
                // clear failbit for next time 
                s_read_->clear();
            }
            else
            {
                OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR, 
                    "Streaming error occurred", 
                    "FileStreamDataStream::read_line");
            }
        }
        else 
        {
            // we need to adjust ret because we want to use it as a
            // pointer to the terminating null character and it is
            // currently the length of the data read from the stream
            // i.e. 1 more than the length of the data in the buffer and
            // hence 1 more than the _index_ of the NULL character
            --ret;
        }

        // trim off CR if we found CR/LF
        if (trimCR && ret && buf[ret-1] == '\r')
        {
            --ret;
            buf[ret] = '\0';
        }
        return ret;
    }
    //-----------------------------------------------------------------------
    void FileStreamDataStream::skip(long count)
    {
#if defined(STLPORT)
        // Workaround for STLport issues: After reached eof of file stream,
        // it's seems the stream was putted in intermediate state, and will be
        // fail if try to repositioning relative to current position.
        // Note: tellg() fail in this case too.
        if (s_read_->eof())
        {
            s_read_->clear();
            // Use seek relative to either begin or end to bring the stream
            // back to normal state.
            s_read_->seekg(0, std::ios::end);
        }
#endif      
        s_read_->clear(); //Clear fail status in case eof was set
        s_read_->seekg(static_cast<std::ifstream::pos_type>(count), std::ios::cur);
    }
    //-----------------------------------------------------------------------
    void FileStreamDataStream::seek( size_t pos )
    {
        s_read_->clear(); //Clear fail status in case eof was set
        s_read_->seekg(static_cast<std::streamoff>(pos), std::ios::beg);
    }
    //-----------------------------------------------------------------------
    size_t FileStreamDataStream::tell(void) const
    {
        s_read_->clear(); //Clear fail status in case eof was set
        return (size_t)s_read_->tellg();
    }
    //-----------------------------------------------------------------------
    bool FileStreamDataStream::eof(void) const
    {
        return s_read_->eof();
    }
    //-----------------------------------------------------------------------
    void FileStreamDataStream::close(void)
    {
        access_ = 0;
        if (s_read_)
        {
            // Unfortunately, there is no file-specific shared class hierarchy between fstream and ifstream (!!)
            if (s_ro_)
                s_ro_->close();
            if (s_rw_)
            {
                s_rw_->flush();
                s_rw_->close();
            }

            if (free_on_close_)
            {
                // delete the stream too
                OGRE_DELETE_T(s_ro_, basic_ifstream, MEMCATEGORY_GENERAL);
                OGRE_DELETE_T(s_rw_, basic_fstream, MEMCATEGORY_GENERAL);
            }

            s_read_ = 0;
            s_ro_ = 0; 
            s_rw_ = 0; 
        }
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    FileHandleDataStream::FileHandleDataStream(FILE* handle, uint16 accessMode)
        : DataStream(), mFileHandle(handle), access_(accessMode)
    {
        // Determine size
        fseek(mFileHandle, 0, SEEK_END);
        size_ = ftell(mFileHandle);
        fseek(mFileHandle, 0, SEEK_SET);
    }
    //-----------------------------------------------------------------------
    FileHandleDataStream::FileHandleDataStream(const String& name, FILE* handle, uint16 accessMode)
        : DataStream(name), mFileHandle(handle), access_(accessMode)
    {
        // Determine size
        fseek(mFileHandle, 0, SEEK_END);
        size_ = ftell(mFileHandle);
        fseek(mFileHandle, 0, SEEK_SET);
    }
    //-----------------------------------------------------------------------
    FileHandleDataStream::~FileHandleDataStream()
    {
        close();
    }
    //-----------------------------------------------------------------------
    size_t FileHandleDataStream::read(void* buf, size_t count)
    {
        return fread(buf, 1, count, mFileHandle);
    }
    //-----------------------------------------------------------------------
    size_t FileHandleDataStream::write(const void* buf, size_t count)
    {
        if (!writable())
            return 0;
        else
            return fwrite(buf, 1, count, mFileHandle);
    }
    //---------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void FileHandleDataStream::skip(long count)
    {
        fseek(mFileHandle, count, SEEK_CUR);
    }
    //-----------------------------------------------------------------------
    void FileHandleDataStream::seek( size_t pos )
    {
        fseek(mFileHandle, static_cast<long>(pos), SEEK_SET);
    }
    //-----------------------------------------------------------------------
    size_t FileHandleDataStream::tell(void) const
    {
        return ftell( mFileHandle );
    }
    //-----------------------------------------------------------------------
    bool FileHandleDataStream::eof(void) const
    {
        return feof(mFileHandle) != 0;
    }
    //-----------------------------------------------------------------------
    void FileHandleDataStream::close(void)
    {
        access_ = 0;
        if (mFileHandle != 0)
        {
            fclose(mFileHandle);
            mFileHandle = 0;
        }
    }
    //-----------------------------------------------------------------------

}
