#pragma once

#include "Console.h"
#include "Language.h"

namespace conf
{
    inline void Init()
    {
        con::Init();
        lang::LoadLanguageMap(lang::langCodesSL);
    }
}