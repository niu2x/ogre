#pragma once

#include <cstddef>
#include <cstdint>

#include <memory>
#include <string>
#include <vector>
#include <list>

#include <hyue/export.h>

namespace hyue {

using String = std::string;
using StringVector = std::vector<String>;
using StringVectorPtr = std::shared_ptr<StringVector>;

} // namespace hyue