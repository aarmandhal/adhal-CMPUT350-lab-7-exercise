#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include <array>
#include <cassert>
#include <cstddef>  // size_t
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "pack.h"
#include "registry.h"

using EntityID = size_t;

static constexpr EntityID InvalidEntity = std::numeric_limits<EntityID>::max();

// Entity manager class template
template <typename... ComponentTypes>
class EntityManager {
    // K is the number of component types
    static constexpr EntityID K = sizeof...(ComponentTypes);

    // aliveEntities is a vector of booleans to keep track of alive entities
    std::vector<bool> aliveEntities;
    // freeEntityList is a vector of entity ids to keep track of free entities
    std::vector<EntityID> freeEntityList;
    // componentIndices is a vector of arrays to keep track of component indices
    std::vector<std::array<EntityID, K>> componentIndices;
    // registries is a tuple of registries to keep track of components
    std::tuple<Registry<ComponentTypes>...> registries;

public:
    // createEntity function to create a new entity
    EntityID createEntity() {
        EntityID id;

        // if there are free entities, use one
        if (!freeEntityList.empty()) {
            id = freeEntityList.back();
            freeEntityList.pop_back();
            aliveEntities[id] = true;
            componentIndices[id].fill(InvalidEntity);
            // if there are no free entities, create a new one
        } else {
            id = aliveEntities.size();
            aliveEntities.push_back(true);
            std::array<EntityID, K> row;
            row.fill(InvalidEntity);
            componentIndices.push_back(row);
        }
        // return the new entity id
        return id;
    }

    // validEntity function to check if an entity is valid
    bool validEntity(EntityID id) const { return id < aliveEntities.size() && aliveEntities[id]; }

    // ComponentTypeIndex function to get the index of a component type
    template <typename ComponentType>
    static constexpr size_t ComponentTypeIndex =
        Pack<ComponentTypes...>::template index<ComponentType>;

    // hasComponent function to check if an entity has a component
    template <typename ComponentType>
    bool hasComponent(EntityID id) const {
        // assert that the component type is valid
        static_assert(Pack<ComponentTypes...>::template contains<ComponentType>);
        // assert that the entity is valid
        assert(validEntity(id));
        // get the index of the component type
        constexpr size_t idx = ComponentTypeIndex<ComponentType>;
        // return true if the component exists
        return componentIndices[id][idx] != InvalidEntity;
    }

    // getComponent function to get a component from an entity
    template <typename ComponentType>
    ComponentType& getComponent(EntityID id) {
        // assert that the component type is valid
        static_assert(Pack<ComponentTypes...>::template contains<ComponentType>);
        // assert that the entity has the component
        assert(hasComponent<ComponentType>(id));
        // get the index of the component type
        constexpr size_t idx = ComponentTypeIndex<ComponentType>;
        // get the ticket for the component
        size_t ticket = componentIndices[id][idx];
        // return the component
        return std::get<idx>(registries)[ticket];
    }

    // addComponent function to add a component to an entity
    template <typename ComponentType>
    void addComponent(EntityID id, ComponentType&& component) {
        // remove reference from the component type
        using Clean = std::remove_reference_t<ComponentType>;
        // assert that the component type is valid
        static_assert(Pack<ComponentTypes...>::template contains<Clean>);
        // assert that the entity is valid
        assert(validEntity(id));
        // assert that the entity does not have the component
        assert(!hasComponent<Clean>(id));

        // get the index of the component type
        constexpr size_t idx = ComponentTypeIndex<Clean>;
        // store the component in the registry
        size_t ticket = std::get<idx>(registries).store(std::forward<ComponentType>(component));
        // store the ticket in the component indices
        componentIndices[id][idx] = ticket;
    }
};

#endif  // ENTITY_MANAGER_H