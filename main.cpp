#include "monad.hpp"
#include <iostream>

using namespace std;
using namespace monad;

const auto putCout = [](const auto& s)
{
    return make_io_action(
        [s](){ cout << s; return monostate{}; });
};

const auto getCout = [](const auto&)
{
    return make_io_action(
        [](){ string s{}; getline(cin, s); return s; });
};

auto test()
{
    return putCout("your name please: ")
        | getCout
        | [](const auto& a){ return putCout(string{"Hello "} + a + "!\n"); }
        ;
}

int main()
{
    test()();
    return 0;
}
