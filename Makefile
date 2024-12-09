build:
	git add OgreMain/ Components/
	git-clang-format || true
	git add OgreMain/ Components/
	cmake -S. -Bbuild
	cmake --build build -j 3

test:
	git grep OgreStableHeaders

.PHONY: build