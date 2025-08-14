target_sources(${HYUE_LIB} PRIVATE
    src/zip/zip.c
    src/Any.cpp
    src/log.cpp
)

target_include_directories(${HYUE_LIB} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)