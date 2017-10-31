#pragma once

#include <mutex>
#include <optional>
#include <utility>
#include <variant>
#include <functional>

using namespace std;

namespace monad
{
    template <class A>
    class lazy_value
    {
    private:
        A m_func;
        mutable optional<decltype(m_func())> m_value;
        mutable mutex m_lock_value;
    public:
        constexpr lazy_value(const A& func)
            : m_func{func}
            , m_value{}
            {}
        constexpr auto operator()() const
            {
                lock_guard<mutex> _(m_lock_value);
                if(!m_value) m_value = invoke(m_func);

                return *m_value;
            }
    };

    template<class A, class B>
    class io
    {
    public:
        constexpr io(const B& action)
            : m_action{action}
            {}
        constexpr A operator()() const
            {
                return invoke(m_action);
            }
    private:
        B m_action;
    };

    template<class A>
    constexpr auto make_io_action(A&& action)
    {
        return io<decltype(action()), A>{forward<A>(action)};
    }
}

