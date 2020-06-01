cmake_minimum_required(VERSION 3.16)
project(ult)

set(CMAKE_CXX_STANDARD 20)

add_library(ult STATIC
        src/scheduler.cpp
        src/task.cpp)
target_include_directories(ult PUBLIC include)

enable_testing()
add_subdirectory(third_party/googletest)

function(unittest NAME)
    add_executable(tests.${NAME} ${ARGN})
    target_link_libraries(tests.${NAME} PUBLIC gtest gtest_main ult)
    add_test(${NAME} tests.${NAME})
endfunction()

add_executable(unittests
    tests/non_preemptive.cpp)
target_link_libraries(unittests PUBLIC gtest gtest_main ult)
add_test(unittests unittests)
