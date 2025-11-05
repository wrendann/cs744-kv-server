#include <iostream>
#include "database.hpp"

thread_local std::unique_ptr<pqxx::connection> Database::thread_connection_ptr = nullptr;

Database::Database(const std::string& conn_str){
    conn_string = conn_str;
    pqxx::connection conn(conn_str);
    pqxx::work txn(conn);
    txn.exec("CREATE TABLE IF NOT EXISTS kv_store ("
             "key TEXT PRIMARY KEY, "
             "value TEXT NOT NULL);");
    txn.commit();
    create_kv_pair_query = "INSERT INTO kv_store (key, value) VALUES ($1, $2) "
                           "ON CONFLICT (key) DO UPDATE SET value = EXCLUDED.value;";
    read_kv_pair_query = "SELECT value FROM kv_store WHERE key = $1;";
    delete_kv_pair_query = "DELETE FROM kv_store WHERE key = $1;";
}

void Database::create(const std::string& key, const std::string& value) {
    pqxx::connection& conn = get_connection();
    pqxx::work txn(conn);
    txn.exec(create_kv_pair_query, pqxx::params{key, value});
    txn.commit();
}

std::string Database::read(const std::string& key) {
    pqxx::connection& conn = get_connection();
    pqxx::work txn(conn);
    pqxx::result r = txn.exec(read_kv_pair_query, pqxx::params{key});
    
    if (r.empty()) {
        throw std::runtime_error("Key not found");
    }
    
    return r[0][0].as<std::string>();
}

void Database::remove(const std::string& key) {
    pqxx::connection& conn = get_connection();
    pqxx::work txn(conn);
    txn.exec(delete_kv_pair_query, pqxx::params{key});
    txn.commit();
}

pqxx::connection &Database::get_connection()
{
    if (thread_connection_ptr == nullptr) {
        thread_connection_ptr = std::make_unique<pqxx::connection>(conn_string);
    }
    return *thread_connection_ptr;
}
