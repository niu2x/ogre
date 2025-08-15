#include <stdexcept>

#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>

#include <hyue/log.h>
#include <hyue/exception.h>

#include "init_logging.h"

namespace hyue {

namespace log = boost::log;

void init_logging() {
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
                << "] "
                << log::expressions::smessage
            )
    );

    log::add_common_attributes();
}

void panic(const std::string &panic_msg) {
    LOG(fatal) << panic_msg;
    throw std::runtime_error(panic_msg);
}

}