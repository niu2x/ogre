#pragma once

#include <cstddef>
#include <cstdint>

#include <limits>
#include <memory>

#include <iosfwd>

#include <string>

#include <vector>
#include <map>
#include <list>

#include <hyue/export.h>

namespace hyue {

using String = std::string;
using StringVector = std::vector<String>;
using StringVectorPtr = std::shared_ptr<StringVector>;

} // namespace hyue