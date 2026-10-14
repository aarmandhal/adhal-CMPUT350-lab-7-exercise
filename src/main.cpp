#include <iostream>

#include "entity_manager.h"

int main() {
    // test here...

    // Registry Tests
    Registry<int> r;

    // 1. Testing simple storage and retrieval
    size_t a = r.store(10);
    size_t b = r.store(20);
    size_t c = r.store(30);
    std::cout << a << ' ' << b << ' ' << c << '\n';

    // 2. Testing size
    std::cout << r.size() << '\n';

    r.remove(b);

    // 3. Testing valid()
    std::cout << r.valid(b) << '\n';

    // 4. Testing size()
    std::cout << r.size() << '\n';

    // 5. Testing recycling
    size_t d = r.store(99);
    std::cout << (d == b) << '\n';
    std::cout << r[d] << '\n';

    // Entity Manager Tests
    EntityManager<int, float, bool> entityManager;

    // 1. Testing createEntity() and hasComponent()
    EntityID e = entityManager.createEntity();
    entityManager.addComponent(e, 5);
    std::cout << entityManager.hasComponent<int>(e) << '\n';
    std::cout << entityManager.hasComponent<float>(e) << '\n';
    std::cout << entityManager.hasComponent<bool>(e) << '\n';

    // 2. Testing getComponent()
    std::cout << entityManager.getComponent<int>(e) << '\n';

    return 0;
}