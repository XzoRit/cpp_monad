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

    template<class A>
    constexpr auto make_io_action(A&& action);

    template<class A>
    class io;

    template<class A>
    class io
    {
    public:
        constexpr io(const function<A()>& action)
            : m_action{action}
            {}
        constexpr A operator()() const
            {
                return invoke(m_action);
            }
        template<class C>
        constexpr auto bind(C io_generator) const
            {
                return make_io_action(
                    [*this, io_generator]()
                    {
                        return io_generator(invoke(m_action))();
                    });
            }
        template<class C>
        constexpr auto fmap(C func) const
            {
                return make_io_action(
                    [*this, func]()
                    {
                        return func(invoke(m_action));
                    });
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
        function<A()> m_action;
    };

    template<class A>
    constexpr auto make_io_action(A&& action)
    {
        return io<decltype(action())>{forward<A>(action)};
    }

    template<class A>
    constexpr auto pure(A val)
    {
        return make_io_action([val](){ return val; });
    }
}

