///
/// anax
/// An open source C++ entity system.
///
/// anax
/// An open source C++ entity system.
///
/// Copyright (C) 2013-2014 Miguel Martin (miguel@miguel-martin.com)
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to
/// deal in the Software without restriction, including without limitation the
/// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
/// sell copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
/// IN THE SOFTWARE.
///

#ifndef BABYLON_EXTENSIONS_ENTITY_COMPONENT_SYSTEM_DETAIL_ENTITY_COMPONENT_STORAGE_H
#define BABYLON_EXTENSIONS_ENTITY_COMPONENT_SYSTEM_DETAIL_ENTITY_COMPONENT_STORAGE_H

#include <array>
#include <memory>

#include <babylon/babylon_api.h>

#include <babylon/extensions/entitycomponentsystem/detail/class_type_id.h>
#include <babylon/extensions/entitycomponentsystem/detail/component_type_list.h>

#include <babylon/extensions/entitycomponentsystem/component.h>
#include <babylon/extensions/entitycomponentsystem/config.h>
#include <babylon/extensions/entitycomponentsystem/entity.h>

namespace BABYLON {
namespace Extensions {
namespace ECS {
namespace detail {

/// \brief A class to store components for entities within a world
///
///
/// Used to store the components
///
///
/// \author Miguel Martin
class BABYLON_SHARED_EXPORT EntityComponentStorage {

public:
  explicit EntityComponentStorage(std::size_t entityAmount);

  EntityComponentStorage(const EntityComponentStorage&) = delete;
  EntityComponentStorage(EntityComponentStorage&&)      = delete;
  EntityComponentStorage& operator=(const EntityComponentStorage&) = delete;
  EntityComponentStorage& operator=(EntityComponentStorage&&) = delete;

  void addComponent(Entity& entity, Component* component, TypeId componentTypeId);

  void removeComponent(Entity& entity, TypeId componentTypeId);

  void removeAllComponents(Entity& entity);

  [[nodiscard]] Component& getComponent(const Entity& entity, TypeId componentTypeId) const;

  [[nodiscard]] ComponentTypeList getComponentTypeList(const Entity& entity) const;

  [[nodiscard]] ComponentArray getComponents(const Entity& entity) const;

  [[nodiscard]] bool hasComponent(const Entity& entity, TypeId componentTypeId) const;

  void resize(std::size_t entityAmount);

  void clear();

private:
  typedef std::array<std::unique_ptr<Component>, MAX_AMOUNT_OF_COMPONENTS> ImplComponentArray;

  /// \brief A data structure to describe the components
  /// within an entity
  ///
  /// \author Miguel Martin
  struct EntityComponents {
    EntityComponents() = default;

    // bug in clang? won't work w/o this
    // http://stackoverflow.com/questions/8560994/stdmake-shared-stdunique-ptr-and-move-constructors
    // I think it's due to ComponentTypeList (may have to update boost)
    EntityComponents(EntityComponents&& e)
        : components(std::move(e.components)), componentTypeList(std::move(e.componentTypeList))
    {
    }

    /// The components of an entity. The
    /// index of this array is the same as the TypeId
    /// of the component.
    ImplComponentArray components;

    /// A list of component types, which resembles
    /// what components an entity has
    ComponentTypeList componentTypeList;
  };

  /// All the components for every entity, which has
  /// an entity. The indices of this array is the same
  /// as the index component of an entity's ID.
  std::vector<EntityComponents> m_componentEntries;

  ImplComponentArray& getComponentsImpl(const Entity& e);
  [[nodiscard]] const ImplComponentArray& getComponentsImpl(const Entity& e) const;

}; // end of class EntityComponentStorage

} // end of namespace detail
} // end of namespace ECS
} // end of namespace Extensions
} // end of namespace BABYLON

#endif // BABYLON_EXTENSIONS_ENTITY_COMPONENT_SYSTEM_DETAIL_ENTITY_COMPONENT_STORAGE_H
