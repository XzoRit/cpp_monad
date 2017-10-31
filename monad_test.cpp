#include <boost/test/unit_test.hpp>
#include <vector>

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

BOOST_FIXTURE_TEST_CASE(size_and_capacity_after_init, init)
{
    BOOST_TEST(true == true);
}

BOOST_AUTO_TEST_SUITE_END()
