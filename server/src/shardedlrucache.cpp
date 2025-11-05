#include "lrucache.hpp"

int ShardedLRUCache::getShardIndex(const std::string &key) const
{
    return std::hash<std::string>{}(key) % num_of_shards;
}

CacheShard* ShardedLRUCache::getShard(const std::string &key)
{
    return shards[getShardIndex(key)];
}

ShardedLRUCache::ShardedLRUCache(Database *db, int capacity, int num_of_shards):
    db(db), num_of_shards(num_of_shards), shard_capacity(capacity/num_of_shards)
{
    for (int i = 0; i < num_of_shards; ++i) {
        shards.emplace_back(new CacheShard(shard_capacity));
    }
}

void ShardedLRUCache::addToCache(const std::string &key, const std::string &value)
{
    CacheShard* shard = getShard(key);
    db->create(key, value);
    std::lock_guard<std::mutex> guard(shard->shard_mutex);

    auto it = shard->cache.find(key);

    if (it != shard->cache.end()) {
        shard->lru_list.erase(it->second.second);
    } else if (shard->cache.size() >= shard->capacity){
        const std::string& lru_key = shard->lru_list.back();
        shard->cache.erase(lru_key);
        shard->lru_list.pop_back(); 
    }
    shard->lru_list.push_front(key);
    shard->cache[key] = {value, shard->lru_list.begin()};
}

std::string ShardedLRUCache::readFromCache(const std::string &key)
{
    CacheShard* shard = getShard(key);
    {
        std::lock_guard<std::mutex> guard(shard->shard_mutex);
        auto it = shard->cache.find(key);
        if (it != shard->cache.end()) {
            shard->lru_list.erase(it->second.second);
            shard->lru_list.push_front(key);
            it->second.second = shard->lru_list.begin();
            return it->second.first;
        }
    }
    std::string value = db->read(key);

    std::lock_guard<std::mutex> guard(shard->shard_mutex);
    if (shard->cache.size() >= shard->capacity) {
        const std::string& lru_key = shard->lru_list.back();
        shard->cache.erase(lru_key);
        shard->lru_list.pop_back();
    }
    shard->lru_list.push_front(key);
    shard->cache[key] = {value, shard->lru_list.begin()};
    return value;
}

void ShardedLRUCache::removeFromCache(const std::string &key)
{
    CacheShard* shard = getShard(key);
    std::lock_guard<std::mutex> guard(shard->shard_mutex);
    db->remove(key);
    auto it = shard->cache.find(key);
    if (it != shard->cache.end()) {
        shard->lru_list.erase(it->second.second);
        shard->cache.erase(it);
    }
}
