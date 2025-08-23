#include <hyue/DynLib.h>

#include <dlfcn.h>

#include <hyue/log.h>
#include <hyue/panic.h>

namespace hyue {

DynLib::DynLib( const String& name )
:name_(name) 
{
}

DynLib::~DynLib() {}

void DynLib::load() {
    LOG(debug) << "Loading library " << name_;

    handle_ = (DynLibHandle) dlopen(name_.c_str(), RTLD_LAZY | RTLD_GLOBAL);

    if(!handle_) {
        panic("Could not load dynamic library " + name_ + ".  System Error: " + get_dyn_lib_error());
    }
}

void DynLib::unload() {
    // Log library unload
    LOG(debug) << "Unloading library " + name_;

    if(dlclose( handle_ ))
    {
        panic("Could not unload dynamic library " + name_ + ".  System Error: " + get_dyn_lib_error());
    }

}

String DynLib::get_dyn_lib_error() 
{
    return dlerror();
}


void* DynLib::get_symbol(const String& name) const
{
    return (void*)dlsym( handle_, name.c_str());
}

}