function(cxx_project_preset)
    foreach(target IN ITEMS ${ARGN})
        set_target_properties(${target} PROPERTIES 
            CXX_STANDARD 17
            C_VISIBILITY_PRESET hidden
            CXX_VISIBILITY_PRESET hidden
            VISIBILITY_INLINES_HIDDEN ON
            POSITION_INDEPENDENT_CODE ON
        )
    endforeach()
endfunction()