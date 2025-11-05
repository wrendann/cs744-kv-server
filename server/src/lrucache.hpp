#pragma once

#include "database.hpp"
#include <unordered_map>
#include <list>
#include <mutex>

class LRUCache {
public:
    virtual void addToCache(const std::string& key, const std::string& value) = 0;
    virtual std::string readFromCache(const std::string& key) = 0;
    virtual void removeFromCache(const std::string& key) = 0;
};

class SingleLockLRUCache : public LRUCache {
public:
    SingleLockLRUCache(Database *db, int capacity);
    void addToCache(const std::string& key, const std::string& value) override;
    std::string readFromCache(const std::string& key) override;
    void removeFromCache(const std::string& key) override;
private:
    Database *db;
    int capacity;
    std::unordered_map<std::string, std::pair<std::string, std::list<std::string>::iterator>> cache;
    std::list<std::string> lru_list;
    std::mutex cache_mutex;
};

struct CacheShard {
    int capacity;
    std::unordered_map<std::string, std::pair<std::string, std::list<std::string>::iterator>> cache;
    std::list<std::string> lru_list;
    std::mutex shard_mutex;

    CacheShard(int cap) : capacity(cap) {}
};

class ShardedLRUCache : public LRUCache {
public:
    ShardedLRUCache(Database *db, int capacity, int num_of_shards);
    void addToCache(const std::string& key, const std::string& value) override;
    std::string readFromCache(const std::string& key) override;
    void removeFromCache(const std::string& key) override;
private:
    Database *db;
    int num_of_shards;
    int shard_capacity;
    std::vector<CacheShard*> shards;
    int getShardIndex(const std::string& key) const;
    CacheShard* getShard(const std::string& key);
};