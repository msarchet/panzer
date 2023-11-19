cmake_minimum_required(VERSION 3.21)

project(panzer LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)
file(GLOB_RECURSE SOURCES "src/*.cpp")
message(STATUS ${SOURCES})
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
add_executable(panzer ${SOURCES})
target_link_libraries(panzer PRIVATE Threads::Threads)
