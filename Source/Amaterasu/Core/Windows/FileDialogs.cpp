#include "pch.h"

#include "Amaterasu/Core/FileDialogs.h"

#ifdef _WIN32
#include <shlobj.h>
#endif


namespace Amaterasu
{
	namespace FileDialogs
	{
		std::string OpenFile(const char* filter)
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


		std::string SaveFile(const char* filter)
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

		std::string OpenFolder(const char* filter)
		{
			bool success = false;
			
			std::wstring folderName;

			std::wstring dialogTitle = L"Testing123";

			IFileOpenDialog* fileDialog; // Unreal has a TComPtr<T> for this...  Why?
			if (SUCCEEDED(::CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&fileDialog))))
			{
				// Set this up as a folder picker
				{
					DWORD dwFlags = 0;
					fileDialog->GetOptions(&dwFlags);
					fileDialog->SetOptions(dwFlags | FOS_PICKFOLDERS);
				}

				// Set up common settings
				fileDialog->SetTitle(dialogTitle.c_str());
				/*if (!DefaultPath.IsEmpty())
				{
					// SHCreateItemFromParsingName requires the given path be absolute and use \ rather than / as our normalized paths do
					FString DefaultWindowsPath = FPaths::ConvertRelativePathToFull(DefaultPath);
					DefaultWindowsPath.ReplaceInline(TEXT("/"), TEXT("\\"), ESearchCase::CaseSensitive);

					TComPtr<IShellItem> DefaultPathItem;
					if (SUCCEEDED(::SHCreateItemFromParsingName(*DefaultWindowsPath, nullptr, IID_PPV_ARGS(&DefaultPathItem))))
					{
						FileDialog->SetFolder(DefaultPathItem);
					}
				}*/

				// Show the picker
				if (SUCCEEDED(fileDialog->Show(NULL/*(HWND)ParentWindowHandle*/)))
				{
					IShellItem* result; // Unreal uses TComPtr<IShellItem> here.
					if (SUCCEEDED(fileDialog->GetResult(&result)))
					{
						PWSTR pFilePath = nullptr;
						if (SUCCEEDED(result->GetDisplayName(SIGDN_FILESYSPATH, &pFilePath)))
						{
							success = true;

							folderName = pFilePath;
							// TODO: Normalize directory name
							
							::CoTaskMemFree(pFilePath);
						}
					}
				}
			}

			return "test";
		}
	}
}