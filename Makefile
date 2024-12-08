build:
	git add OgreMain/;
	git-clang-format || true
	git add OgreMain/;
	cmake -S. -Bbuild
	cmake --build build -j 3

test:
	git grep OgreStableHeaders

.PHONY: build