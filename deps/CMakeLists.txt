cmake_minimum_required(VERSION 3.17.0)

project(HYUE_deps VERSION 0.1.0 LANGUAGES CXX)

# Include necessary submodules
list(APPEND CMAKE_MODULE_PATH
    "${PROJECT_SOURCE_DIR}/../cmake"
)

include(build_deps)
