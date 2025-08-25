#pragma once

#include <hyue/Singleton.h>
#include <hyue/ArchiveFactory.h>

namespace hyue {

class HYUE_API Root : public Singleton<Root> {
public:
    Root(const String &log_file="hyue.log");
    ~Root();

private:
    std::vector<std::unique_ptr<ArchiveFactory>> archive_factories_;
};

} // namespace hyue