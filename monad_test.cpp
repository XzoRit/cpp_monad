#include <boost/test/unit_test.hpp>
#include <functional>
#include <mutex>
#include <optional>
#include <utility>
#include <variant>
#include <iostream>
#include <string>

namespace std
{
    ostream& boost_test_print_type(ostream& ostr, monostate const& right)
    {
        ostr << "monostate";
        return ostr;
    }
}

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

struct init
{
    init()
    {
        BOOST_TEST_MESSAGE("init");
    }
    ~init()
    {
        BOOST_TEST_MESSAGE("~init");
    }
};

using namespace monad;

const auto putCout = [](const auto& s)
{
    return make_io_action([s](){ cout << s; return monostate{}; });
};

BOOST_AUTO_TEST_SUITE(monad)

BOOST_FIXTURE_TEST_CASE(monad_lazy_value, init)
{
    const auto a = lazy_value{[](){ return 2374; }};

    BOOST_TEST(a() == 2374);
    BOOST_TEST(a() == 2374);
}

BOOST_FIXTURE_TEST_CASE(monad_io_monostate, init)
{
    const auto a = make_io_action([](){ cout << "a"; return monostate{}; });

    BOOST_TEST(a() == monostate{});

    BOOST_TEST(putCout("b")() == monostate{});
    BOOST_TEST(putCout( 1 )() == monostate{});
}

BOOST_AUTO_TEST_SUITE_END()
