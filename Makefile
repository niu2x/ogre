build:
	cmake -S. -B build/ -DOGRE_STATIC=ON -DCMAKE_BUILD_TYPE=Release; cmake --build  build/ -j2

.PHONY: build
