#include "Windows.h"
#include <iostream>
#include "base/Types.h"
#include "base/Console.h"
#include <format>

using namespace spc;
int main(int argc, char** argv)
{
    con::Init();
    std::cout << "\x1b[95mMagenta\x1b[0m";
    std::cin.get();
}