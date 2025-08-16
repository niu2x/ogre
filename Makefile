build:
	cmake -S . -B build -G Ninja -DHYUE_BUILD_TEST=ON
	cmake --build build -j 3

.PHONY: build