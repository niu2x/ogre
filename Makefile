build:
	cmake -S deps -B build_deps -G Ninja
	cmake --build build_deps -j 3
	cmake -S . -B build -G Ninja -DHYUE_BUILD_TEST=ON -DHYUE_BUILD_EXAMPLE=ON
	cmake --build build -j 3

.PHONY: build