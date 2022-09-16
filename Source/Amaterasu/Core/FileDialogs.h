#pragma once

#include <string>

namespace Amaterasu
{
    namespace FileDialogs
    {
        enum EFileDialogFlags
        {

        };

    	std::string OpenFileDialog(const char* filter);
	    std::string SaveFileDialog(const char* filter);
        bool OpenDirectoryDialog(const void* windowHandle);
    }
}