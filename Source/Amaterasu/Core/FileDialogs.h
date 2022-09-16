#pragma once

//#include "Filesystem/Path.h"

#include <string>
#include <filesystem>

namespace Amaterasu
{
    namespace FileDialogs
    {
        enum EOpenDirectoryDialogFlags
        {

        };

    	std::string OpenFileDialog(const char* filter);
	    std::string SaveFileDialog(const char* filter);
        bool OpenDirectoryDialog(const void* windowHandle, const std::wstring& dialogTitle, const std::filesystem::path& defaultPath, std::filesystem::path& directoryPath);
    }
}