#ifndef REGISTRY_H
#define REGISTRY_H

#include <cassert>
#include <cstddef>
#include <utility>
#include <vector>

template <typename T>
class Registry {
    // slots is a vector of components
    std::vector<T> slots;
    // alive is a vector of booleans to keep track of alive components
    std::vector<bool> alive;
    // free_list is a vector of size_t to keep track of free indices
    std::vector<size_t> free_list;
    // active_count is the number of active components
    size_t active_count = 0;

public:
    // store function to store a component
    template <typename U>
    size_t store(U&& value) {
        size_t id;
        // if there are free indices, use one
        if (!free_list.empty()) {
            id = free_list.back();
            free_list.pop_back();
            slots[id] = std::forward<U>(value);
            alive[id] = true;

            // if there are no free indices, create a new one
        } else {
            id = slots.size();
            slots.emplace_back(std::forward<U>(value));
            alive.push_back(true);
        }
        // increment active count
        ++active_count;
        // return the id
        return id;
    }

    // remove function to remove a component
    void remove(size_t id) {
        // assert that the id is valid
        assert(valid(id));
        // reset the component
        slots[id] = T{};
        // mark the component as not alive
        alive[id] = false;
        // add the id to the free list
        free_list.push_back(id);
        // decrement active count
        --active_count;
    }

    // operator[] function to access a component
    T& operator[](size_t id) { return slots[id]; }

    // operator[] function to access a component
    const T& operator[](size_t id) const { return slots[id]; }

    // valid function to check if an id is valid
    bool valid(size_t id) const { return id < alive.size() && alive[id]; }

    // size function to get the number of active components
    size_t size() const { return active_count; }

    // reserve function to reserve space for components
    void reserve(size_t capacity) {
        slots.reserve(capacity);
        alive.reserve(capacity);
    }
};

#endif  // REGISTRY_H
