build:
	cmake -S. -B build/ -DOGRE_STATIC=ON -DCMAKE_BUILD_TYPE=Release; cmake --build  build/ -j2

.PHONY: build

# /home/niu2x/project/ogre/Samples,/home/niu2x/project/ogre/OgreMain, /home/niu2x/project/ogre/PlugIns, /home/niu2x/project/ogre/RenderSystems, /home/niu2x/project/ogre/Components
