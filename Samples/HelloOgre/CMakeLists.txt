add_executable(HelloOgre WIN32 )


if (OGRE_BUILD_RENDERSYSTEM_D3D9 AND OGRE_STATIC)
    target_include_directories(HelloOgre PRIVATE ${DirectX9_LIBRARY_DIR})
endif()


# Fix for static build with MinGW
if (OGRE_BUILD_RENDERSYSTEM_D3D9 AND OGRE_STATIC)
   target_link_libraries(HelloOgre PRIVATE ${DirectX9_LIBRARIES})
endif()

target_link_libraries(HelloOgre PRIVATE OgreMain ${OGRE_PLUGIN_LIBRARIES} ${SAMPLE_LIBRARIES} OgreBites)

# configure HelloOgre build
ogre_config_common(HelloOgre)


target_sources(HelloOgre PRIVATE main.cpp)