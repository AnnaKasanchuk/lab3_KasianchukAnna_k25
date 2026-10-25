cmake_minimum_required(VERSION 4.3)
project(lab3_KasianchukAnna_k25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_executable(lab3_KasianchukAnna_k25 main.cpp)
target_link_libraries(lab3_KasianchukAnna_k25 Threads::Threads)