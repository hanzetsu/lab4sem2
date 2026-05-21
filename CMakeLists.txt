cmake_minimum_required(VERSION 3.16)
project(Lab4 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OLD_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../lab2sem2/include")

set(NEW_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(Lab4 source/main.cpp)

target_include_directories(Lab4 PRIVATE
    ${NEW_INCLUDE_PATH}
    ${OLD_INCLUDE_PATH}
)
add_executable(test_lazysequence tests/test_lazysequence.cpp)
target_include_directories(test_lazysequence PRIVATE include ${OLD_INCLUDE_PATH})

add_executable(test_manual tests/test_manual.cpp)
target_include_directories(test_manual PRIVATE include ${OLD_INCLUDE_PATH})
