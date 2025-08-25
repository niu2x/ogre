enable_testing()

find_package(GTest REQUIRED)

file(GLOB test_src "test_*.cpp")

list(LENGTH test_src test_src_length)

add_executable(unittest "")

target_sources(unittest PRIVATE
    ${test_src}
)

cxx_project_preset(unittest)

# Link libraries
target_link_libraries(unittest PRIVATE 
    # test_lib 
    GTest::GTest 
    GTest::Main
    ${HYUE_LIB}
)

target_compile_definitions(unittest PRIVATE
    UNITTEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
    UNITTEST_COUNT=${test_src_length}
)

add_test(NAME hyue_unittest COMMAND unittest)
