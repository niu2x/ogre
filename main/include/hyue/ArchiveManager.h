#pragma once

#include <hyue/Singleton.h>

namespace hyue {

class HYUE_API ArchiveManager : public Singleton<ArchiveManager> {
private:
    typedef std::map<String, ArchiveFactory*> ArchiveFactoryMap;
    /// Factories available to create archives, indexed by archive type (String identifier e.g. 'Zip')
    ArchiveFactoryMap mArchFactories;
    /// Currently loaded archives
    typedef std::map<String, Archive*> ArchiveMap;
    ArchiveMap mArchives;

public:
    /** Default constructor - should never get called by a client app.
     */
    ArchiveManager();
    /** Default destructor.
     */
    virtual ~ArchiveManager();

    /** Opens an archive for file reading.

        The archives are created using class factories within
        extension libraries.
        @param filename
            The filename that will be opened
        @param archiveType
            The type of archive that this is. For example: "Zip".
        @param readOnly
            Whether the Archive is read only
        @return
            If the function succeeds, a valid pointer to an Archive
            object is returned.
        @par
            If the function fails, an exception is thrown.
    */
    Archive* load(const String& filename, const String& archiveType, bool readOnly);

    /** Unloads an archive.

        You must ensure that this archive is not being used before removing it.
    */
    void unload(Archive* arch);
    /** Unloads an archive by name.

        You must ensure that this archive is not being used before removing it.
    */
    void unload(const String& filename);
    typedef MapIterator<ArchiveMap> ArchiveMapIterator;
    /** Get an iterator over the Archives in this Manager. */
    ArchiveMapIterator getArchiveIterator(void);

    /** Adds a new ArchiveFactory to the list of available factories.

        Plugin developers who add new archive codecs need to call
        this after defining their ArchiveFactory subclass and
        Archive subclasses for their archive type.
    */
    void addArchiveFactory(ArchiveFactory* factory);
    /// @copydoc Singleton::getSingleton()
    static ArchiveManager& getSingleton(void);
    /// @copydoc Singleton::getSingleton()
    static ArchiveManager* getSingletonPtr(void);
};

} // namespace hyue
