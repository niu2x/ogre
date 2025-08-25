#pragma once

#include <hyue/ArchiveFactory.h>

namespace hyue {

class HYUE_API ZipArchiveFactory : public ArchiveFactory {
public:
    const String& get_type(void) const override;

    using ArchiveFactory::create_instance;

    Archive* create_instance(const String& name, bool read_only) override;
};

} // namespace hyue
