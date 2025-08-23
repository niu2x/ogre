#pragma once

#include <hyue/type.h>

namespace hyue {

HYUE_API void panic(const std::string& error_msg);

}

#define HYUE_ASSERT(cond, message) if(!(cond)) hyue::panic(message);