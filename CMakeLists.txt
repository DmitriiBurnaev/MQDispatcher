project(infra-elements LANGUAGES CXX)

cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(PROJECT_VERSION 0.0.1.0)
#set(CMAKE_OSX_ARCHITECTURES x86_64;arm64)
#option(BUILD_EXAMPLES "Build examples" ON)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(Dependencies)
include(CTest)

add_subdirectory(src)
add_subdirectory(tests)
#if(BUILD_EXAMPLES)
add_subdirectory(examples)
#endif()