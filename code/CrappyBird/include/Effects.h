#pragma once
#include "AIngine/Core.h"

namespace CrappyBird {

	class Player;

	class Effect {
	public:
		Effect() : Age(0), Duration(0), Rotation(0), Color(1) {}
		Effect(const Effect& other);
		Effect(Effect&& other);
		Effect& operator=(const Effect& other);
		Effect& operator=(Effect&& other);
		virtual ~Effect() {}

		virtual bool Start(Player* player) { return true; }
		virtual void Update(float delta) {}
		virtual void End() {}

		float Age = 0;
		float Duration;

		Texture2D Texture;
		glm::vec4 Color = glm::vec4(1);
		float Rotation;
	};


	class SlowEffect : public Effect {
	public:
		SlowEffect();

		virtual bool Start(Player* player) override;
		virtual void End() override;

	private:
		float SpeedDecrease = 1.0f;
	};

	class SpeedEffect : public Effect {
	public:
		SpeedEffect();

		virtual bool Start(Player* player) override;
		virtual void End() override;

	private:
		float SpeedIncrease = 1.0f;
	};
}