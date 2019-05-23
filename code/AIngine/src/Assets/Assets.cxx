#include "Assets/Assets.h"
#include "AIngine/Macros.h"
#include <functional>

namespace AIngine::Assets {

	AssetRegistry* Assets::AssetRegistry::s_instance = nullptr;

	UID GetUID(std::string const & path) {
		static std::hash<std::string> hash;
		return hash(path);
	}

	AssetRegistry::AssetRegistry()
	{
		ASSERT(!s_instance, "There's already an instance of AssetRegistry!");
		s_instance = this;
	}
}