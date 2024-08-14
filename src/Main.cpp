#include <iostream>
#include "core/Types.h"
#include "core/Console.h"
#include "core/Logging.h"
#include "core/Exception.h"
#include "engine/Engine.h"
#include <format>
#include "engine/Localization.h"
#include "core/Fsm.h"

using namespace file;
using namespace eng::loc;
int main(int argc, char** argv)
{
    con::Init();
    /*
    Localization loc;
    loc.LoadFiles({ "res/test.txtloc", "res/test2.txtloc" });
    lg::Info(std::to_string(loc.GetLoadedFilesNum()));
    lg::Output(loc.GetFileContents("test2.txtloc"));
    */


    /*
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
    */
    //con::PrintN("юникод");
    std::string s = "т";
    con::Print(s);
    //std::wstring str = con::Utf8ToUtf16(s);
    /*
    unsigned short* vtemp = (unsigned short*)str.c_str();
    for (int i = 0; i < str.length(); ++i)
    {
        std::wcout << (unsigned short)((unsigned char)vtemp[i]) << " ";
    }
    */
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