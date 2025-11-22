#pragma once

#include <chrono>
#include <vector>

struct MetricSnapshot {
    std::chrono::system_clock::time_point timestamp;
    double cpu_percent;
    long long resident_set_size_kb;
    long long read_throughput_bytes_sec;
    long long write_throughput_bytes_sec;
};

struct ProcState {
    long long utime;
    long long stime;
    long long read_bytes;
    long long write_bytes;
};

extern std::vector<MetricSnapshot> monitoring_data;

void monitor_server_resources(int pid, std::chrono::milliseconds interval, const bool& stop_flag);