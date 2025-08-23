#pragma once

#include <cstddef>
#include <cstdint>

#include <limits>
#include <memory>

#include <iosfwd>

#include <string>
#include <any>

#include <vector>
#include <map>
#include <list>

#include <hyue/export.h>

namespace hyue {

using String = std::string;
using StringVector = std::vector<String>;
using StringVectorPtr = std::shared_ptr<StringVector>;

using Any = std::any;
using std::any_cast;

template<class T>
using SharedPtr = std::shared_ptr<T>;

} // namespace hyue