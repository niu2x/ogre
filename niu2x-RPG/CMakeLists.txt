add_executable(niu2x-RPG "")

add_subdirectory(lua-5.3)

target_sources(niu2x-RPG PRIVATE
	"src/main.cpp"
)

# target_include_directories(niu2x-RPG PRIVATE
	# ${CMAKE_CURRENT_SOURCE_DIR}/../OgreMain/include/
# )

target_link_libraries(niu2x-RPG PRIVATE 
	OgreMain
	OgreBites
	lua
)