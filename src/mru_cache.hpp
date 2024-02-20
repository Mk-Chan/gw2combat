#ifndef GW2COMBAT_MRU_CACHE_HPP
#define GW2COMBAT_MRU_CACHE_HPP

#include <list>
#include <string>
#include <unordered_map>

namespace gw2combat {

template <typename T>
struct mru_cache_t {
    using key_type = unsigned long;

    [[nodiscard]] static mru_cache_t<T>& instance() {
        static mru_cache_t<T> instance(4096);
        return instance;
    }

    [[nodiscard]] static key_type djb2_hash(const std::string& str) {
        unsigned long hash = 5381;
        for (auto& c : str) {
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
        }
        return static_cast<key_type>(hash);
    }

    [[nodiscard]] bool contains(key_type key) const {
        return cache.find(key) != cache.end();
    }

    [[nodiscard]] T& get(key_type key) {
        auto item = cache.find(key);
        mru_list.erase(item->second.second);
        mru_list.push_front(key);
        item->second.second = mru_list.begin();
        return item->second.first;
    }

    T& put(key_type key, T&& value) {
        auto item = cache.find(key);
        if (item != cache.end()) {
            mru_list.erase(item->second.second);
            cache.erase(item);
        } else {
            while (cache.size() >= capacity) {
                cache.erase(mru_list.back());
                mru_list.pop_back();
            }
        }

        mru_list.push_front(key);
        cache[key] = {std::move(value), mru_list.begin()};
        return cache[key].first;
    }

    void resize(int desired_size_in_MiB, int average_registry_size_in_MiB = 64.0) {
        capacity = desired_size_in_MiB / average_registry_size_in_MiB;
    }

   protected:
    explicit mru_cache_t(int desired_size_in_MiB, int average_registry_size_in_MiB = 64.0)
        : capacity(desired_size_in_MiB / average_registry_size_in_MiB) {
    }

   private:
    size_t capacity;
    std::list<key_type> mru_list;
    std::unordered_map<key_type, std::pair<T, std::list<key_type>::iterator>> cache;
};

}  // namespace gw2combat

#endif  // GW2COMBAT_MRU_CACHE_HPP
