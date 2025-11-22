# cs744-kv-server/load_generator/CMakeLists.txt
add_executable(load_generator src/client.cpp src/load_generation.hpp
    src/generate_random.cpp src/requests.cpp
    src/put_all.cpp src/get_all.cpp src/get_popular.cpp src/get_put.cpp
    src/resource_monitor.hpp src/resource_monitor.cpp)

target_link_libraries(load_generator PRIVATE )

set_property(TARGET load_generator PROPERTY CXX_STANDARD 17)