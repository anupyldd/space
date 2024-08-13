#include "Localization.h"

namespace eng
{
    namespace loc
    {
        void Localization::LoadFiles(std::initializer_list<std::filesystem::path> paths)
        {
            for (const auto& path : paths)
            {
                try
                {
                    if (m_loadedLocFiles.contains(path.filename().string()))
                    {
                        throw exc::EngineException("Localization file with this name is already loaded");
                    }
                    else
                    {
                        if (m_loadedLocFiles[path.filename().string()].m_file.Open(path, file::FileMode::READ))
                            m_loadedLocFiles.at(path.filename().string()).m_file.Map();
                    }
                }
                catch (const exc::IException& e)
                {
                    lg::Error(std::format("{}\n          When trying to load {}", e.What(), path.string()));
                    m_loadedLocFiles.erase(path.filename().string());
                }
            }
        }

        void Localization::UnloadFiles(std::initializer_list<String> fileNames)
        {
            for (const auto& name : fileNames)
            {
                try
                {
                    if (m_loadedLocFiles.contains(name))
                        m_loadedLocFiles.erase(name);
                    else
                        throw exc::EngineException(std::format("Failed to unload a localization file (check if the specified file name is correct: {})", name));
                }
                catch (const exc::IException& e)
                {
                    lg::Error(e.What());
                }
            }
        }

        void Localization::UnloadFilesAll()
        {
            m_loadedLocFiles.clear();   // does clear call dtors? hope it does. TODO: check
        }

        void Localization::CreateFileIndex(std::initializer_list<String> fileNames)
        {
            // TODO: decide on file format
        }

        void Localization::LoadFileIntoMap(const String& fileName, std::unordered_map<String, String>& map)
        {

        }

        String Localization::GetStrByTag(const String& tag) const
        {
            return String();
        }

        String Localization::GetFileContents(const String& fileName)
        {
            return m_loadedLocFiles.at(fileName).m_file.GetContent();
        }

        uint16_t Localization::GetLoadedFilesNum() const
        {
            return m_loadedLocFiles.size();
        }

        void Localization::SetLanguage(Language lang)
        {
            m_gameLang = lang;
            // TODO: save new lang to config
        }

        Language Localization::GetLanguage() const
        {
            return m_gameLang;
        }

        void Localization::m_ParseFile()
        {

        }
    }
}