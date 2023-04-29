# Basic Build Environment
Stripped down framework based on Graham Sellers' examples from: `git clone https://github.com/openglsuperbible/sb7code`  

# Dependencies
This assumes you have an alternate **working** set of s7code examples. In particular the GLFW library already installed and ready to go.

# Usage
* Replace any code you want inside src/main.cpp (at the moment it is the code for the grass.cpp example)
* First time set up, run from top level (with the CMakeLists.txt): `cmake -G "MinGW Makefiles" -S . -B . -DCMAKE_INSTALL_PREFIX=C:\mingw-w64\mingw64\x86_64-w64-mingw32`
* When you want to build run: `mingw32-make` or `mingw32-make all` or `mingw32-make main` 
* You should find an executable in `bin`
  * Run this from your terminal: `.\bin\main.exe` 

# Reset
* If something goes wonky, you can 'reset' this build by deleting:
  * .\Makefile
  * .\CMakeLists.txt
  * .\cmake_install.cmake
  * .\CmakeFiles
* Or run the `cmake -G "MinGW Makefiles"...` with a `--fresh` tag