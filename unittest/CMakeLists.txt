enable_testing()

find_package(GTest REQUIRED)

file(GLOB test_src "test_*.cpp")

list(LENGTH test_src test_src_length)

add_executable(unit_test "")

target_sources(unit_test PRIVATE
    ${test_src}
)

cxx_project_preset(unit_test)

# Link libraries
target_link_libraries(unit_test PRIVATE 
    # test_lib 
    GTest::GTest 
    GTest::Main
    ${HYUE_LIB}
)

target_compile_definitions(unit_test PRIVATE
    UNIT_TEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
    UNIT_TEST_COUNT=${test_src_length}
)

add_test(NAME hyue_unit_test COMMAND unit_test)
