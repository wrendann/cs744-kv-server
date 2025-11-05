#include "load_generation.hpp"
#include <iostream>
#include <json.hpp>

std::string ping(httplib::Client &cli)
{
    auto res = cli.Get("/ping");
    if (!res || res->status != 200) {
        std::cerr << ("Failed to ping server\n");
    }
    return res->body;
}

void create_kv_pair(httplib::Client &cli, const std::string& key, const std::string& value)
{
    try{
        nlohmann::json payload_json;
        payload_json["key"] = key;
        payload_json["value"] = value;
        auto res = cli.Post("/kv", payload_json.dump(), "application/json");
        if (!res || res->status != 201) {
            std::cerr << ("Failed to create key-value pair: " + key) << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception in create_kv_pair: " << e.what() << std::endl;
    }
}

std::string get_value(httplib::Client &cli, const std::string& key)
{
    try{
        auto res = cli.Get("/kv/" + key);
        if (!res || res->status != 200) {
            std::cerr << ("Failed to get value for key: " + key) << std::endl;
            if(!res) {
                httplib::Error err = res.error();
                std::cerr << "Request failed with error code: " 
                            << httplib::to_string(err) << std::endl;
        
                switch (err) {
                    case httplib::Error::Connection:
                        std::cerr << "-> Possible cause: Server unreachable, firewall, or wrong port." << std::endl;
                        break;
                    case httplib::Error::SSLConnection:
                        std::cerr << "-> Possible cause: Tried to use HTTP on an HTTPS client/server or vice versa." << std::endl;
                        break;
                    case httplib::Error::Read:
                        std::cerr << "-> Possible cause: Server took too long to respond." << std::endl;
                        break;
                    // ... check other httplib::Error values for more detail
                    default:
                        std::cerr << "-> Unknown or generic connection error." << std::endl;
                        break;
                }
                return "";
            }
            std::cerr << "Status code: " << res->status << ", returning empty string." << std::endl;
            std::cout << "**Reason:** " << res->reason << std::endl;
            std::cout << "\n--- Headers ---" << std::endl;
            // The 'headers' member is a multimap, so you can iterate over it
            for (const auto& header : res->headers) {
                std::cout << header.first << ": " << header.second << std::endl;
            }
            if (res->body.empty()) {
                std::cout << "[Empty Body]" << std::endl;
            } else {
                // Print only the first 200 characters to prevent huge console dumps
                std::string preview = res->body.substr(0, 200);
                std::cout << preview << (res->body.size() > 200 ? "..." : "") << std::endl;
            }
            return "";
        }
        return res->body;
    } catch (const std::exception& e) {
        std::cerr << "Exception in get_value: " << e.what() << std::endl;
        return "";
    }
}

void delete_key(httplib::Client &cli, const std::string& key)
{
    try{
        auto res = cli.Delete("/kv/" + key);
        if (!res || res->status != 204) {
            std::cerr << ("Failed to delete key: " + key) << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception in delete_key: " << e.what() << std::endl;
    }
}