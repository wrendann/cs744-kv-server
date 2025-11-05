# cs744-kv-server/server/CMakeLists.txt
add_executable(kv_server src/server.cpp src/database.cpp src/database.hpp 
    src/lrucache.hpp src/singlelocklrucache.cpp src/shardedlrucache.cpp)

target_link_libraries(kv_server PRIVATE pqxx)

set_property(TARGET kv_server PROPERTY CXX_STANDARD 17)