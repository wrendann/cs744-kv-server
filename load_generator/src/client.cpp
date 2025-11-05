#include <iostream>
#include <vector>
#include "load_generation.hpp"

int main(int argc, char** argv)
{
    if(argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <num_threads> <time_duration> <type_of_load_test>\n";
        std::cerr << "'put' for put all, 'get' for get all, 'pop' for get popular, 'gp' for get + put\n";
        return 1;
    }
    int num_threads = std::stoi(argv[1]);
    int time_duration = std::stoi(argv[2]);
    std::string load_test_type = argv[3];

    if(load_test_type == "put") {
        perform_put_all_load_test(num_threads, time_duration);
    } else if(load_test_type == "get") {
        perform_get_all_load_test(num_threads, time_duration);
    } else if(load_test_type == "pop") {
        perform_get_popular_load_test(num_threads, time_duration);
    } else if(load_test_type == "gp") {
        perform_get_put_load_test(num_threads, time_duration);
    }else if(load_test_type == "check") {
        httplib::Client cli("http://kv_server:8080");
        std::string response = ping(cli);
        std::cout << "Ping response: " << response << std::endl;
        std::vector<std::pair<std::string, std::string>> key_value_store;
        for (int i = 0; i < 5000; ++i) {
            std::string key = generate_random_string(10);
            std::string value = std::to_string(i);
            key_value_store.emplace_back(key, value);
            create_kv_pair(cli, key, value);
        }
        for (int i = 0; i < 5000; ++i) {
            const std::string& key = key_value_store[i].first;
            std::string value = get_value(cli, key);
            if (value != key_value_store[i].second) {
                std::cerr << "Data mismatch for key: " << key << std::endl;
            }
        }
        for (int i = 0; i < 5000; ++i) {
            const std::string& key = key_value_store[i].first;
            delete_key(cli, key);
        }
        std::cout << "Data consistency check completed.\n";
    } else {
        std::cerr << "Unknown load test type: " << load_test_type << "\n";
        return 1;
    }
    
    return 0;
}