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

// -- Disable warnings -- //
#ifdef _MSC_VER
// 'identifier' : class 'type' needs to have dll-interface to be used by clients
// of class 'type2'
#pragma warning(disable : 4251)
#endif

#ifndef BABYLON_EXTENSIONS_ENTITY_COMPONENT_SYSTEM_DETAIL_FILTER_H
#define BABYLON_EXTENSIONS_ENTITY_COMPONENT_SYSTEM_DETAIL_FILTER_H

#include <type_traits>

#include <babylon/babylon_api.h>

#include <babylon/extensions/entitycomponentsystem/component.h>
#include <babylon/extensions/entitycomponentsystem/config.h>

#include <babylon/extensions/entitycomponentsystem/detail/class_type_id.h>
#include <babylon/extensions/entitycomponentsystem/detail/component_type_list.h>

namespace BABYLON {
namespace Extensions {
namespace ECS {
namespace detail {

template <class... Args>
struct BABYLON_SHARED_EXPORT TypeList {
};

struct BABYLON_SHARED_EXPORT BaseRequires {
};
struct BABYLON_SHARED_EXPORT BaseExcludes {
};

struct BABYLON_SHARED_EXPORT Filter {
public:
  Filter(ComponentTypeList requirements, ComponentTypeList excludes)
      : m_requires(requirements), m_excludes(excludes)
  {
  }

  [[nodiscard]] bool doesPassFilter(const ComponentTypeList& typeList) const;

private:
  ComponentTypeList m_requires;
  ComponentTypeList m_excludes;
};

template <class... Args>
static ComponentTypeList types(TypeList<Args...> /*typeList*/)
{
  return ComponentTypeList();
}

template <class T, class... Args>
static ComponentTypeList types(detail::TypeList<T, Args...> /*typeList*/)
{
  static_assert(std::is_base_of<Component, T>::value, "Invalid component");
  return ComponentTypeList().set(ComponentTypeId<T>()) | types(detail::TypeList<Args...>());
}

template <class RequireList, class ExcludeList>
Filter MakeFilter()
{
  static_assert(std::is_base_of<BaseRequires, RequireList>::value,
                "RequireList is not a requirement list");
  static_assert(std::is_base_of<BaseExcludes, ExcludeList>::value,
                "ExcludeList is not an excludes list");
  return Filter{types(RequireList{}), types(ExcludeList{})};
}

} // end of namespace detail
} // end of namespace ECS
} // end of namespace Extensions
} // end of namespace BABYLON

#endif // BABYLON_EXTENSIONS_ENTITY_COMPONENT_SYSTEM_DETAIL_FILTER_H
