#include "Assets/Assets.h"
#include <functional>

namespace AIngine::Assets {
	UID GetUID(std::string const & path) {
		static std::hash<std::string> hash;
		return hash(path);
	}
}