#pragma once

#include <pqxx/pqxx>

class Database {
public:
    Database(const std::string& conn_str);

    void create(const std::string& key, const std::string& value);
    std::string read(const std::string& key);
    void remove(const std::string& key);

private:
    std::string conn_string;
    std::string_view create_kv_pair_query;
    std::string_view read_kv_pair_query;
    std::string_view delete_kv_pair_query;
    static thread_local std::unique_ptr<pqxx::connection> thread_connection_ptr;

    pqxx::connection& get_connection();
};