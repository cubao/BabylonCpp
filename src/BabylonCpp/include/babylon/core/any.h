#ifndef BABYLON_CORE_ANY_H
#define BABYLON_CORE_ANY_H

#include <algorithm>
#include <iostream>
#include <string>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <vector>

namespace BABYLON {

//------------------------------------------------------------------------------

struct alloc {
  template <typename D, typename V>
  D* copy(V&& v)
  {
    return new D{std::forward<V>(v)};
  }

  template <typename D, typename V, typename B>
  B* move(V&& /*v*/, B*& p)
  {
    B* q = p;
    p    = nullptr;
    return q;
  }

  template <typename D>
  void free(D* p)
  {
    delete p;
  }
};

//------------------------------------------------------------------------------

template <size_t N = 16>
class store {
  char space[N];

  template <typename T>
  static constexpr bool fits()
  {
    return sizeof(typename std::decay<T>::type) <= N;
  }

public:
  template <typename D, typename V>
  D* copy(V&& v)
  {
#if !defined(__clang__)
#if __GNUC__ > 5
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wplacement-new="
#endif
#endif
    return fits<D>() ? new (space) D{std::forward<V>(v)} : new D{std::forward<V>(v)};
#if !defined(__clang__)
#if __GNUC__ > 5
#pragma GCC diagnostic pop
#endif
#endif
  }

  template <typename D, typename V, typename B>
  B* move(V&& v, B*& p)
  {
    B* q = fits<D>() ? copy<D>(std::forward<V>(v)) : p;
    p    = nullptr;
    return q;
  }

  template <typename D>
  void free(D* p)
  {
    fits<D>() ? p->~D() : delete p;
  }
};

//------------------------------------------------------------------------------

template <typename A = store<>>
class some : A {
  using id = size_t;

  template <typename T>
  struct type {
    static void id()
    {
    }
  };

  template <typename T>
  static id type_id()
  {
    return reinterpret_cast<id>(&type<T>::id);
  }

  //----------------------------------------------------------------------------

  template <typename T>
  using decay = typename std::decay<T>::type;

  template <typename T>
  using none = typename std::enable_if<!std::is_same<some, T>::value>::type;

  //----------------------------------------------------------------------------

  struct base {
    virtual ~base()                         = default;
    [[nodiscard]] virtual bool is(id) const = 0;
    virtual base* copy(A&) const            = 0;
    virtual base* move(A&, base*&)          = 0;
    virtual void free(A&)                   = 0;
  }* p = nullptr;

  //----------------------------------------------------------------------------

  template <typename T>
  struct data : base, std::tuple<T> {
    using std::tuple<T>::tuple;

    T& get() &
    {
      return std::get<0>(*this);
    }
    [[nodiscard]] T const& get() const&
    {
      return std::get<0>(*this);
    }

    [[nodiscard]] bool is(id i) const override
    {
      return i == type_id<T>();
    }

    base* copy(A& a) const override
    {
      return a.template copy<data>(get());
    }

    base* move(A& a, base*& p2) override
    {
      return a.template move<data>(std::move(get()), p2);
    }

    void free(A& a) override
    {
      a.free(this);
    }
  };

  //----------------------------------------------------------------------------

  template <typename T>
  T& stat()
  {
    return static_cast<data<T>*>(p)->get();
  }

  template <typename T>
  [[nodiscard]] T const& stat() const
  {
    return static_cast<data<T> const*>(p)->get();
  }

  template <typename T>
  T& dyn()
  {
    return dynamic_cast<data<T>&>(*p).get();
  }

  template <typename T>
  [[nodiscard]] T const& dyn() const
  {
    return dynamic_cast<data<T> const&>(*p).get();
  }

  //----------------------------------------------------------------------------

  base* move(some& s)
  {
    return s.p->move(*this, s.p);
  }
  base* copy(some const& s)
  {
    return s.p->copy(*this);
  }

  base* read(some&& s)
  {
    return s.p ? move(s) : s.p;
  }
  base* read(some const& s)
  {
    return s.p ? copy(s) : s.p;
  }

  template <typename V, typename U = decay<V>, typename = none<U>>
  base* read(V&& v)
  {
    return A::template copy<data<U>>(std::forward<V>(v));
  }

  //----------------------------------------------------------------------------

  template <typename X>
  some& assign(X&& x)
  {
    if (!p)
      p = read(std::forward<X>(x));
    else {
      some t{std::move(*this)};
      try {
        p = read(std::forward<X>(x));
      }
      catch (...) {
        p = move(t);
        throw;
      }
    }
    return *this;
  }

  void swap(some& s)
  {
    if (!p)
      p = read(std::move(s));
    else if (!s.p)
      s.p = move(*this);
    else {
      some t{std::move(*this)};
      try {
        p = move(s);
      }
      catch (...) {
        p = move(t);
        throw;
      }
      s.p = move(t);
    }
  }

  //-----------------------------------------------------------------------------

public:
  some() = default;
  ~some()
  {
    if (p)
      p->free(*this);
  }

  some(some&& s) : p{read(std::move(s))}
  {
  }
  some(some const& s) : p{read(s)}
  {
  }

  template <typename V, typename = none<decay<V>>>
  some(V&& v) : p{read(std::forward<V>(v))}
  {
  }

  some& operator=(some&& s)
  {
    return assign(std::move(s)); // NOLINT
  }
  some& operator=(some const& s) // NOLINT
  {
    return assign(s); // NOLINT
  }

  template <typename V, typename = none<decay<V>>>
  some& operator=(V&& v)
  {
    return assign(std::forward<V>(v)); // NOLINT
  }

  friend void swap(some& s, some& r)
  {
    s.swap(r);
  }

  void clear()
  {
    if (p) {
      p->free(*this);
      p = nullptr;
    }
  }

  [[nodiscard]] bool empty() const
  {
    return p;
  }

  template <typename T>
  [[nodiscard]] bool is() const
  {
    return p ? p->is(type_id<T>()) : false;
  }

  template <typename T>
  T&& _() &&
  {
    return std::move(stat<T>());
  }
  template <typename T>
  T& _() &
  {
    return stat<T>();
  }
  template <typename T>
  [[nodiscard]] T const& _() const&
  {
    return stat<T>();
  }

  template <typename T>
  T&& cast() &&
  {
    return std::move(dyn<T>());
  }
  template <typename T>
  T& cast() &
  {
    return dyn<T>();
  }
  template <typename T>
  [[nodiscard]] T const& cast() const&
  {
    return dyn<T>();
  }

  template <typename T>
  operator T &&() &&
  {
    return std::move(_<T>());
  }
  template <typename T>
  operator T&() &
  {
    return _<T>();
  }
  template <typename T>
  operator T const&() const&
  {
    return _<T>();
  }
};

//------------------------------------------------------------------------------

using any = some<>;

//------------------------------------------------------------------------------

template <typename S, typename T>
S& operator<<(S& s, const std::vector<T>& v)
{
  for (auto& x : v)
    s << x << " ";
  return s;
}

} // end of namespace BABYLON

#endif // end of BABYLON_CORE_ANY_H
