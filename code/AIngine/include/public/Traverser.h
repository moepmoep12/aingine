#pragma
#include "GameObject.h"

namespace AIngine {

	class Traverser {
	public:

		virtual bool Traverse(GameObject* root) = 0;

		virtual bool Enter(GameObject& node) = 0;
		virtual bool Leave(GameObject& node) = 0;
		virtual bool Visit(GameObject& node) = 0;

		virtual ~Traverser() {}
	};
}