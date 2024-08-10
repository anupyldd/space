#include "Windows.h"
#include <iostream>
#include "core/Types.h"
#include "core/Console.h"
#include "core/Logging.h"
#include "core/Exception.h"
#include <format>

using namespace spc;
int main(int argc, char** argv)
{
    con::Init();

    try
    {
        throw exc::RenderException("wrong");
    }
    catch (const exc::IException& e)
    {
        log::Error(e.What());
    }

    /*
    String s2 = "текст текст";
    log::Debug(s2);
    log::Info(s2);
    log::Warning(s2);
    log::Error(s2);
    */

    //std::cout << "\x1b[31mMagenta\x1b[0m";
    std::cin.get();
}