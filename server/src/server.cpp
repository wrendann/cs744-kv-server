#include <iostream>
#include <string>
#include <httplib.h>
#include <json.hpp>

#include "database.hpp"
#include "lrucache.hpp"

int main(int argc, char** argv) {
    const int CACHE_SIZE = 1024;
    const int NO_OF_SHARDS = 16;
    
    httplib::Server svr;
    svr.set_keep_alive_timeout(3);
    svr.set_keep_alive_max_count(50);
    svr.set_tcp_nodelay(true);

    if(argc > 1)
    {
        int num_threads = std::stoi(argv[1]);
        svr.new_task_queue = [num_threads] { return new httplib::ThreadPool(num_threads); };
    }

    using json = nlohmann::json;

    Database *db = new Database("dbname=kvstore user=user password=password host=kv_postgres port=5432");

    LRUCache *cache = new ShardedLRUCache(db, CACHE_SIZE, NO_OF_SHARDS);

    svr.Get("/ping", [](const httplib::Request& req, httplib::Response& res) {
        res.set_content("pong", "text/plain");
    });

    svr.Post("/kv", [cache](const httplib::Request& req, httplib::Response& res) {
        try {
            json j = json::parse(req.body);
            std::string key = j.at("key").get<std::string>();
            std::string value = j.at("value").get<std::string>();
            
            cache->addToCache(key, value);

            res.set_content("Success: Key=" + key, "text/plain"); 
            res.status = 201; 

        } catch (const std::exception& e) {
            res.set_content("Error processing request: " + std::string(e.what()), "text/plain");
            res.status = 500;
        }
    });

    svr.Get(R"(/kv/([^/]+))", [cache](const httplib::Request& req, httplib::Response& res) {
        try {
            std::string key = req.matches[1]; 

            std::string value = cache->readFromCache(key);

            res.set_content(value, "text/plain");
            res.status = 200;
        } catch (const std::exception& e) {
            res.set_content("Error processing request: " + std::string(e.what()), "text/plain");
            res.status = 500;
        }
    });

    svr.Delete(R"(/kv/([^/]+))", [cache](const httplib::Request& req, httplib::Response& res) {
        try {
            std::string key = req.matches[1];

            cache->removeFromCache(key);

            res.set_content("", "text/plain");
            res.status = 204;
        } catch (const std::exception& e) {
            res.set_content("Error processing request: " + std::string(e.what()), "text/plain");
            res.status = 500;
        }
    });

    std::cout << "Server listening on http://localhost:8080" << std::endl;
    svr.listen("0.0.0.0", 8080);
}