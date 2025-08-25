enable_testing()

find_package(GTest REQUIRED)

add_executable(unit_test 
    test_string.cpp
    test_bitwise.cpp
    test_archive.cpp
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
)

add_test(NAME hyue_unit_test COMMAND unit_test)
