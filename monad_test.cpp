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

string str_toupper(string a)
{
    transform(
        a.begin(), a.end(),
        a.begin(),
        [](unsigned char c)
        { return toupper(c); }
        );
    return a;
}

BOOST_AUTO_TEST_SUITE(monad)

BOOST_FIXTURE_TEST_CASE(monad_lazy_value, init)
{
    using namespace monad;

    const auto a = lazy_value{[](){ return 2374; }};

    BOOST_TEST(a() == 2374);
    BOOST_TEST(a() == 2374);
}

BOOST_FIXTURE_TEST_CASE(monad_io_monostate, init)
{
    const auto a = monad::io::make_action([](){ return monostate{}; });

    BOOST_TEST(a() == monostate{});
}

BOOST_FIXTURE_TEST_CASE(monad_io_bind, init)
{
    const auto a =
        monad::io::make_action([](){ return 1; })
        .bind([](const auto& a){ return monad::io::make_action([a](){ return a + 2; }); })
        .bind([](const auto& a){ return monad::io::make_action([a](){ return a + 3; }); })
        ;

    BOOST_TEST(a() == 6);

    const auto b =
        monad::io::make_action([](){ return string{"1"}; })
        | [](const auto& a){ return monad::io::make_action([a](){ return a + "2"; }); }
        | [](const auto& a){ return monad::io::make_action([a](){ return a + "3"; }); }
        ;

    BOOST_TEST(b() == "123");
}

BOOST_FIXTURE_TEST_CASE(monad_io_fmap, init)
{
    const auto a =
        monad::io::make_action([](){ return string{"a"}; })
        .fmap([](const auto& a){ return  a + "b"; })
        .fmap([](const auto& a){ return  a + "c"; })
        .fmap(str_toupper)
        ;

    BOOST_TEST(a() == "ABC");

    const auto b =
        monad::io::make_action([](){ return string{"a"}; })
        > [](const auto& a){ return  a + "b"; }
        > [](const auto& a){ return  a + "c"; }
        > str_toupper
        ;

    BOOST_TEST(b() == "ABC");
}

BOOST_FIXTURE_TEST_CASE(monad_io_pure, init)
{
    const auto a = monad::io::pure(string{"abc"});

    BOOST_TEST(a() == "abc");
}

BOOST_AUTO_TEST_SUITE_END()
