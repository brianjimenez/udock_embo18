
miniMolViewer is a minimal molecular viewer.
//to change


how to compile: 



on linux:


install the SDL2 lib (probably with apt-get install libsdl2-dev)

first you need to install bullet


mkdir build
cd build
cmake ..
make



on windows:
install mingw (we need gcc, g++, make)

mkdir build
cd build
cmake .. -G "MinGW Makefiles"
mingw32-make.exe