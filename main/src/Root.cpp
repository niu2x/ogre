#include <hyue/Root.h>

#include <hyue/log.h>
#include <hyue/FileSystemArchiveFactory.h>
#include <hyue/ZipArchiveFactory.h>
#include <hyue/ArchiveManager.h>

#include "init_logging.h"

namespace hyue {

Root::Root(const String &log_file) { 
    init_logging(log_file);

    LOG(debug) << "HYUE Initialising";

    archive_factories_.push_back(std::make_unique<FileSystemArchiveFactory>());
    archive_factories_.push_back(std::make_unique<ZipArchiveFactory>());
    for (auto& item : archive_factories_) {
        ArchiveManager::get_singleton()->add_archive_factory(item.get());
    }
}

Root::~Root()
{
    archive_factories_.clear();
}

template <>
Root* Singleton<Root>::singleton_ = nullptr;

} // namespace hyue