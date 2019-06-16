#pragma once
#include "AIngine/Core.h"

namespace CrappyBird {
	class Obstacle :public Script {
	public:
		Obstacle() { SetName(typeid(*this).name()); }
		virtual void OnStart() override;

		inline AIngine::Structures::RectangleF& GetRectangle() { return m_rect; }
		void Set(const AIngine::Structures::RectangleF& rect);

	private:
		AIngine::Structures::RectangleF m_rect;
	};

}