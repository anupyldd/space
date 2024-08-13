#include <iostream>
#include "core/Types.h"
#include "core/Console.h"
#include "core/Logging.h"
#include "core/Exception.h"
#include "engine/Engine.h"
#include <format>

using namespace file;
int main(int argc, char** argv)
{
    con::Init();

    try
    {
        File fl;
        lg::Info(fl.Read("res/test.txtloc"));
    }
    catch (const exc::IException& e)
    {
        lg::Error(e.What());
    }
    catch (...)
    {
        lg::Error("something went wrong");
    }
    con::PrintN("тест юникода");
    /*
    String s2 = "текст текст";
    lg::Debug(s2);
    lg::Info(s2);
    lg::Warning(s2);
    lg::Error(s2);
    */

    //std::cout << "\x1b[31mMagenta\x1b[0m";
    std::cin.get();
}