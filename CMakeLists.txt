cmake_minimum_required(VERSION 3.17.0)

project(HYUE VERSION 0.1.0 LANGUAGES CXX)

# Include necessary submodules
list(APPEND CMAKE_MODULE_PATH
    "${PROJECT_SOURCE_DIR}/cmake"
)

include(cxx_project)

option(HYUE_BUILD_TEST "build UnitTest" OFF)

set(HYUE_LIB hyue)

add_library(${HYUE_LIB} SHARED "")

find_package(Boost CONFIG REQUIRED COMPONENTS 
    log
)

set(libzip_DIR build_deps/libzip/lib/cmake/libzip)
find_package(libzip REQUIRED)

cxx_project_preset(${HYUE_LIB})

target_include_directories(${HYUE_LIB} PUBLIC
    include/
)

target_link_libraries(${HYUE_LIB} PUBLIC
    Boost::log
    libzip::zip
)

add_subdirectory(main)

if(HYUE_BUILD_TEST)
    add_subdirectory(unittest)
endif()

if(HYUE_BUILD_EXAMPLE)
    add_subdirectory(Example)
endif()