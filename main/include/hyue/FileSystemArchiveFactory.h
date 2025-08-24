#pragma once

#include <hyue/ArchiveFactory.h>

namespace hyue {

class HYUE_API FileSystemArchiveFactory : public ArchiveFactory {
public:
    const String& get_type(void) const override;

    using ArchiveFactory::create_instance;

    Archive* create_instance(const String& name, bool read_only) override;

    /// Set whether filesystem enumeration will include hidden files or not.
    /// This should be called prior to declaring and/or initializing filesystem
    /// resource locations. The default is true (ignore hidden files).
    static void set_ignore_hidden(bool ignore);

    /// Get whether hidden files are ignored during filesystem enumeration.
    static bool is_ignore_hidden();
};

} // namespace hyue