cmake_minimum_required(VERSION 3.31)

project(internal-cheat-template)

set(CMAKE_CXX_STANDARD 23)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_subdirectory(dependencies)

add_subdirectory(dll-injector)
add_subdirectory(template-dll)
add_subdirectory(stub-executable)
