#pragma once

#include <httplib.h>

typedef struct LoadTestResult {
    int total_requests;
    double total_response_time_ms;
    LoadTestResult() : total_requests(0), total_response_time_ms(0.0) {}
} LoadTestResult;

std::string ping(httplib::Client &cli);
void create_kv_pair(httplib::Client &cli, const std::string& key, const std::string& value);
std::string get_value(httplib::Client &cli, const std::string& key);
void delete_key(httplib::Client &cli, const std::string& key);

std::string generate_random_string(int length);
int generate_random_number(int min_value, int max_value);

void perform_put_all_load_test(int thread_num, int duration_seconds);
void perform_get_all_load_test(int thread_num, int duration_seconds);
void perform_get_popular_load_test(int thread_num, int duration_seconds);
void perform_get_put_load_test(int thread_num, int duration_seconds);