#include "Util/FileSystem.h"
#include "Util/Project.h"

#include <nfd.h>
#include <filesystem>

namespace AIngine::Util::Filesystem {
	Result OpenFile(const char * filter, std::string * path, const char * defaultPath)
	{
		std::string absoluteDefaultPath;
		if (defaultPath) {
			static const std::string resourceDir = Project::GetResourceDirectory();

			try {
				absoluteDefaultPath = std::filesystem::canonical(resourceDir + defaultPath).string();
			}
			catch (const std::exception& e) {
				absoluteDefaultPath = "";
			}
		}

		nfdchar_t *outPath = NULL;
		nfdresult_t result = NFD_OpenDialog(filter, defaultPath != nullptr && !absoluteDefaultPath.empty() ? absoluteDefaultPath.c_str() : NULL, &outPath);

		switch (result) {
		case NFD_OKAY:
			*path = outPath;
			free(outPath);
			return Result::OKAY;
		case NFD_CANCEL:
			return Result::CANCEL;
		case NFD_ERROR:
			return Result::ERR;
		default:
			return  Result::CANCEL;
		}
	}

	Result SaveFile(const char * filter, std::string * path, const char * defaultPath)
	{
		std::string absoluteDefaultPath;
		if (defaultPath) {
			static const std::string resourceDir = Project::GetResourceDirectory();

			try {
				absoluteDefaultPath = std::filesystem::canonical(resourceDir + defaultPath).string();
			}
			catch (const std::exception& e) {
				absoluteDefaultPath = "";
			}
		}

		nfdchar_t *outPath = NULL;
		nfdresult_t result = NFD_SaveDialog(filter, defaultPath != nullptr && !absoluteDefaultPath.empty() ? absoluteDefaultPath.c_str() : NULL, &outPath);

		switch (result) {
		case NFD_OKAY:
			*path = outPath;
			free(outPath);
			return Result::OKAY;
		case NFD_CANCEL:
			return Result::CANCEL;
		case NFD_ERROR:
			return Result::ERR;
		default:
			return  Result::CANCEL;
		}
	}

	Result PickFolder(std::string * path, const char * defaultPath)
	{
		std::string absoluteDefaultPath;
		if (defaultPath) {
			static const std::string resourceDir = Project::GetResourceDirectory();

			try {
				absoluteDefaultPath = std::filesystem::canonical(resourceDir + defaultPath).string();
			}
			catch (const std::exception& e) {
				absoluteDefaultPath = "";
			}
		}

		nfdchar_t *outPath = NULL;
		nfdresult_t result = NFD_PickFolder(defaultPath != nullptr && !absoluteDefaultPath.empty() ? absoluteDefaultPath.c_str() : NULL, &outPath);

		switch (result) {
		case NFD_OKAY:
			*path = outPath;
			free(outPath);
			return Result::OKAY;
		case NFD_CANCEL:
			return Result::CANCEL;
		case NFD_ERROR:
			return Result::ERR;
		default:
			return  Result::CANCEL;
		}
	}
}