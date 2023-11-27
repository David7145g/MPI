cmake_minimum_required(VERSION 3.24)
project(MPI)

set(CMAKE_CXX_STANDARD 17)

find_package(MPI REQUIRED)

add_executable(lab1 1/lab_1.cpp)
add_executable(lab2 2/lab_2.cpp)
add_executable(lab3 3/lab_3.cpp)
add_executable(lab4 4/lab_4.cpp)

target_link_libraries(lab1 PRIVATE MPI::MPI_CXX)
target_link_libraries(lab2 PRIVATE MPI::MPI_CXX)
target_link_libraries(lab3 PRIVATE MPI::MPI_CXX)
target_link_libraries(lab4 PRIVATE MPI::MPI_CXX)
