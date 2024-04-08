cmake_minimum_required(VERSION 3.20)
project(common)

set(CMAKE_CXX_STANDARD 23)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
set(CMAKE_CXX_FLAGS_UBSAN "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=undefined -fno-omit-frame-pointer")
set(CMAKE_EXE_LINKER_FLAGS_UBSAN "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=undefined -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_ASAN "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer -fno-common")
set(CMAKE_EXE_LINKER_FLAGS_ASAN "${CMAKE_EXEC_LINKER_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")

#execute_process(COMMAND sleep 9999)

set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

enable_testing()

add_subdirectory(src)
