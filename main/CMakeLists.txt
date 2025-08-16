target_sources(${HYUE_LIB} PRIVATE
    src/zip/zip.c
    src/log.cpp
    src/thread.cpp
    src/Any.cpp
    src/DynLib.cpp
    src/Singleton.cpp
    src/String.cpp
)

target_include_directories(${HYUE_LIB} PUBLIC
    # $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    include/
)

target_link_libraries(${HYUE_LIB} PUBLIC 
    dl
)