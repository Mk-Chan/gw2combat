#ifndef GW2COMBAT_ITERABLE_PRIORITY_QUEUE_HPP
#define GW2COMBAT_ITERABLE_PRIORITY_QUEUE_HPP

#include <queue>

namespace gw2combat {

template <class T>
class iterable_priority_queue
    : public std::priority_queue<T, std::vector<T>, decltype(&T::compare_by_remaining_duration)> {
    using super =
        std::priority_queue<T, std::vector<T>, decltype(&T::compare_by_remaining_duration)>;

   public:
    iterable_priority_queue() : super(T::compare_by_remaining_duration) {
    }

    using iterator = typename super::container_type::iterator;
    using const_iterator = typename super::container_type::const_iterator;

    [[maybe_unused]] [[nodiscard]] iterator begin() {
        return super::c.begin();
    }
    [[maybe_unused]] [[nodiscard]] iterator end() {
        return super::c.end();
    }
    [[maybe_unused]] [[nodiscard]] const_iterator cbegin() const {
        return super::c.cbegin();
    }
    [[maybe_unused]] [[nodiscard]] const_iterator cend() const {
        return super::c.cend();
    }
};

}  // namespace gw2combat

#endif  // GW2COMBAT_ITERABLE_PRIORITY_QUEUE_HPP
