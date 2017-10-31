#include "monad.hpp"
#include <boost/test/unit_test.hpp>
#include <variant>
#include <iostream>

namespace std
{
    ostream& boost_test_print_type(ostream& ostr, monostate const& right)
    {
        ostr << "monostate";
        return ostr;
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

using namespace std;
using namespace monad;

BOOST_AUTO_TEST_SUITE(monad)

BOOST_FIXTURE_TEST_CASE(monad_lazy_value, init)
{
    const auto a = lazy_value{[](){ return 2374; }};

    BOOST_TEST(a() == 2374);
    BOOST_TEST(a() == 2374);
}

BOOST_FIXTURE_TEST_CASE(monad_io_monostate, init)
{
    const auto a = make_io_action([](){ return monostate{}; });

    BOOST_TEST(a() == monostate{});
}

BOOST_AUTO_TEST_SUITE_END()
