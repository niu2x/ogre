build:
	cmake -S . -B build -G Ninja
	cmake --build build -j 3
	
.PHONY: build