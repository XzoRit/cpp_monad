#include "monad.hpp"
#include <cctype>
#include <iostream>
#include <string>
#include <variant>

using namespace std;

const auto putCout = [](const auto& s) {
  return monad::io::make_action([s]() {
    cout << s;
    return monostate{};
  });
};

auto getCout(const monostate&)
{
  return monad::io::make_action([]() {
    string s{};
    getline(cin, s);
    return s;
  });
}

string str_tolower(string a)
{
  transform(
    a.begin(), a.end(), a.begin(), [](unsigned char c) { return tolower(c); });
  return a;
}

auto your_name_please()
{
  return (putCout("your name please: ") | getCout) > str_tolower >
    [](const auto& a) { return string{ "\"" } + a + '\"'; } |
    [](const auto& a) { return putCout(string{ "Hello " } + a + "!\n"); };
}

auto ask(int i)
{
  return putCout("is less then? ") | [i](const auto&) { return putCout(i); } |
    [](const auto&) { return putCout("(y/n) ?"); } | getCout |
    [](const auto& a) { return monad::io::pure(a == "y"); };
}

monad::io::action<int> guess(int a, int b)
{
  if (a >= b)
    return monad::io::pure(a);
  int m = (a + b + 1) / 2;
  return ask(m) | [a, m, b](const auto& yes) {
    if (yes)
      return guess(a, m - 1);
    else
      return guess(m, b);
  };
}

int main()
{
  {
    const auto a{ guess(0, 10) };
    a();
  }
  {
    const auto a{ your_name_please() };
    a();
  }
  return 0;
}
