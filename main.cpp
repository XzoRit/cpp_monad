#include "monad.hpp"
#include <iostream>
#include <string>
#include <cctype>
#include <variant>

using namespace std;
using namespace monad;

const auto putCout = [](const auto& s)
{
    return make_io_action(
        [s](){ cout << s; return monostate{}; });
};

auto getCout(const monostate&)
{
    return make_io_action(
        []()
        {
            string s{};
            getline(cin, s);
            return s;
        });
}

string str_tolower(string a)
{
    transform(
        a.begin(), a.end(),
        a.begin(),
        [](unsigned char c)
        { return tolower(c); }
        );
    return a;
}

auto test()
{
    return
        (putCout("your name please: ") | getCout)
        > str_tolower
        > [](const auto& a){ return string{"\""} + a + '\"'; }
        | [](const auto& a){ return putCout(string{"Hello "} + a + "!\n"); }
        ;
}

int main()
{
    const auto a{test()};
    a();
    return 0;
}
