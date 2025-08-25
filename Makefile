core_jobs := $(shell expr $(shell nproc) - 1)

build:
	echo "core_jobs $(core_jobs)"
	cmake -S deps -B build_deps -G Ninja
	cmake --build build_deps -j $(core_jobs)
	cmake -S . -B build -G Ninja -DHYUE_BUILD_TEST=ON -DHYUE_BUILD_EXAMPLE=ON
	cmake --build build -j $(core_jobs)

.PHONY: build