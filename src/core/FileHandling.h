#pragma once

#if defined(_WIN64) || defined(_WIN32)

#include "WinDef.h"

#else

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

#endif

#include "Logging.h"
#include "Defines.h"
#include "Exception.h"

#include <filesystem>

// API ---------------------------------

namespace spc
{
    namespace file
    {
        struct File;    // common struct for both windows and posix files
        enum class FileMode { READ, WRITE, READ_WRITE };
        
        inline bool    Open(const std::filesystem::path& path, FileMode mode, File& file); // opens into "file" object
        inline bool    Map(File& file);
        inline void    CleanUp(File& file);
        inline String  GetContent(const File& file);
    }
}

// -------------------------------------

namespace spc
{
    namespace file
    {
        // internal functions
        namespace itrn
        {
            inline DWORD GetFileModeWindows(FileMode mode)
            {
                switch (mode)
                {
                case FileMode::READ: return GENERIC_READ;
                case FileMode::WRITE: return GENERIC_WRITE;
                case FileMode::READ_WRITE: return GENERIC_READ | GENERIC_WRITE;
                default: throw exc::CoreException("Unknown file access mode");
                }
            }

            inline int GetFileModePosix(FileMode mode)
            {
                switch (mode)
                {
                case FileMode::READ: return O_RDONLY;
                case FileMode::WRITE: return O_WRONLY;
                case FileMode::READ_WRITE: return O_RDWR;
                default: throw exc::CoreException("Unknown file access mode");
                }
            }
        }

        struct File
        {
            bool valid = false; // sets to true when successfully mapped

            PLATFORM_TYPE(HANDLE, int)          handle;
            PLATFORM_TYPE(DWORD,  struct stat)  stat;
            PLATFORM_TYPE(LPVOID, char*)        map;
            HANDLE                              mapping;      // only used on windows, since there's a 2-step mapping process there
        };
    
        inline bool Open(const std::filesystem::path& path, FileMode mode, File& file) 
        {
        // Windows -----------------------
        #if WINDOWS_PLATFORM

            file.handle = CreateFile(path.string().c_str(), itrn::GetFileModeWindows(mode),
                FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (file.handle == INVALID_HANDLE_VALUE)
            {
                throw exc::CoreException("Failed to create a file handle: " + GetLastError());
                return false;
            }

            file.stat = GetFileSize(file.handle, NULL);
            if (file.stat = INVALID_FILE_SIZE)
            {
                throw exc::CoreException("Failed to get file size: " + GetLastError());
                CloseHandle(file.handle);
                return false;
            }

            return true;

        // Posix -------------------------
        #else

            file.handle = open(path.string().c_str(), itrn::GetFileModePosix(mode));
            if (file.handle == -1)
            {
                throw exc::CoreException("Failed to open a file");
                perror("Failed to open a file");
                return false;
            }

            if (std::filesystemtat(file.handle, &file.stat) == -1)
            {
                throw exc::CoreException("Failed to get file size");
                perror("Failed to get file size");
                close(file.handle);
                return false;
            }

            return true;

        #endif
        }

        inline bool Map(File& file)
        {
        // Windows -----------------------
        #if WINDOWS_PLATFORM
            return true;


        // Posix -------------------------
        #else
            return true;


        #endif      
        }
    }
}