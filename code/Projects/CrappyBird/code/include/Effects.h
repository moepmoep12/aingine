#pragma once
#include "AIngine/Core.h"
#include "EffectDurationDisplay.h"
namespace CrappyBird {

	class Player;

	class Effect {
	public:
		Effect() : Age(0), Duration(0), Rotation(0), Color(1), m_player(nullptr) {}
		Effect(const Effect& other);
		Effect(Effect&& other);
		Effect& operator=(const Effect& other);
		Effect& operator=(Effect&& other);
		virtual ~Effect() { Texture.~Texture2D(); }

		virtual bool Start(Player* player) { return true; }
		virtual void Update(float delta) {}
		virtual void End() {}
		virtual void Undo() {}

		float Age = 0;
		float Duration;

		Texture2D Texture;
		glm::vec4 Color = glm::vec4(1);
		float Rotation;
		EffectDurationDisplay* m_effectDisplay;

	protected:
		Player* m_player;
	};


	class SlowEffect : public Effect {
	public:
		SlowEffect();

		virtual bool Start(Player* player) override;
		virtual void End() override;
		virtual void Undo() override;

	private:
		float SpeedDecrease = 1.0f;
	};

	class SpeedEffect : public Effect {
	public:
		SpeedEffect();

		virtual bool Start(Player* player) override;
		virtual void End() override;
		virtual void Undo() override;

	private:
		float SpeedIncrease = 1.0f;
	};

	class ShrinkEffect : public Effect {
	public:
		ShrinkEffect();

		virtual bool Start(Player* player) override;
		virtual void Update(float delta) override;
		virtual void End() override;
		virtual void Undo() override;

	private:
		const float animDuration = 0.5f;
		const glm::vec2 destScale = glm::vec2(0.6f);
		const glm::vec2 scaleSpeed = destScale / animDuration;
		glm::vec2 verticesScaleSpeed[AIngine::Physics::maxVertices];
	};
}