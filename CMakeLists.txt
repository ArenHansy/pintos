cmake_minimum_required(VERSION 3.14)
project(Pintos)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_FLAGS -m32)
set(CMAKE_CXX_FLAGS -m32)

include_directories(${PROJECT_SOURCE_DIR} "lib/" "lib/kernel/" "lib/user/" "devices/" "examples/" "filesys/" "threads/" "userprog/" "utils/")
file(GLOB SOURCES "*/*.c" "*/*/*.c" "*/*.h" "*/*/*.h" main.cpp)


add_executable(untitled main.cpp ${SOURCES})
