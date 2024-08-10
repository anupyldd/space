#pragma once

#if defined(_WIN64) || defined(_WIN32)
    
    #define WINDOWS_PLATFORM 1
    #define PLATFORM_TYPE(win, unix) win    // for when different type is needed on different platforms

#elif defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))

    #define WINDOWS_PLATFORM 0
    #define PLATFORM_TYPE(win, unix) unix    // for when different type is needed on different platforms

#endif