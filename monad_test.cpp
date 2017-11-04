#include "monad.hpp"
#include <boost/test/unit_test.hpp>
#include <variant>
#include <iostream>
#include <cctype>

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

std::string str_toupper(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::toupper(c); }
        );
    return s;
}

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

BOOST_FIXTURE_TEST_CASE(monad_io_bind, init)
{
    const auto a =
        make_io_action([](){ return 1; })
        .bind([](const auto& a){ return make_io_action([a](){ return a + 2; }); })
        .bind([](const auto& a){ return make_io_action([a](){ return a + 3; }); })
        ;

    BOOST_TEST(a() == 6);

    const auto b =
        make_io_action([](){ return string{"1"}; })
        | [](const auto& a){ return make_io_action([a](){ return a + "2"; }); }
        | [](const auto& a){ return make_io_action([a](){ return a + "3"; }); }
        ;

    BOOST_TEST(b() == "123");
}

BOOST_FIXTURE_TEST_CASE(monad_io_fmap, init)
{
    const auto a =
        make_io_action([](){ return string{"a"}; })
        .fmap([](const auto& a){ return  a + "b"; })
        .fmap([](const auto& a){ return  a + "c"; })
        .fmap([](const auto& a){ return str_toupper(a); })
        ;

    BOOST_TEST(a() == "ABC");

    const auto b =
        make_io_action([](){ return string{"a"}; })
        > [](const auto& a){ return  a + "b"; }
        > [](const auto& a){ return  a + "c"; }
        > [](const auto& a){ return str_toupper(a); }
        ;

    BOOST_TEST(b() == "ABC");
}

BOOST_AUTO_TEST_SUITE_END()
