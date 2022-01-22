cmake_minimum_required(VERSION 3.5)

project(reflection LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

aux_source_directory(src SRC_LIST)
aux_source_directory(test TEST_LIST)
add_executable(reflection ${SRC_LIST} ${TEST_LIST})
