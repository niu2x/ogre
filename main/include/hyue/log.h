#pragma once

#include <string>

#include <boost/log/trivial.hpp>

#define LOG(...) BOOST_LOG_TRIVIAL(__VA_ARGS__)

namespace hyue {

void panic(const std::string& error_msg);

}