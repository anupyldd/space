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
#include <sstream>

// API ---------------------------------

namespace spc
{
    namespace file
    {
        struct File;    // common struct for both windows and posix files
        enum class FileMode { READ, WRITE, READ_WRITE };
        
        inline bool    Open(const std::filesystem::path& path, FileMode mode, File& file); // opens into "file" object
        inline bool    Map(File& file);
        inline bool    Close(File& file);
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

            inline DWORD GetFileProtectWindows(FileMode mode)
            {
                switch (mode)
                {
                case FileMode::READ: return PAGE_READONLY;
                case FileMode::WRITE: 
                case FileMode::READ_WRITE: return PAGE_READWRITE;
                default: throw exc::CoreException("Unknown file access mode");
                }
            }

            inline DWORD GetMapAccessWindows(FileMode mode)
            {
                switch (mode)
                {
                case FileMode::READ: return FILE_MAP_READ;
                case FileMode::WRITE: return FILE_MAP_WRITE;
                case FileMode::READ_WRITE: return FILE_MAP_ALL_ACCESS;
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
            bool open = false;      // sets to true when successfully opened
            bool mapped = false;    // sets to true when successfully mapped

            FileMode mode;

            PLATFORM_TYPE(HANDLE, int)          handle;
            PLATFORM_TYPE(DWORD,  struct stat)  stat;
            PLATFORM_TYPE(LPVOID, char*)        map;
            HANDLE                              mappingWin;      // only used on windows, since there's a 2-step mapping process there
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

            file.mode = mode;
            file.open = true;
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

            file.mode = mode;
            file.open = true;
            return true;

        #endif
        }

        inline bool Map(File& file)
        {
        // Windows -----------------------
        #if WINDOWS_PLATFORM
            
            if (!file.open)
            {
                throw exc::CoreException("File mapping failed: requested file is not open");
                return false;
            }

            file.mappingWin = CreateFileMapping(file.handle, NULL, itrn::GetFileProtectWindows(file.mode), 0, 0, NULL);
            if (file.mappingWin == NULL)
            {
                throw exc::CoreException("Failed to create file mapping: " + GetLastError());
                CloseHandle(file.handle);
                return false;
            }

            file.map = MapViewOfFile(file.mappingWin, itrn::GetMapAccessWindows(file.mode), 0, 0, 0);
            if (file.map == NULL)
            {
                throw exc::CoreException("Failed to map the view of the file: " + GetLastError());
                CloseHandle(file.handle);
                return false;
            }

            file.mapped = true;
            return true;

        // Posix -------------------------
        #else

            if (!file.open)
            {
                throw exc::CoreException("File mapping failed: requested file is not open");
                return false;
            }

            file.map = static_cast<char*>(mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0));
            if (file.map = MAP_FAILED)
            {
                throw exc::CoreException("Failed to create file mapping: " + GetLastError());
                perror("Failed to create file mapping");
                close(file.handle);
                return false;
            }

            file.mapped = true;
            return true;

        #endif      
        }

        inline bool Close(File& file)
        {
            if (!file.open) return true;

        // Windows -----------------------
        #if WINDOWS_PLATFORM

            UnmapViewOfFile(file.map);
            CloseHandle(file.mappingWin);
            CloseHandle(file.handle);
            return true;

        // Posix -------------------------
        #else

            if (munmap(file.map, file.stat.st_size) == -1) 
            {
                throw exc::CoreException("Failed to unmap the file")
                perror("Failed to unmap the file");
                return false;
            }
            close(fd);
            return true;

        #endif
        }

        inline String GetContent(const File& file)
        {
        // Windows -----------------------
        #if WINDOWS_PLATFORM

            //char* content = static_cast<char*>(file.map);
            return String(static_cast<char*>(file.map));

        // Posix -------------------------
        #else

            return String(file.map);

        #endif
        }
    }
}