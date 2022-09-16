#include "pch.h"

#include "Amaterasu/Core/FileDialogs.h"

#ifdef _WIN32
#include <shlobj.h>
#endif


namespace Amaterasu
{
	namespace FileDialogs
	{
		std::string OpenFileDialog(const char* filter)
		{
			OPENFILENAMEA ofn;
			CHAR szFile[260] = { 0 };
			CHAR currentDir[256] = { 0 };
			ZeroMemory(&ofn, sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = NULL;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = sizeof(szFile);
			if (GetCurrentDirectoryA(256, currentDir))
				ofn.lpstrInitialDir = currentDir;
			ofn.lpstrFilter = filter;
			ofn.nFilterIndex = 1;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

			if (GetOpenFileNameA(&ofn) == TRUE)
				return ofn.lpstrFile;
			return std::string();
		}


		std::string SaveFileDialog(const char* filter)
		{
			OPENFILENAMEA ofn;
			CHAR szFile[260] = { 0 };
			CHAR currentDir[256] = { 0 };
			ZeroMemory(&ofn, sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = NULL;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = sizeof(szFile);
			if (GetCurrentDirectoryA(256, currentDir))
				ofn.lpstrInitialDir = currentDir;
			ofn.lpstrFilter = filter;
			ofn.nFilterIndex = 1;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

			// Sets the default extension by extracting it from the filter
			ofn.lpstrDefExt = strchr(filter, '\0') + 1;

			if (GetSaveFileNameA(&ofn) == TRUE)
				return ofn.lpstrFile;
			return std::string();
		}
		
		bool OpenDirectoryDialog(const void* windowHandle, const std::string& dialogTitle, const std::filesystem::path& defaultPath, std::filesystem::path& directoryPath)
		{
			bool success = false;

			IFileOpenDialog* fileDialog;
			if (SUCCEEDED(::CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&fileDialog))))
			{
				// Set this up as a folder picker
				{
					DWORD dwFlags = 0;
					fileDialog->GetOptions(&dwFlags);
					fileDialog->SetOptions(dwFlags | FOS_PICKFOLDERS);
				}

				// Set up common settings
				fileDialog->SetTitle(*dialogTitle);
				if (std::filesystem::exists(defaultPath))
				{
					// SHCreateItemFromParsingName requires the given path be absolute and use \ rather than / as our normalized paths do
					std::filesystem::path defaultWindowsPath = std::filesystem::absolute(defaultPath);
					defaultWindowsPath.make_preferred();

					IShellItem* DefaultPathItem;
					if (SUCCEEDED(::SHCreateItemFromParsingName(*defaultWindowsPath.wstring(), nullptr, IID_PPV_ARGS(&DefaultPathItem))))
					{
						fileDialog->SetFolder(DefaultPathItem);
					}
				}

				// Show the picker
				if (SUCCEEDED(fileDialog->Show((HWND)windowHandle)))
				{
					IShellItem* result;
					if (SUCCEEDED(fileDialog->GetResult(&result)))
					{
						PWSTR filePath = nullptr;
						if (SUCCEEDED(result->GetDisplayName(SIGDN_FILESYSPATH, &filePath)))
						{
							success = true;

							/// TODO
							directoryPath = filePath;

							::CoTaskMemFree(filePath);
						}
					}
				}
			}

			return success;
		}
	}
}