#include "lrucache.hpp"

SingleLockLRUCache::SingleLockLRUCache(Database *db, int capacity)
    : db(db), capacity(capacity) {}

void SingleLockLRUCache::addToCache(const std::string& key, const std::string& value)
{
    db->create(key, value);
    std::lock_guard<std::mutex> guard(cache_mutex);
    auto it = cache.find(key);
    if (it != cache.end()) {
        lru_list.erase(it->second.second);
    }
    else if (cache.size() >= capacity) {
        const std::string& lru_key = lru_list.back();
        cache.erase(lru_key);
        lru_list.pop_back();
    }
    lru_list.push_front(key);
    cache[key] = {value, lru_list.begin()};
}

std::string SingleLockLRUCache::readFromCache(const std::string& key)
{
    {
        std::lock_guard<std::mutex> guard(cache_mutex);
        auto it = cache.find(key);
        if (it != cache.end()) {
            lru_list.erase(it->second.second);
            lru_list.push_front(key);
            it->second.second = lru_list.begin();
            return it->second.first;
        }
    }
    std::string value = db->read(key);

    std::lock_guard<std::mutex> guard(cache_mutex);
    if (cache.size() >= capacity) {
        const std::string& lru_key = lru_list.back();
        cache.erase(lru_key);
        lru_list.pop_back();
    }
    lru_list.push_front(key);
    cache[key] = {value, lru_list.begin()};
    return value;
}

void SingleLockLRUCache::removeFromCache(const std::string& key)
{
    std::lock_guard<std::mutex> guard(cache_mutex);
    db->remove(key);
    auto it = cache.find(key);
    if (it != cache.end()) {
        lru_list.erase(it->second.second);
        cache.erase(it);
    }
}