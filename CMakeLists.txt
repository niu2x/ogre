cmake_minimum_required(VERSION 3.17.0)

project(HYUE VERSION 0.1.0 LANGUAGES CXX C)

# Include necessary submodules
list(APPEND CMAKE_MODULE_PATH
    "${PROJECT_SOURCE_DIR}/cmake"
)

include(cxx_project)

set(HYUE_LIB hyue)

add_library(${HYUE_LIB} SHARED "")
cxx_project_preset(${HYUE_LIB})

target_include_directories(${HYUE_LIB} PUBLIC
    # During build, use local path
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    # When installed, use relative path
    $<INSTALL_INTERFACE:include>
)

add_subdirectory(main)