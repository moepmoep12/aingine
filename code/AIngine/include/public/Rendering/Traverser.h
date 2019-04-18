#pragma
#include "SceneNode.h"

namespace AIngine::Rendering {

	class Traverser {
	public:
		virtual bool Enter(GroupNode& node) = 0;
		virtual bool Leave(GroupNode& node) = 0;

		//virtual bool Visit(SceneNode& node) = 0;
		virtual bool Visit(ShapeNode& node) = 0;
	};


}