set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR amd64)

set(tools /usr/local/gcc-linaro-5.5.0-2017.10-x86_64_arm-linux-gnueabihf)
set(CMAKE_C_COMPILER ${tools}/bin/arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER ${tools}/bin/arm-linux-gnueabihf-g++)
#set(CMAKE_EXE_LINKER_FLAGS "-static")
#set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
#set(CMAKE_SHARED_LIBRARY_PREFIX ".a")

SET(CMAKE_FIND_ROOT_PATH ${tools})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

#set(CMAKE_C_FLAGS "-flto -ffreestanding -nostdlib -static ${CMAKE_C_FLAGS}" )
#set(CMAKE_CXX_FLAGS "-march=armv8-a -fopenmp ${CMAKE_CXX_FLAGS}")

add_definitions(-D__ARM_NEON)
add_definitions(-DLINUX)
