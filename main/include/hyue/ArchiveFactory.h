#pragma once

#include <hyue/FactoryObj.h>

namespace hyue {

class HYUE_API ArchiveFactory : public FactoryObj<Archive> {
public:
    virtual ~ArchiveFactory();

    virtual Archive* create_instance(const String& name, bool read_only) = 0;

    Archive* create_instance(const String& name) override
    {
        return create_instance(name, true);
    }
};

} // namespace hyue
