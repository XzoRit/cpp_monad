#include <boost/test/unit_test.hpp>
#include <functional>
#include <mutex>
#include <optional>
#include <utility>

namespace monad
{
    using namespace std;

    template <class A>
    class val
    {
    private:
        A m_func;
        mutable optional<decltype(m_func())> m_value;
        mutable mutex m_lock_value;
    public:
        val(const A& func)
            : m_func{func}
            , m_value{}
            {}
        auto operator()() const
            {
                lock_guard<mutex> _(m_lock_value);
                if(!m_value) m_value = invoke(m_func);

                return *m_value;
            }
    };
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

BOOST_AUTO_TEST_SUITE(vector)

BOOST_FIXTURE_TEST_CASE(monad_val, init)
{
    using namespace monad;
    const auto a = val([](){ return 2374; });

    BOOST_TEST(a() == 2374);
    BOOST_TEST(a() == 2374);
}

BOOST_AUTO_TEST_SUITE_END()
