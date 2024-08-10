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

    String s2 = "текст текст";
    log::Debug(s2);
    log::Info(s2);
    log::Warning(s2);
    log::Error(s2);

    //std::cout << "\x1b[31mMagenta\x1b[0m";
    std::cin.get();
}