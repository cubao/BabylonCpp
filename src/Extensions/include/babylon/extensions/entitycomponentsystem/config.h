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

#ifndef BABYLON_EXTENSIONS_ENTITY_COMPONENT_SYSTEM_CONFIG_H
#define BABYLON_EXTENSIONS_ENTITY_COMPONENT_SYSTEM_CONFIG_H

#include <cstddef>

#define ANAX_VERSION_MAJOR 2
#define ANAX_VERSION_MINOR 1
#define ANAX_VERSION_PATCH 0

#if defined(_WIN32) && !defined(_WIN64)
#define ANAX_32_BIT_ENTITY_IDS
#endif
#ifdef __EMSCRIPTEN__
#define ANAX_32_BIT_ENTITY_IDS
#endif

#if defined(ANAX_32_BIT_ENTITY_IDS)
#define ANAX_ENTITY_ID_INDEX_BIT_COUNT 20
#define ANAX_ENTITY_ID_COUNTER_BIT_COUNT 12
#else
#define ANAX_ENTITY_ID_INDEX_BIT_COUNT 48
#define ANAX_ENTITY_ID_COUNTER_BIT_COUNT 16
#endif

namespace BABYLON {
namespace Extensions {
namespace ECS {

/// The default size of a pool within a world
constexpr const std::size_t DEFAULT_ENTITY_POOL_SIZE = 1000;

/// The maximum amount of components an entity can
/// contain. Try to make this number even, or preferably
/// a power of 2.
constexpr const std::size_t MAX_AMOUNT_OF_COMPONENTS = 64;

} // end of namespace ECS
} // end of namespace Extensions
} // end of namespace BABYLON

#endif // BABYLON_EXTENSIONS_ENTITY_COMPONENT_SYSTEM_CONFIG_H
