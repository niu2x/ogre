target_sources(${HYUE_LIB} PRIVATE
    src/zip/zip.c
    src/log.cpp
    src/thread.cpp
    src/Any.cpp
    src/Singleton.cpp
    src/SharedPtr.cpp
)

target_include_directories(${HYUE_LIB} PUBLIC
    # $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    include/
)