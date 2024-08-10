#include "Windows.h"
#include <iostream>
#include "base/Types.h"
#include "base/Console.h"
#include "base/Logging.h"
#include <format>

using namespace spc;
int main(int argc, char** argv)
{
    con::Init();

    String s = "привет";
    log::Debug(s);
    String s2 = con::SetStringColor(s, con::BLUE, con::BRIGHT_GREEN);
    log::Debug(s2);

    //std::cout << "\x1b[31mMagenta\x1b[0m";
    std::cin.get();
}