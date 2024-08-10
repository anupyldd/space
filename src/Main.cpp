#include "Windows.h"
#include <iostream>
#include "core/Types.h"
#include "core/Console.h"
#include "core/Logging.h"
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