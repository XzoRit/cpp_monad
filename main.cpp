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

auto ask(int i)
{
    return putCout("is less then? ")
        | [i](const auto&){ return putCout(i); }
        | [](const auto&){ return putCout("(y/n) ?"); }
        | getCout
        | [](const auto& a){ return pure(a == "y"); }
        ;
}

io<int> guess(int a, int b)
{
    if(a >= b) return pure(a);
    int m = (a + b + 1) / 2;
    return ask(m)
        | [a, m, b](const auto& yes)
          {
              if(yes) return guess(a, m - 1);
              else return guess(m, b);
          }
        ;
}

int main()
{
    {
        const auto a{guess(0, 10)};
        a();
    }
    {
        const auto a{test()};
        a();
    }
    return 0;
}
