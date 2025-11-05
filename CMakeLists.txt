# cs744-kv-server/CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(KVServerProject CXX)

include_directories(include)

add_subdirectory(server)
add_subdirectory(load_generator)