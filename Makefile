build:
	git add OgreMain/ Components/ Samples;
	git-clang-format || true
	git add OgreMain/ Components/ Samples;
	cmake -S. -Bbuild
	cmake --build build -j 3

test:
	git grep OgreStableHeaders

.PHONY: build