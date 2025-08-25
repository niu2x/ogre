#include <hyue/FileSystemArchiveFactory.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <fstream>

#include <hyue/panic.h>
#include <hyue/log.h>
#include <hyue/StringUtils.h>

namespace hyue {

class FileSystemArchive : public Archive {
public:
    FileSystemArchive(const String& name, const String& arch_type, bool read_only);

    ~FileSystemArchive();

    /// @copydoc Archive::isCaseSensitive
    bool is_case_sensitive(void) const override;

    /// @copydoc Archive::load
    void load() override;
    /// @copydoc Archive::unload
    void unload() override;

    /// @copydoc Archive::open
    DataStreamPtr open(const String& filename, bool read_only = true) const override;

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
    /** Utility method to retrieve all files in a directory matching pattern.
    @param pattern
        File pattern.
    @param recursive
        Whether to cascade down directories.
    @param dirs
        Set to @c true if you want the directories to be listed instead of files.
    @param simple_list
        Populated if retrieving a simple list.
    @param detailList
        Populated if retrieving a detailed list.
    */
    void find_files(const String& pattern,
                    bool recursive,
                    bool dirs,
                    StringVector* simple_list,
                    FileInfoList* detail_list) const;

    void find_files_recursive(const FilePath& path,
                              const String& pattern,
                              bool recursive,
                              bool dirs,
                              StringVector* simple_list,
                              FileInfoList* detail_list) const;
};

bool g_ignore_hidden = true;

FileSystemArchive::FileSystemArchive(const String& name, const String& arch_type, bool read_only)
: Archive(name, arch_type)
{
    // Even failed attempt to write to read only location violates Apple AppStore validation
    // process. And successful writing to some probe file does not prove that whole location with
    // subfolders is writable. Therefore we accept read only flag from outside and do not try to be
    // too smart.
    read_only_ = read_only;
}

bool FileSystemArchive::is_case_sensitive(void) const
{
#if HYUE_OS_WINDOWS
    return false;
#else
    return true;
#endif
}

FileSystemArchive::~FileSystemArchive()
{
    unload();
}

//-----------------------------------------------------------------------
void FileSystemArchive::load()
{
    // nothing to do here
}

void FileSystemArchive::unload()
{
    // nothing to see here, move along
}

FilePath concatenate_path(const FilePath& base, const FilePath& name)
{
    if (base.empty() || name.is_absolute())
        return name;
    else
        return base / name;
}

bool is_absolute_path(const FilePath& path)
{
    return path.is_absolute();
}

size_t get_file_size(const String& full_path)
{
    struct stat st;
    int ret = stat(full_path.c_str(), &st);

    size_t st_size = ret == 0 ? st.st_size : 0;
    return st_size;
}

DataStreamPtr open_file_stream(const String& full_path, std::ios::openmode mode, const String& name)
{
    // Use filesystem to determine size
    // (quicker than streaming to the end and back)
    size_t st_size = get_file_size(full_path);

    std::istream* base_stream = nullptr;
    std::ifstream* ro_stream = nullptr;
    std::fstream* rw_stream = nullptr;

    if (mode & std::ios::out) {
        rw_stream = new std::fstream;
        rw_stream->open(full_path.c_str(), mode);
        base_stream = rw_stream;
    } else {
        ro_stream = new std::ifstream;
        ro_stream->open(full_path.c_str(), mode);
        base_stream = ro_stream;
    }

    // Should check ensure open succeeded, in case fail for some reason.
    if (base_stream->fail()) {
        delete ro_stream;
        delete rw_stream;
        LOG(error) << "Cannot open file: " + full_path;
        return nullptr;
    }

    /// Construct return stream, tell it to delete on destroy
    DataStreamPtr stream;
    const String& stream_name = name.empty() ? full_path : name;
    if (rw_stream) {
        // use the writeable stream
        stream = std::make_shared<FileStreamDataStream>(stream_name, rw_stream, st_size);
    } else {
        // read-only stream
        stream = std::make_shared<FileStreamDataStream>(stream_name, ro_stream, st_size);
    }
    return stream;
}

DataStreamPtr FileSystemArchive::open(const String& filename, bool read_only) const
{
    if (!read_only && is_read_only()) {
        panic("Cannot open a file in read-write mode in a read-only archive");
    }

    // Always open in binary mode
    // Also, always include reading
    std::ios::openmode mode = std::ios::in | std::ios::binary;

    if (!read_only)
        mode |= std::ios::out;

    return open_file_stream(concatenate_path(name_, filename), mode, filename);
}

DataStreamPtr FileSystemArchive::create(const String& filename)
{
    if (is_read_only()) {
        panic("Cannot create a file in a read-only archive");
    }

    String full_path = concatenate_path(name_, filename);

    // Always open in binary mode
    // Also, always include reading
    std::ios::openmode mode = std::ios::out | std::ios::binary;

    std::fstream* rw_stream = new std::fstream;

    rw_stream->open(full_path.c_str(), mode);

    // Should check ensure open succeeded, in case fail for some reason.
    if (rw_stream->fail()) {
        delete rw_stream;
        panic("Cannot open file: " + filename);
    }

    /// Construct return stream, tell it to delete on destroy
    return std::make_shared<FileStreamDataStream>(filename, rw_stream, 0, true);
}

void FileSystemArchive::remove(const String& filename)
{
    if (is_read_only()) {
        panic("Cannot remove a file from a read-only archive");
    }
    String full_path = concatenate_path(name_, filename);
    ::remove(full_path.c_str());
}

StringVector FileSystemArchive::list(bool recursive, bool dirs) const
{
    // directory change requires locking due to saved returns
    StringVector ret;
    find_files("*", recursive, dirs, &ret, nullptr);
    return ret;
}

void FileSystemArchive::find_files_recursive(const FilePath& path,
                                             const String& pattern,
                                             bool recursive,
                                             bool dirs,
                                             StringVector* simple_list,
                                             FileInfoList* detail_list) const
{

    for (const auto& entry : std_fs::directory_iterator(path)) {

        auto filename = entry.path().filename();
        if (g_ignore_hidden) {
            if ((!filename.empty()) && filename.u8string()[0] == '.') {
                continue;
            }
        }

        if (filename == "." || filename == "..")
            continue;

        auto entry_path = entry.path();

        bool should_add = false;

        if (std_fs::is_directory(entry.status())) {
            if (recursive) {
                find_files_recursive(entry_path,
                                     pattern,
                                     recursive,
                                     dirs,
                                     simple_list,
                                     detail_list);
            }

            if (dirs) {
                should_add = true;
            }

        } else if (std_fs::is_regular_file(entry.status())) {
            if (!dirs) {
                should_add = true;
            }
        }

        should_add = should_add && StringUtils::fnmatch(entry_path, pattern);

        // todo pattern
        if (should_add) {
            if (simple_list) {
                simple_list->push_back(entry_path);
            }

            if (detail_list) {

                auto file_size = get_file_size(entry_path);

                FileInfo f_info;
                f_info.archive = this;
                f_info.filename = entry_path;
                f_info.dir = entry_path.parent_path();
                f_info.basename = entry_path.filename();
                f_info.compressed_size = file_size;
                f_info.uncompressed_size = file_size;
                detail_list->push_back(std::move(f_info));
            }
        }
    }
}

void FileSystemArchive::find_files(const String& pattern,
                                   bool recursive,
                                   bool dirs,
                                   StringVector* simple_list,
                                   FileInfoList* detail_list) const
{
    find_files_recursive(name_, pattern, recursive, dirs, simple_list, detail_list);
}

FileInfoList FileSystemArchive::list_file_info(bool recursive, bool dirs) const
{
    FileInfoList ret;

    find_files("*", recursive, dirs, nullptr, &ret);

    return ret;
}

StringVector FileSystemArchive::find(const String& pattern, bool recursive, bool dirs) const
{
    StringVector ret;

    find_files(pattern, recursive, dirs, &ret, nullptr);

    return ret;
}

FileInfoList FileSystemArchive::find_file_info(const String& pattern,
                                               bool recursive,
                                               bool dirs) const
{
    FileInfoList ret;

    find_files(pattern, recursive, dirs, nullptr, &ret);

    return ret;
}

bool FileSystemArchive::exists(const String& filename) const
{
    if (filename.empty())
        return false;

    String full_path = concatenate_path(name_, filename);

    struct stat st;
    bool ret = (stat(full_path.c_str(), &st) == 0);

    // stat will return true if the filename is absolute, but we need to check
    // the file is actually in this archive
    if (ret && is_absolute_path(filename)) {
        // only valid if full path starts with our base
        if (is_case_sensitive()) {
            ret = StringUtils::starts_with(full_path, name_, false);
        } else {

            ret = StringUtils::starts_with(full_path, name_, true);
        }
    }

    return ret;
}

//-----------------------------------------------------------------------
// FileSystemArchiveFactory
//-----------------------------------------------------------------------
const String& FileSystemArchiveFactory::get_type(void) const
{
    static String name = "FileSystem";
    return name;
}

Archive* FileSystemArchiveFactory::create_instance(const String& name, bool read_type)
{
    return new FileSystemArchive(name, get_type(), read_type);
}

void FileSystemArchiveFactory::set_ignore_hidden(bool ignore)
{
    g_ignore_hidden = ignore;
}

bool FileSystemArchiveFactory::is_ignore_hidden()
{
    return g_ignore_hidden;
}

} // namespace hyue
