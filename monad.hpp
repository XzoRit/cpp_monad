#pragma once

#include <functional>
#include <mutex>
#include <optional>
#include <utility>

namespace monad
{
template<class A>
class lazy_value
{
private:
  A m_func;
  mutable std::optional<decltype(m_func())> m_value;
  mutable std::mutex m_lock_value;

public:
  constexpr lazy_value(const A& func) : m_func{ func }, m_value{} {}
  auto operator()() const
  {
    std::lock_guard<std::mutex> _(m_lock_value);
    if (!m_value)
      m_value = std::invoke(m_func);

    return *m_value;
  }
};
namespace io
{
template<class A>
constexpr auto make_action(A&& action);

template<class A>
class action;

template<class A>
class action
{
public:
  constexpr action(const std::function<A()>& act) : m_action{ act } {}
  constexpr A operator()() const { return std::invoke(m_action); }
  template<class C>
  constexpr auto bind(C io_generator) const
  {
    return make_action(
      [*this, io_generator]() { return io_generator(std::invoke(m_action))(); });
  }
  template<class C>
  constexpr auto fmap(C func) const
  {
    return make_action([*this, func]() { return func(std::invoke(m_action)); });
  }
  template<class C>
  constexpr auto operator|(C io_generator) const
  {
    return this->bind(io_generator);
  }
  template<class C>
  constexpr auto operator>(C func) const
  {
    return this->fmap(func);
  }

private:
  std::function<A()> m_action;
};

template<class A>
constexpr auto make_action(A&& act)
{
  return action<decltype(act())>{ std::forward<A>(act) };
}

template<class A>
constexpr auto pure(A val)
{
  return make_action([val]() { return val; });
}
}
}
