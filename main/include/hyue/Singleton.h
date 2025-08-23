#pragma once

#include <boost/noncopyable.hpp>

#include <hyue/panic.h>

namespace hyue {

template<class T> 
class Singleton: private boost::noncopyable {
public:
    Singleton()
    {
        HYUE_ASSERT(!singleton_, "There can be only one singleton");
        singleton_ = static_cast<T*>(this);
    }
    ~Singleton()
    {
        singleton_ = nullptr;
    }

    static T* get_singleton()
    {
        return singleton_;
    }
private:
    static T* singleton_;
};

}