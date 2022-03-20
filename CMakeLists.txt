project(infra-elements LANGUAGES CXX)

cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(PROJECT_VERSION 0.0.1.0)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(Dependencies)
include(CTest)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(examples)