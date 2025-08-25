#include <hyue/ZipArchiveFactory.h>

#include <hyue/StringUtils.h>
#include <hyue/Archive.h>
#include <hyue/log.h>
#include <hyue/panic.h>
#include <hyue/DataStream.h>

extern "C" {
#include "zip/zip.h"
}

namespace hyue {

class ZipArchive : public Archive {

public:
    ZipArchive(const String& name,
               const String& archType,
               const uint8_t* extern_buf = 0,
               size_t extern_buf_sz = 0);
    ~ZipArchive();
    /// @copydoc Archive::isCaseSensitive
    bool is_case_sensitive(void) const override
    {
        return true;
    }

    /// @copydoc Archive::load
    void load() override;
    /// @copydoc Archive::unload
    void unload() final override;

    /// @copydoc Archive::open
    DataStreamPtr open(const String& filename, bool readOnly = true) const override;

    /// @copydoc Archive::create
    DataStreamPtr create(const String& filename) override;

    /// @copydoc Archive::remove
    void remove(const String& filename) override;

    /// @copydoc Archive::list
    StringVector list(bool recursive = true, bool dirs = false) const override;

    /// @copydoc Archive::listFileInfo
    FileInfoList list_file_info(bool recursive = true, bool dirs = false) const override;

    /// @copydoc Archive::find
    StringVector find(const String& pattern,
                      bool recursive = true,
                      bool dirs = false) const override;

    /// @copydoc Archive::findFileInfo
    FileInfoList find_file_info(const String& pattern,
                                bool recursive = true,
                                bool dirs = false) const override;

    /// @copydoc Archive::exists
    bool exists(const String& filename) const override;

private:
    /// Handle to root zip file
    zip_t* zip_file_;
    MemoryDataStreamPtr buffer_;
    /// File list (since zziplib seems to only allow scanning of dir tree once)
    FileInfoList file_list_;
};

ZipArchive::ZipArchive(const String& name,
                       const String& arch_type,
                       const uint8_t* extern_buf,
                       size_t extern_buf_sz)
: Archive(name, arch_type),
  zip_file_(nullptr)
{
    if (extern_buf)
        buffer_ = std::make_shared<MemoryDataStream>(const_cast<uint8_t*>(extern_buf),
                                                     extern_buf_sz);
}
//-----------------------------------------------------------------------
ZipArchive::~ZipArchive()
{
    unload();
}

DataStreamPtr open_file_stream(const String& full_path,
                               std::ios::openmode mode,
                               const String& name = "");

void ZipArchive::load()
{
    if (!zip_file_) {
        if (!buffer_) {
            auto file_stream = open_file_stream(name_, std::ios::binary);
            buffer_ = std::make_shared<MemoryDataStream>(file_stream.get());
        }

        zip_file_ = zip_stream_open((const char*)buffer_->get_ptr(), buffer_->get_size(), 0, 'r');

        // Cache names
        int n = zip_entries_total(zip_file_);
        for (int i = 0; i < n; ++i) {
            FileInfo info;
            info.archive = this;

            zip_entry_openbyindex(zip_file_, i);

            info.filename = zip_entry_name(zip_file_);
            // Get basename / path
            StringUtils::split_filename(info.filename, &info.dir, &info.basename);

            // Get sizes
            info.uncompressed_size = zip_entry_size(zip_file_);
            info.compressed_size = zip_entry_comp_size(zip_file_);

            if (zip_entry_isdir(zip_file_)) {
                info.filename = info.filename.substr(0, info.filename.length() - 1);
                StringUtils::split_filename(info.filename, &info.dir, &info.basename);
                // Set compressed size to -1 for folders; anyway nobody will check
                // the compressed size of a folder, and if he does, its useless anyway
                info.compressed_size = 0;
            }

            zip_entry_close(zip_file_);
            file_list_.push_back(info);
        }
    }
}

void ZipArchive::unload()
{
    if (zip_file_) {
        zip_close(zip_file_);
        zip_file_ = nullptr;
    }
    file_list_.clear();
    buffer_.reset();
}

DataStreamPtr ZipArchive::open(const String& filename, bool readOnly) const
{
    // zip is not threadsafe
    String look_ip_filename = filename;

    bool open = zip_entry_open(zip_file_, look_ip_filename.c_str(), true) == 0;

    if (!open) {
        LOG(error) << "could not open " + look_ip_filename;
        return nullptr;
    }

    // Construct & return stream
    auto ret = std::make_shared<MemoryDataStream>(look_ip_filename,
                                                  zip_entry_size(zip_file_),
                                                  true,
                                                  true);

    if (zip_entry_noallocread(zip_file_, ret->get_ptr(), ret->get_size()) < 0) {
        panic("could not read " + look_ip_filename);
    }

    zip_entry_close(zip_file_);
    return ret;
}

DataStreamPtr ZipArchive::create(const String& filename)
{
    panic("Modification of zipped archives is not implemented");
    return nullptr;
}
//---------------------------------------------------------------------
void ZipArchive::remove(const String& filename)
{
    panic("Modification of zipped archives is not implemented");
}

StringVector ZipArchive::list(bool recursive, bool dirs) const
{
    StringVector ret;

    for (auto& f : file_list_) {
        if ((dirs == (f.compressed_size == size_t(0))) && (recursive || f.dir.empty())) {
            ret.push_back(f.filename);
        }
    }

    return ret;
}
//-----------------------------------------------------------------------
FileInfoList ZipArchive::list_file_info(bool recursive, bool dirs) const
{
    FileInfoList ret;
    for (auto& f : file_list_) {
        if ((dirs == (f.compressed_size == size_t(0))) && (recursive || f.dir.empty())) {
            ret.push_back(f);
        }
    }

    return ret;
}
//-----------------------------------------------------------------------
StringVector ZipArchive::find(const String& pattern, bool recursive, bool dirs) const
{
    StringVector ret;

    for (auto& f : file_list_) {
        if ((dirs == (f.compressed_size == size_t(0))) && (recursive)) {
            if (StringUtils::fnmatch(f.filename, pattern)) {
                ret.push_back(f.filename);
            }
        }
    }

    return ret;
}
//-----------------------------------------------------------------------
FileInfoList ZipArchive::find_file_info(const String& pattern, bool recursive, bool dirs) const
{
    FileInfoList ret;
    // If pattern contains a directory name, do a full match
    for (auto& f : file_list_) {
        if ((dirs == (f.compressed_size == size_t(0))) && (recursive)) {
            // Check name matches pattern (zip is case insensitive)
            if (StringUtils::fnmatch(f.filename, pattern)) {
                ret.push_back(f);
            }
        }
    }

    return ret;
}

bool ZipArchive::exists(const String& filename) const
{
    String clean_name = filename;

    return std::find_if(file_list_.begin(),
                        file_list_.end(),
                        [&clean_name](auto& fi) { return fi.filename == clean_name; })
           != file_list_.end();
}

//-----------------------------------------------------------------------
// ZipArchiveFactory
//-----------------------------------------------------------------------

Archive* ZipArchiveFactory::create_instance(const String& name, bool read_only)
{
    if (!read_only)
        return nullptr;

    return new ZipArchive(name, get_type());
}
//-----------------------------------------------------------------------
const String& ZipArchiveFactory::get_type(void) const
{
    static String name = "Zip";
    return name;
}

} // namespace hyue