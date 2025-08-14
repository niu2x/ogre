#ifndef HYUE_API_H
#define HYUE_API_H

// TODO: other platform support

#ifdef HYUE_STATIC_DEFINE
    #define HYUE_API
    #define HYUE_NO_EXPORT
#else
    #ifndef HYUE_API
        #ifdef HYUERT_EXPORTS
            /* We are building this library */
            #if defined(_MSC_VER)
                #define HYUE_API __declspec(dllexport)
            #else
                #define HYUE_API __attribute__((visibility("default")))
            #endif
        #else
            /* We are using this library */
            #if defined(_MSC_VER)
                #define HYUE_API __declspec(dllimport)
            #else
                #define HYUE_API __attribute__((visibility("default")))
            #endif
        #endif
    #endif

    #ifndef HYUE_NO_EXPORT
        #if defined(_MSC_VER)
            #define HYUE_NO_EXPORT
        #else
            #define HYUE_NO_EXPORT __attribute__((visibility("hidden")))
        #endif
    #endif
#endif

#ifndef HYUE_DEPRECATED
    #if defined(_MSC_VER)
        #define HYUE_DEPRECATED
    #else
        #define HYUE_DEPRECATED __attribute__((__deprecated__))
    #endif
#endif

#ifndef HYUE_DEPRECATED_EXPORT
    #define HYUE_DEPRECATED_EXPORT HYUE_API HYUE_DEPRECATED
#endif

#ifndef HYUE_DEPRECATED_NO_EXPORT
    #define HYUE_DEPRECATED_NO_EXPORT HYUE_NO_EXPORT HYUE_DEPRECATED
#endif

#endif /* HYUE_API_H */
