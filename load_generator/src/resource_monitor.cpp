#include "resource_monitor.hpp"
#include <fstream>
#include <sstream>
#include <cmath>
#include <unistd.h>
#include <iomanip>
#include <iostream>
#include <thread>

std::vector<MetricSnapshot> monitoring_data;

void monitor_server_resources(int pid, std::chrono::milliseconds interval, const bool& stop_flag) {
    if (pid <= 0) {
        std::cerr << "Invalid PID provided." << std::endl;
        return;
    }

    long long hertz = sysconf(_SC_CLK_TCK);
    const int NUM_CORES = 2;

    ProcState prev_state = {0, 0, 0, 0};

    std::string stat_path = "/proc/" + std::to_string(pid) + "/stat";
    std::string io_path = "/proc/" + std::to_string(pid) + "/io";
    std::string status_path = "/proc/" + std::to_string(pid) + "/status";

    auto read_proc_state = [&](ProcState& state) -> bool {
        std::ifstream stat_file(stat_path);
        if (!stat_file.is_open()) return false;
        long long dummy;
        stat_file >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> state.utime >> state.stime;
        stat_file.close();

        std::ifstream io_file(io_path);
        if (!io_file.is_open()) return false;
        std::string line;
        while (std::getline(io_file, line)) {
            if (line.rfind("read_bytes:", 0) == 0) {
                std::istringstream iss(line.substr(12));
                iss >> state.read_bytes;
            } else if (line.rfind("write_bytes:", 0) == 0) {
                std::istringstream iss(line.substr(13));
                iss >> state.write_bytes;
            }
        }
        io_file.close();
        return true;
    };

    if (!read_proc_state(prev_state)) {
        std::cerr << "Error: Initial read failed for PID " << pid << std::endl;
        return;
    }

    while (!stop_flag) {
        std::this_thread::sleep_for(interval);

        auto current_time = std::chrono::system_clock::now();
        double delta_t_sec = static_cast<double>(interval.count()) / 1000.0;
        
        ProcState curr_state;
        if (!read_proc_state(curr_state)) {
            if (!stop_flag) std::cerr << "Process " << pid << " terminated." << std::endl;
            return;
        }

        long long delta_P_ticks = (curr_state.utime - prev_state.utime) + (curr_state.stime - prev_state.stime);
        double available_ticks = delta_t_sec * hertz * NUM_CORES; 
        double cpu_percent = 0.0;
        if (available_ticks > 0) {
            cpu_percent = (static_cast<double>(delta_P_ticks) / available_ticks) * 100.0;
        }

        long long delta_read = curr_state.read_bytes - prev_state.read_bytes;
        long long delta_write = curr_state.write_bytes - prev_state.write_bytes;

        long long read_throughput = 0;
        long long write_throughput = 0;

        if (delta_t_sec > 0) {
            read_throughput = static_cast<long long>(std::round(static_cast<double>(delta_read) / delta_t_sec));
            write_throughput = static_cast<long long>(std::round(static_cast<double>(delta_write) / delta_t_sec));
        }

        long long rss_kb = 0;
        std::ifstream status_file_stream(status_path);
        if (status_file_stream.is_open()) {
            std::string line;
            while (std::getline(status_file_stream, line)) {
                if (line.rfind("VmRSS:", 0) == 0) {
                    std::istringstream iss(line.substr(6)); 
                    iss >> rss_kb;
                    break;
                }
            }
            status_file_stream.close();
        }

        MetricSnapshot snapshot = {
            current_time,
            cpu_percent,
            rss_kb,
            read_throughput,
            write_throughput
        };
        
        monitoring_data.push_back(snapshot);
        
        std::cout << current_time.time_since_epoch().count() << " | PID: " << pid << " | CPU: " << std::fixed << std::setprecision(2) << cpu_percent 
                  << "% | RSS: " << rss_kb << " KB | Read: " << read_throughput << " B/s | Write: " << write_throughput << " B/s" << std::endl;

        prev_state = curr_state;
    }
}