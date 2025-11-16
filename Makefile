# doom2016minhook Copyright (C) 2025 kernaltrap8
# This program comes with ABSOLUTELY NO WARRANTY
# This is free software, and you are welcome to redistribute it
# under certain conditions

.SILENT:

all: build-minhook
	@echo "Building injector & DLL..."
	mkdir -p build
	x86_64-w64-mingw32-g++ -O2 -shared -static -fPIC src/dllmain.cpp -I./include -L./lib -lminhook -o build/hook.dll -Wl,--kill-at
	x86_64-w64-mingw32-g++ -O2 -static src/main.cpp -o build/injector.exe
clone:
	if [ ! -d "dependencies/minhook" ]; then \
		echo "Cloning minhook..." && \
		mkdir -p dependencies && \
		git clone --depth 1 https://github.com/TsudaKageyu/minhook dependencies/minhook; \
	fi
build-minhook: clone
	@echo "Building minhook..."
	cd dependencies/minhook && mkdir -p _build && cd _build && \
	x86_64-w64-mingw32-g++ -O2 -Wall -Werror -I../include -I../src -c ../src/*.c ../src/hde/*.c && \
	x86_64-w64-mingw32-ar rcs ../../../lib/libminhook.a *.o
