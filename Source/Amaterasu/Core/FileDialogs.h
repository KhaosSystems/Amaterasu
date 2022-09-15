#pragma once

#include <string>

namespace Amaterasu
{
    namespace FileDialogs
    {
    	std::string OpenFile(const char* filter);
	    std::string SaveFile(const char* filter);
        std::string OpenFolder(const char* filter);
    }
}