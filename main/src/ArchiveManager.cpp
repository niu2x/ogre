#include <hyue/ArchiveManager.h>

#include <hyue/log.h>

namespace hyue {

// using CreateFunc = void (*)(Archive**, const String&);

//-----------------------------------------------------------------------
template <>
ArchiveManager* Singleton<ArchiveManager>::singleton_ = 0;

//-----------------------------------------------------------------------
ArchiveManager::ArchiveManager()
{
}

ArchiveFactory* ArchiveManager::get_archive_factory(const String& archive_type)
{
    auto it = arch_factories_.find(archive_type);
    if (it == arch_factories_.end()) {
        // Factory not found
        panic("Cannot find an ArchiveFactory for type '" + archive_type + "'");
    }
    return it->second;
}

//-----------------------------------------------------------------------
Archive* ArchiveManager::load(const String& filename, const String& archive_type, bool read_only)
{
    ArchiveMap::iterator i = archives_.find(filename);
    Archive* p_arch = 0;

    if (i == archives_.end()) {
        // Search factories
        auto factory = get_archive_factory(archive_type);

        p_arch = factory->create_instance(filename, read_only);
        p_arch->load();
        archives_[filename] = p_arch;

    } else {
        p_arch = i->second;
        HYUE_ASSERT(p_arch->is_read_only() == read_only, "existing archive location has different readOnly status");
    }

    return p_arch;
}

//-----------------------------------------------------------------------
void ArchiveManager::unload(Archive* arch)
{
    unload(arch->get_name());
}
// //-----------------------------------------------------------------------
void ArchiveManager::unload(const String& filename)
{
    ArchiveMap::iterator i = archives_.find(filename);

    if (i != archives_.end()) {
        i->second->unload();
        // Find factory to destroy. An archive factory created this file, it should still be there!
        auto factory = get_archive_factory(i->second->get_type());
        factory->destroy_instance(i->second);

        archives_.erase(i);
    }
}

const ArchiveMap* ArchiveManager::get_archives() const
{
    return &archives_;
}

ArchiveManager::~ArchiveManager()
{
    // Thanks to http://www.viva64.com/en/examples/V509/ for finding the error for us!
    // (originally, it detected we were throwing using OGRE_EXCEPT in the destructor)
    // We now raise an assert.

    // Unload & delete resources in turn
    for (auto& a : archives_) {
        // Unload
        a.second->unload();
        // Find factory to destroy. An archive factory created this file, it should still be there!
        auto factory = get_archive_factory(a.second->get_type());
        factory->destroy_instance(a.second);
    }
    // Empty the list
    archives_.clear();
}

//-----------------------------------------------------------------------
void ArchiveManager::add_archive_factory(ArchiveFactory* factory)
{
    arch_factories_.emplace(factory->get_type(), factory);
    LOG(info) << "ArchiveFactory for type '" << factory->get_type() << "' registered";
}

} // namespace hyue
