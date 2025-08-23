#pragma once

#include <hyue/Singleton.h>
#include <hyue/Archive.h>
#include <hyue/ArchiveFactory.h>

namespace hyue {

using ArchiveMap = std::map<String, Archive*>;

class HYUE_API ArchiveManager : public Singleton<ArchiveManager> {
public:
    /** Default constructor - should never get called by a client app.
     */
    ArchiveManager();
    /** Default destructor.
     */
    virtual ~ArchiveManager();

    Archive* load(const String& filename, const String& archive_type, bool read_only);

    /** Unloads an archive.

        You must ensure that this archive is not being used before removing it.
    */
    void unload(Archive* arch);
    /** Unloads an archive by name.

        You must ensure that this archive is not being used before removing it.
    */
    void unload(const String& filename);

    const ArchiveMap* get_archives() const;
    /** Adds a new ArchiveFactory to the list of available factories.

        Plugin developers who add new archive codecs need to call
        this after defining their ArchiveFactory subclass and
        Archive subclasses for their archive type.
    */
    void add_archive_factory(ArchiveFactory* factory);

private:
    ArchiveFactory* get_archive_factory(const String& archive_type);

private:
    using ArchiveFactoryMap = std::map<String, ArchiveFactory*>;
    /// Factories available to create archives, indexed by archive type (String identifier e.g. 'Zip')
    ArchiveFactoryMap arch_factories_;
    /// Currently loaded archives
    ArchiveMap archives_;
};

} // namespace hyue
