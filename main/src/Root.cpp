#include <hyue/Root.h>

#include "init_logging.h"
#include <hyue/log.h>

namespace hyue {

Root::Root(const String &log_file) { 
    init_logging(log_file);

    LOG(debug) << "HYUE Initialising";
}

Root::~Root() { }

template <>
Root* Singleton<Root>::singleton_ = nullptr;

} // namespace hyue