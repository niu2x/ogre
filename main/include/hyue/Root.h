#pragma once

#include <hyue/export.h>
#include <hyue/Singleton.h>
#include <hyue/String.h>

namespace hyue {

class HYUE_API Root : public Singleton<Root> {
public:
    Root(const String &log_file="hyue.log");
    ~Root();
};

} // namespace hyue