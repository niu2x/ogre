add_executable(Example "")

target_sources(Example PRIVATE
    main.cpp
)

target_link_libraries(Example PUBLIC
    ${HYUE_LIB}
)

cxx_project_preset(Example)
