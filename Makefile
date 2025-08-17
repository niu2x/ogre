build:
	cmake -S . -B build -G Ninja -DHYUE_BUILD_TEST=ON -DHYUE_BUILD_EXAMPLE=ON
	cmake --build build -j 3

.PHONY: build