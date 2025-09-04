/* SPDX-License-Identifier: GPL-2.0-or-later */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "pluginloader.h"

PluginsLoaded LoadPluginsFromDirectory(LPCWSTR directoryPath)
{
	HANDLE hFind;
	WIN32_FIND_DATAW findData;
	WCHAR searchPath[MAX_PATH];
	PluginsLoaded plugins = {0};	

	wsprintfW(searchPath, L"%s\\*.dll", directoryPath);

	hFind = FindFirstFileW(searchPath, &findData);

	if(hFind == INVALID_HANDLE_VALUE)
		return plugins;

	do
	{
		if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue;

		WCHAR fullDllPath[MAX_PATH];
		wsprintfW(fullDllPath, L"%s\\%s", directoryPath, findData.cFileName);
		
		HMODULE hMod = LoadLibraryW(fullDllPath);

		if(hMod && plugins.count < 128)
			plugins.hModules[plugins.count++] = hMod;
	}
	while(FindNextFileW(hFind, &findData));

	FindClose(hFind);

	return plugins;
}