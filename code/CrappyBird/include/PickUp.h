#pragma once
#include "AIngine/Core.h"

#include <vector>

namespace CrappyBird {


	class PickUp : public Script {
	public:
		PickUp() { SetName(typeid(*this).name()); }
		virtual void OnStart() override;
		virtual void Update(float deltatime) override;

		virtual void OnPickUp() {}

		virtual void Start() {}

	public:
		bool Active = false;
		bool Available = true;

	protected:
		void OnFinish();
		void ResetSprite();

	private:
		void OnCollision(PhysicsComponent* other);
		AIngine::Events::EventHandler<void, PhysicsComponent*> m_collisionHandler;
	};

	class PickUpFactory : public Script {
	public:
		PickUpFactory();
		virtual void OnStart() override;
		virtual void OnEnd() override;
		//void SpawnPickUpInArea(const AIngine::Structures::RectangleI& worldRect);

		void SpawnPickUp(const glm::vec2& pos);

	private:
		//AIngine::Events::EventHandler<void, AIngine::Structures::RectangleI&> m_newScreenHandler;

		GameObject* GetAvailableGameObject();
		void AddPickUpScript(GameObject& obj);
	};

	class SpeedPickUp : public PickUp {
	public:
		SpeedPickUp() { SetName(typeid(*this).name()); }
		virtual void OnPickUp() override;
		virtual void Update(float deltatime) override;
		virtual void Start() override;

		float Duration = 5;
	};

	class SlowSpeedPickUp : public PickUp {
	public:
		SlowSpeedPickUp() { SetName(typeid(*this).name()); }
		virtual void OnPickUp() override;
		virtual void Update(float deltatime) override;
		virtual void Start() override;


		float Duration = 5;

	private:
		float m_value = 1.5f;
		float m_appliedReduction = 0;
	};


	class ShrinkPickUp : public PickUp {
	public:
		ShrinkPickUp() { SetName(typeid(*this).name()); }
		virtual void OnPickUp() override;
		virtual void Update(float deltatime) override;
		virtual void Start() override;

		float AnimDuration = 2;
		float EffectDuration = 3;

	private:
		float m_currentEffectDuration = 0;
		float m_currentLerpTime = 0;
		float direction = 1;

	};
}