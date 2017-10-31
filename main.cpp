#include "monad.hpp"
#include <iostream>

using namespace std;
using namespace monad;

const auto putCout = [](const auto& s)
{
    return make_io_action([s](){ cout << s; return monostate{}; });
};

const auto getCout = []()
{
    string s{};
    getline(cin, s);
    return s;
};

int main()
{
    putCout("Hello\n")();
    return 0;
}
