#include <iostream>
#include "core/Types.h"
#include "core/Console.h"
#include "core/Logging.h"
#include "core/Exception.h"
#include "engine/Engine.h"
#include <format>
#include "engine/Localization.h"
#include "core/Fsm.h"
#include "core/Ltf.h"
#include "core/Config.h"
#include "core/Math.h"

using namespace file;
using namespace eng::loc;
int main(int argc, char** argv)
{
    conf::Init();
    
    try
    {
        
    }
    catch (const exc::IException& e)
    {
        lg::Error(e.What());
    }
    catch (const std::exception& e2)
    {
        lg::Error(e2.what());
    }
    
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