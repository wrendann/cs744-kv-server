#include <string>
#include <random>

#include "load_generation.hpp"

std::string generate_random_string(int length) {
    static const std::string ALPHANUMERIC_CHARS =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    thread_local std::mt19937 generator(std::random_device{}());
    
    thread_local std::uniform_int_distribution<int> distribution(
        0, ALPHANUMERIC_CHARS.length() - 1
    );

    std::string result;
    if (length > 0) {
        result.reserve(length);
        
        for (int i = 0; i < length; ++i) {
            result += ALPHANUMERIC_CHARS[distribution(generator)];
        }
    }

    return result;
}

int generate_random_number(int min_value, int max_value) {
    thread_local std::mt19937 generator(std::random_device{}());
    thread_local std::uniform_int_distribution<int> distribution(min_value, max_value);
    return distribution(generator);
}