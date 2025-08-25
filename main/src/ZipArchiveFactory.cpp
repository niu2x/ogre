#include <hyue/ZipArchiveFactory.h>

#include <hyue/Archive.h>
#include <hyue/DataStream.h>

#include <zip.h>

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