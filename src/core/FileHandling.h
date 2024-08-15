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

namespace file
{
    class File; // main class for handling files, prefer this over FileStruct

    struct FileStruct;    // common struct for both windows and posix files, use with Open, Map, Close, GetContent
    enum class FileMode { READ, WRITE, READ_WRITE };
        
    // all these should be used together
    inline bool    Open(const std::filesystem::path& path, FileMode mode, FileStruct& file); // opens into "file" object
    inline bool    Map(FileStruct& file);   // file memory mapping
    inline bool    Close(FileStruct& file);
    inline String  GetContent(const FileStruct& file);
}

// -------------------------------------

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

    struct FileStruct
    {
        bool open = false;      // sets to true when successfully opened
        bool mapped = false;    // sets to true when successfully mapped

        FileMode mode;

        PLATFORM_TYPE(HANDLE, int)                  handle;
        PLATFORM_TYPE(LARGE_INTEGER,  struct stat)  stat;
        PLATFORM_TYPE(LPVOID, char*)                map;
        HANDLE                                      mappingWin;      // only used on windows, since there's a 2-step mapping process there
    };
    
    inline bool Open(const std::filesystem::path& path, FileMode mode, FileStruct& file) 
    {
    // Windows -----------------------
    #if WINDOWS_PLATFORM

        file.handle = CreateFile(path.string().c_str(), itrn::GetFileModeWindows(mode),
            0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        if (file.handle == INVALID_HANDLE_VALUE)
        {
            throw exc::CoreException("Failed to create a file handle with error: " + GetLastErrorStr());
            return false;
        }

        if (!GetFileSizeEx(file.handle, &file.stat))
        {
            throw exc::CoreException("Failed to get file size with error: " + GetLastErrorStr());
            CloseHandle(file.handle);
            return false;
        }

        if (file.stat.QuadPart == 0 && mode == FileMode::READ)
        {
            throw exc::CoreException("Opened file is empty");
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

        if (fstat(file.handle, &file.stat) == -1)
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

    inline bool Map(FileStruct& file)
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
            throw exc::CoreException("Failed to create file mapping with error: " + GetLastErrorStr());
            CloseHandle(file.handle);
            return false;
        }

        file.map = MapViewOfFile(file.mappingWin, itrn::GetMapAccessWindows(file.mode), 0, 0, 0);
        if (file.map == NULL)
        {
            throw exc::CoreException("Failed to map the view of the file with error: " + GetLastErrorStr());
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
            throw exc::CoreException("Failed to create file mapping: " + GetLastErrorStr());
            perror("Failed to create file mapping");
            close(file.handle);
            return false;
        }

        file.mapped = true;
        return true;

    #endif      
    }

    inline bool Close(FileStruct& file)
    {
        if (!file.open) return true;

    // Windows -----------------------
    #if WINDOWS_PLATFORM

        if (file.mapped)
        {
            UnmapViewOfFile(file.map);
            CloseHandle(file.mappingWin);
        }
        if(file.open) CloseHandle(file.handle);

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

    inline String GetContent(const FileStruct& file)
    {
        if (!file.open)
        {
            throw exc::CoreException("Unable to get file content: the file is not open");
            return String();
        }
        if (!file.mapped) 
        {
            throw exc::CoreException("Unable to get file content: the file is not mapped");
            return String();
        }

    // Windows -----------------------
    #if WINDOWS_PLATFORM

        //char* content = static_cast<char*>(file.map);
        return String(static_cast<char*>(file.map));

    // Posix -------------------------
    #else

        return String(file.map);

    #endif
    }

    // main class for handling files, prefer this over FileStruct
    class File
    {
    public:
        File() = default;
        ~File() { Close(); }

        bool Open(const std::filesystem::path& path, FileMode mode)
        {
        // Windows -----------------------
        #if WINDOWS_PLATFORM

            m_handle = CreateFile(path.string().c_str(), itrn::GetFileModeWindows(mode),
                0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
            if (m_handle == INVALID_HANDLE_VALUE)
            {
                throw exc::CoreException("Failed to create a file handle with error: " + GetLastErrorStr());
                return false;
            }

            if (!GetFileSizeEx(m_handle, &m_stat))
            {
                throw exc::CoreException("Failed to get file size with error: " + GetLastErrorStr());
                CloseHandle(m_handle);
                return false;
            }

            if (m_stat.QuadPart == 0 && mode == FileMode::READ)
            {
                throw exc::CoreException("Opened file is empty");
                CloseHandle(m_handle);
                return false;
            }

            m_mode = mode;
            m_open = true;
            return true;

        // Posix -------------------------
        #else

            m_handle = open(path.string().c_str(), itrn::GetFileModePosix(mode));
            if (m_handle == -1)
            {
                throw exc::CoreException("Failed to open a file");
                perror("Failed to open a file");
                return false;
            }

            if (fstat(m_handle, &m_stat) == -1)
            {
                throw exc::CoreException("Failed to get file size");
                perror("Failed to get file size");
                close(m_handle);
                return false;
            }

            m_mode = mode;
            m_open = true;
            return true;

        #endif
        }

        bool Map()
        {
        // Windows -----------------------
        #if WINDOWS_PLATFORM

            if (!m_open)
            {
                throw exc::CoreException("File mapping failed: requested file is not open");
                return false;
            }

            m_mappingWin = CreateFileMapping(m_handle, NULL, itrn::GetFileProtectWindows(m_mode), 0, 0, NULL);
            if (m_mappingWin == NULL)
            {
                throw exc::CoreException("Failed to create file mapping with error: " + GetLastErrorStr());
                CloseHandle(m_handle);
                return false;
            }

            m_map = MapViewOfFile(m_mappingWin, itrn::GetMapAccessWindows(m_mode), 0, 0, 0);
            if (m_map == NULL)
            {
                throw exc::CoreException("Failed to map the view of the file with error: " + GetLastErrorStr());
                CloseHandle(m_handle);
                return false;
            }

            m_mapped = true;
            return true;

        // Posix -------------------------
        #else
                
            if (!m_open)
            {
                throw exc::CoreException("File mapping failed: requested file is not open");
                return false;
            }

            m_map = static_cast<char*>(mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0));
            if (m_map = MAP_FAILED)
            {
                throw exc::CoreException("Failed to create file mapping: " + GetLastErrorStr());
                perror("Failed to create file mapping");
                close(m_handle);
                return false;
            }

            m_mapped = true;
            return true;

        #endif      
        }

        // called on destruction
        bool Close()
        {
            if (!m_open) return true;

        // Windows -----------------------
        #if WINDOWS_PLATFORM

            if (m_mapped)
            {
                UnmapViewOfFile(m_map);
                CloseHandle(m_mappingWin);
            }
            CloseHandle(m_handle);

            lg::Debug("Closed the file");
            return true;

        // Posix -------------------------
        #else

            if (munmap(m_map, m_stat.st_size) == -1)
            {
                throw exc::CoreException("Failed to unmap the file")
                    perror("Failed to unmap the file");
                return false;
            }
            close(fd);

            lg::Debug("Closed the file");
            return true;

        #endif

        }

        String GetContent()
        {
            if (!m_open)
            {
                throw exc::CoreException("Unable to get file content: the file is not open");
                return String();
            }
            if (!m_mapped)
            {
                throw exc::CoreException("Unable to get file content: the file is not mapped");
                return String();
            }

        // Windows -----------------------
        #if WINDOWS_PLATFORM

            return String(static_cast<char*>(m_map));

        // Posix -------------------------
        #else

            return String(m_map);

        #endif
        }

        String Read(const std::filesystem::path& path)
        {
            Open(path, FileMode::READ);
            Map();
            return GetContent();
        }

    protected:
        bool                                        m_open = false;
        bool                                        m_mapped = false;
        FileMode                                    m_mode;

        PLATFORM_TYPE(HANDLE, int)                  m_handle;
        PLATFORM_TYPE(LARGE_INTEGER, struct stat)   m_stat;
        PLATFORM_TYPE(LPVOID, char*)                m_map;
        HANDLE                                      m_mappingWin;      // only used on windows, since there's a 2-step mapping process 
    };
}
