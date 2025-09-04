#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

typedef struct
{
	HMODULE hModules[128];
	int count;
}
PluginsLoaded;

PluginsLoaded LoadPluginsFromDirectory(LPCWSTR directoryPath);

#endif
