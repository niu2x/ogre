enable_testing()

find_package(GTest REQUIRED)

add_executable(unit_test 
    test_string.cpp
    test_bitwise.cpp
)

# Link libraries
target_link_libraries(unit_test PRIVATE 
    # test_lib 
    GTest::GTest 
    GTest::Main
    ${HYUE_LIB}
)

add_test(NAME hyue_unit_test COMMAND unit_test)

