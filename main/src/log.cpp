#include <stdexcept>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
// #include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>

#include <hyue/log.h>
#include <hyue/exception.h>

#include "init_logging.h"

namespace hyue {

namespace log = boost::log;

void init_logging(const String &log_file) {
    // 添加全局属性 AppName
    log::core::get()->add_global_attribute(
        "AppName",
        log::attributes::constant<std::string>("Hyue")
    );
    
    // 设置前缀格式
    log::add_console_log(
        std::clog,
        log::keywords::format = (
            log::expressions::stream 
                << "[" 
                << log::expressions::attr<std::string>("AppName") 
                << "] ["
                << log::trivial::severity
                << "] "
                << log::expressions::smessage
        )
    );

    log::add_file_log(
        log::keywords::file_name = log_file,
        log::keywords::rotation_size = 10 * 1024 * 1024, // 日志文件大小达到10MB时轮转
        log::keywords::max_size = 50 * 1024 * 1024,     // 最大保留50MB日志
        log::keywords::time_based_rotation = log::sinks::file::rotation_at_time_point(0, 0, 0), // 每天午夜轮转
        log::keywords::format = (
            log::expressions::stream 
                << "[" 
                << log::expressions::attr<std::string>("AppName") 
                << "] ["
                << log::trivial::severity
                << "] "
                << log::expressions::smessage
        ),
        log::keywords::auto_flush = true
    );

    log::add_common_attributes();
}

void panic(const std::string &panic_msg) {
    LOG(fatal) << panic_msg;
    throw std::runtime_error(panic_msg);
}

}