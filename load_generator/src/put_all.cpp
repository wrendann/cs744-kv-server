#include "load_generation.hpp"
#include <thread>
#include <string>
#include <atomic>
#include <vector>
#include <chrono>
#include <iostream>
#include <unordered_set>

void thread_put_all(std::atomic<bool> &stop_flag, LoadTestResult* result)
{
    std::unordered_set<std::string> local_keys;
    httplib::Client cli("http://kv_server:8080");
    cli.set_keep_alive(true);
    cli.set_tcp_nodelay(true);
    cli.set_connection_timeout(5); 
    cli.set_read_timeout(5);
    while (!stop_flag) {
        if(generate_random_number(0, 2) == 0 && !local_keys.empty()) {
            auto it = local_keys.begin();
            //std::advance(it, generate_random_number(0, local_keys.size()-1));
            auto start = std::chrono::high_resolution_clock::now();
            delete_key(cli, *it);
            auto end = std::chrono::high_resolution_clock::now();
            result->total_requests++;
            result->total_response_time_ms += std::chrono::duration<double, std::milli>(end - start).count();
            local_keys.erase(it);
            continue;
        }
        std::string key = generate_random_string(10);
        std::string value = std::to_string(result->total_requests);
        auto start = std::chrono::high_resolution_clock::now();
        create_kv_pair(cli, key, value);
        auto end = std::chrono::high_resolution_clock::now();
        result->total_requests++;
        result->total_response_time_ms += std::chrono::duration<double, std::milli>(end - start).count();
        local_keys.insert(key);
    }
}

void perform_put_all_load_test(int thread_num, int duration_seconds)
{
    std::cout << "Starting PUT ALL Load Test with " << thread_num << " threads for " << duration_seconds << " seconds.\n";

    std::vector<std::thread> threads;
    std::vector<LoadTestResult*> results(thread_num);
    std::atomic<bool> stop_flag;
    stop_flag = false;
    for (int i = 0; i < thread_num; ++i) {
        results[i] = new LoadTestResult();
        threads.emplace_back(thread_put_all, std::ref(stop_flag), results[i]);
    }
    std::this_thread::sleep_for(std::chrono::seconds(duration_seconds));
    stop_flag = true;
    for (auto &t : threads) {
        t.join();
    }
    int total_requests = 0;
    double total_response_time_ms = 0.0;
    for (const auto& res : results) {
        total_requests += res->total_requests;
        total_response_time_ms += res->total_response_time_ms;
        delete res;
    }
    double avg_throughput = static_cast<double>(total_requests) / duration_seconds;
    double avg_response_time_ms = total_requests > 0 ? total_response_time_ms / total_requests : 0.0;
    std::cout << "PUT ALL Load Test Completed: \n";
    std::cout << "Total Requests: " << total_requests << "\n";
    std::cout << "Average Throughput (requests/sec): " << avg_throughput << "\n";
    std::cout << "Average Response Time (ms): " << avg_response_time_ms << "\n";
}