include(ExternalProject)

ExternalProject_Add(
    libzip
    GIT_REPOSITORY https://github.com/nih-at/libzip
    GIT_TAG        v1.11.4
    PREFIX         ${CMAKE_BINARY_DIR}/deps/libzip
    CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
)

add_dependencies(${HYUE_LIB} libzip)

set(libzip_DIR ${CMAKE_BINARY_DIR}/deps/libzip/lib/cmake/libzip)
