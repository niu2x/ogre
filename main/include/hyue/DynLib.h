#pragma once

#include <hyue/type.h>

namespace hyue {

class HYUE_API DynLib {
public:
    DynLib(const String& name);
    ~DynLib();

    /** Load the library
    */

    void load();
    /** Unload the library
    */
    void unload();
    /// Get the name of the library
    const String& get_name(void) const { return name_; }

    /**
        Returns the address of the given symbol from the loaded library.
        @param
            strName The name of the symbol to search for
        @return
            If the function succeeds, the returned value is a handle to
            the symbol.
        @par
            If the function fails, the returned value is <b>NULL</b>.

    */
    void* get_symbol( const String& symbol_name ) const;
private:

    String name_;

    using DynLibHandle = void *;
    DynLibHandle handle_;

    String get_dyn_lib_error();
};

}