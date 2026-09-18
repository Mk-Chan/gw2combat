#ifndef GW2COMBAT_MRU_CACHE_HPP
#define GW2COMBAT_MRU_CACHE_HPP

#include <list>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace gw2combat {

template <typename T>
struct mru_cache_t {
    // Retain equality as well as hashing. Hash-only keys can silently resume a
    // different encounter (unsigned long is only 32 bits on Windows).
    using key_type = std::string;

    [[nodiscard]] static mru_cache_t<T>& instance() {
        static mru_cache_t<T> instance(4096);
        return instance;
    }

    [[nodiscard]] bool enabled() const {
        const std::lock_guard lock(mutex);
        return capacity != 0;
    }

    [[nodiscard]] bool contains(const key_type& key) const {
        const std::lock_guard lock(mutex);
        return cache.find(key) != cache.end();
    }

    // Keep the immutable snapshot alive while callers copy it, even if another
    // request replaces or evicts the cache entry after this lock is released.
    [[nodiscard]] std::shared_ptr<const T> get(const key_type& key) {
        const std::lock_guard lock(mutex);
        auto item = cache.find(key);
        if (item == cache.end()) {
            return nullptr;
        }
        mru_list.splice(mru_list.begin(), mru_list, item->second.second);
        item->second.second = mru_list.begin();
        return item->second.first;
    }

    void put(const key_type& key, T&& value) {
        const std::lock_guard lock(mutex);
        if (capacity == 0) {
            return;
        }
        auto snapshot = std::make_shared<const T>(std::move(value));
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
        cache[key] = {std::move(snapshot), mru_list.begin()};
    }

    void resize(int desired_size_in_MiB, int average_registry_size_in_MiB = 64) {
        if (average_registry_size_in_MiB <= 0) {
            throw std::invalid_argument("average registry size must be positive");
        }
        const std::lock_guard lock(mutex);
        capacity = desired_size_in_MiB > 0
                       ? desired_size_in_MiB / average_registry_size_in_MiB
                       : 0;
        while (cache.size() > capacity) {
            cache.erase(mru_list.back());
            mru_list.pop_back();
        }
    }

   protected:
    explicit mru_cache_t(int desired_size_in_MiB, int average_registry_size_in_MiB = 64) {
        resize(desired_size_in_MiB, average_registry_size_in_MiB);
    }

   private:
    mutable std::mutex mutex;
    size_t capacity = 0;
    std::list<key_type> mru_list;
    std::unordered_map<key_type,
                       std::pair<std::shared_ptr<const T>, std::list<key_type>::iterator>> cache;
};

}  // namespace gw2combat

#endif  // GW2COMBAT_MRU_CACHE_HPP
