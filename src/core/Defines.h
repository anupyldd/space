#pragma once

#if defined(_WIN64) || defined(_WIN32)
    
    #define WINDOWS_PLATFORM 1

#elif defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))

    #define WINDOWS_PLATFORM 0

#endif