#pragma once

#include <memory>

namespace hyue {

template<class T>
using SharedPtr = std::shared_ptr<T>;

}
