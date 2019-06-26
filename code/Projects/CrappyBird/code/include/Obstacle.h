#pragma once
#include "AIngine/Core.h"

namespace CrappyBird {
	class Obstacle : public AIngine::Script {
	public:
		Obstacle();
		// Inherited via Script
		virtual void OnStart() override;
		virtual void OnEnd() override;
		virtual void Update(float delta) override;
		virtual void OnEventData(AIngine::Events::EventData& e) override;

		inline AIngine::Structures::RectangleF& GetRectangle() { return m_rect; }
		void Set(const AIngine::Structures::RectangleF& rect);

		void Reset();

	private:
		AIngine::Structures::RectangleF m_rect;
		glm::vec2 m_spawnPosition;
	};
}