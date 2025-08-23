#pragma once

#include <hyue/type.h>
#include <hyue/Singleton.h>

namespace hyue {

class HYUE_API Root : public Singleton<Root> {
public:
    Root(const String &log_file="hyue.log");
    ~Root();
};

} // namespace hyue