#pragma once

#include <hyue/DataStream.h>

namespace hyue {

class Archive;

struct HYUE_API FileInfo {
    /// The archive in which the file has been found (for info when performing
    /// multi-Archive searches, note you should still open through ResourceGroupManager)
    const Archive* archive;
    /// The file's fully qualified name
    String filename;
    /// Path name; separated by '/' and ending with '/'
    String dir;
    /// Base filename
    String basename;
    /// Compressed size
    size_t compressed_size;
    /// Uncompressed size
    size_t uncompressed_size;
    // is dir
    bool is_dir;
};

using FileInfoList = std::vector<FileInfo>;

/** Archive-handling class.

    An archive is a generic term for a container of files. This may be a
    filesystem folder, it may be a compressed archive, it may even be
    a remote location shared on the web. This class is designed to be
    subclassed to provide access to a range of file locations.
@par
    Instances of this class are never constructed or even handled by end-user
    applications. They are constructed by custom ArchiveFactory classes,
    which plugins can register new instances of using ArchiveManager.
    End-user applications will typically use ResourceManager or
    ResourceGroupManager to manage resources at a higher level, rather than
    reading files directly through this class. Doing it this way allows you
    to benefit from OGRE's automatic searching of multiple file locations
    for the resources you are looking for.
*/
class HYUE_API Archive {

public:
    /** Constructor - don't call direct, used by ArchiveFactory.
     */
    Archive(const String& name, const String& arch_type);

    /** Default destructor.
     */
    virtual ~Archive();

    /// Get the name of this archive
    const String& get_name(void) const
    {
        return name_;
    }

    /// Returns whether this archive is case sensitive in the way it matches files
    virtual bool is_case_sensitive(void) const = 0;

    /** Loads the archive.

        This initializes all the internal data of the class.
    @warning
        Do not call this function directly, it is meant to be used
        only by the ArchiveManager class.
    */
    virtual void load() = 0;

    /** Unloads the archive.
    @warning
        Do not call this function directly, it is meant to be used
        only by the ArchiveManager class.
    */
    virtual void unload() = 0;

    /** Reports whether this Archive is read-only, or whether the contents
        can be updated.
    */
    virtual bool is_read_only() const
    {
        return read_only_;
    }

    /** Open a stream on a given file.
    @note
        There is no equivalent 'close' method; the returned stream
        controls the lifecycle of this file operation.
    @param filename The fully qualified name of the file
    @param readOnly Whether to open the file in read-only mode or not (note,
        if the archive is read-only then this cannot be set to false)
    @return A shared pointer to a DataStream which can be used to
        read / write the file. If the file is not present, returns a null
        shared pointer.
    */
    virtual DataStreamPtr open(const String& filename, bool read_only = true) const = 0;

    /** Create a new file (or overwrite one already there).
    @note If the archive is read-only then this method will fail.
    @param filename The fully qualified name of the file
    @return A shared pointer to a DataStream which can be used to
    read / write the file.
    */
    virtual DataStreamPtr create(const String& filename);

    /** Delete a named file.
    @remarks Not possible on read-only archives
    @param filename The fully qualified name of the file
    */
    virtual void remove(const String& filename);

    /** List all file names in the archive.
    @note
        This method only returns filenames, you can also retrieve other
        information using listFileInfo.
    @param recursive Whether all paths of the archive are searched (if the
        archive has a concept of that)
    @param dirs Set to true if you want the directories to be listed
        instead of files
    @return A list of filenames matching the criteria, all are fully qualified
    */
    virtual StringVector list(bool recursive = true, bool dirs = false) const = 0;

    /** List all files in the archive with accompanying information.
    @param recursive Whether all paths of the archive are searched (if the
        archive has a concept of that)
    @param dirs Set to true if you want the directories to be listed
        instead of files
    @return A list of structures detailing quite a lot of information about
        all the files in the archive.
    */
    virtual FileInfoList list_file_info(bool recursive = true, bool dirs = false) const = 0;

    /** Find all file or directory names matching a given pattern
        in this archive.
    @note
        This method only returns filenames, you can also retrieve other
        information using findFileInfo.
    @param pattern The pattern to search for; wildcards (*) are allowed
    @param recursive Whether all paths of the archive are searched (if the
        archive has a concept of that)
    @param dirs Set to true if you want the directories to be listed
        instead of files
    @return A list of filenames matching the criteria, all are fully qualified
    */
    virtual StringVector find(const String& pattern, bool recursive = true, bool dirs = false) const = 0;

    /** Find out if the named file exists (note: fully qualified filename required) */
    virtual bool exists(const String& filename) const = 0;

    /** Retrieve the modification time of a given file */
    // virtual time_t getModifiedTime(const String& filename) const = 0;

    /** Find all files or directories matching a given pattern in this
        archive and get some detailed information about them.
    @param pattern The pattern to search for; wildcards (*) are allowed
    @param recursive Whether all paths of the archive are searched (if the
    archive has a concept of that)
    @param dirs Set to true if you want the directories to be listed
        instead of files
    @return A list of file information structures for all files matching
        the criteria.
    */
    virtual FileInfoList find_file_info(const String& pattern,
                                        bool recursive = true,
                                        bool dirs = false) const
        = 0;

    /// Return the type code of this Archive
    const String& get_type(void) const
    {
        return type_;
    }

protected:
    /// Archive name
    String name_;
    /// Archive type code
    String type_;
    /// Read-only flag
    bool read_only_;
};

} // namespace hyue
