#pragma once

#include <string>

namespace AIngine::Util::Filesystem {
	typedef enum {
		OKAY,
		ERR,
		CANCEL
	} Result;

	/* Opens a file dialog where a path to a file can be specified. 
	@param filter: seperate filter with ',', e.g. "txt,json".
	@param defaultPath: relative to the project resource directory */
	Result OpenFile(const char* filter, std::string* path, const char* defaultPath = nullptr);

	/* Opens a file dialog where a path to a file can be specified that will be saved on disk.
	@param filter: seperate filter with ',', e.g. "txt,json".
	@param defaultPath: relative to the project resource directory */
	Result SaveFile(const char* filter, std::string* path, const char* defaultPath = nullptr);
	
	/* Opens a file dialog where a path to a folder can be specified.
	@param defaultPath: relative to the project resource directory */
	Result PickFolder(std::string* path, const char* defaultPath = nullptr);
}