#include "Effects.h"
#include "CrappyBird.h"
#include "Player.h"
#include <sstream>
namespace CrappyBird {

	// copy constructor
	Effect::Effect(const Effect & other)
		: Texture(other.Texture), Duration(other.Duration), Age(other.Age), Color(other.Color), Rotation(other.Rotation)
	{
	}

	// move constructor
	Effect::Effect(Effect && other)
		: Texture(other.Texture), Duration(other.Duration), Age(other.Age), Color(other.Color), Rotation(other.Rotation)
	{
		other.Age = other.Duration;
	}

	// copy assignment
	Effect & Effect::operator=(const Effect & other)
	{
		if (&other != this) {
			Texture = other.Texture;
			Duration = other.Duration;
			Age = other.Age;
			Color = other.Color;
			Rotation = other.Rotation;
		}
		return *this;
	}

	// move assignment
	Effect & Effect::operator=(Effect && other)
	{
		if (&other != this) {
			Texture = other.Texture;
			Duration = other.Duration;
			Age = other.Age;
			Color = other.Color;
			Rotation = other.Rotation;
			other.Age = other.Duration;
		}
		return *this;
	}

	/*************************************** SLOW EFFECT ***********************************************************/

	SlowEffect::SlowEffect()
	{
		Texture = Texture2D(Assets::Load<BitmapAsset>("textures/fast-forward.png")->GetBitmap());
		Rotation = M_PI;
		Color = glm::vec4(1, 0, 1, 1);
		Duration = 5;
		m_effectDisplay = AIngine::World::GetGameObject("SlowEffect")->GetComponent<EffectDurationDisplay>();
	}

	bool SlowEffect::Start(Player * player)
	{
		SlowEffect* sloweffect = player->GetEffect<SlowEffect>();
		if (!sloweffect) {
			CrappyBird::s_GameSpeed -= SpeedDecrease;
			m_effectDisplay->Activate(Duration);
			return true;
		}
		else {
			sloweffect->Duration += Duration;
			m_effectDisplay->Remaining += Duration;
			return false;
		}
	}

	void SlowEffect::End()
	{
		CrappyBird::s_GameSpeed += SpeedDecrease;
		m_effectDisplay->GetOwner()->SetActive(false);
	}

	void SlowEffect::Undo()
	{
		End();
	}

	/*************************************** SPEED EFFECT ***********************************************************/

	SpeedEffect::SpeedEffect()
	{
		Texture = Texture2D(Assets::Load<BitmapAsset>("textures/fast-forward.png")->GetBitmap());
		Rotation = 0;
		Color = glm::vec4(1);
		Duration = 5;
		m_effectDisplay = AIngine::World::GetGameObject("SpeedEffect")->GetComponent<EffectDurationDisplay>();
	}


	bool SpeedEffect::Start(Player * player)
	{
		SpeedEffect* speedeffect = player->GetEffect<SpeedEffect>();
		if (!speedeffect) {
			CrappyBird::s_GameSpeed += SpeedIncrease;
			m_effectDisplay->Activate(Duration);
			return true;
		}
		else {
			speedeffect->Duration += Duration;
			m_effectDisplay->Remaining += Duration;
			return false;
		}
	}

	void SpeedEffect::End()
	{
		CrappyBird::s_GameSpeed -= SpeedIncrease;
		m_effectDisplay->GetOwner()->SetActive(false);
	}

	void SpeedEffect::Undo()
	{
		End();
	}

	/*************************************** SHRINK EFFECT ***********************************************************/

	static std::vector<glm::vec2> s_originalVertices;

	ShrinkEffect::ShrinkEffect()
	{
		Texture = Texture2D(Assets::Load<BitmapAsset>("textures/reduce.png")->GetBitmap());
		Rotation = 0;
		Color = glm::vec4(1);
		Duration = 5;
		m_effectDisplay = AIngine::World::GetGameObject("ShrinkEffect")->GetComponent<EffectDurationDisplay>();
	}

	bool ShrinkEffect::Start(Player * player)
	{
		ShrinkEffect* effect = player->GetEffect<ShrinkEffect>();
		if (!effect) {
			m_player = player;
			PhysicsComponent* physcomp = m_player->GetOwner()->GetComponent<PhysicsComponent>();
			if (s_originalVertices.size() == 0)
				s_originalVertices = m_player->GetOriginalPhysVertices();

			for (int i = 0; i < physcomp->GetBodyInformation().verticesCount; i++) {
				verticesScaleSpeed[i] = (destScale * s_originalVertices[i]) / animDuration;
			}

			m_effectDisplay->Activate(Duration);

			return true;
		}

		else {
			effect->Duration += Duration;
			m_effectDisplay->Remaining += Duration;
			return false;
		}
	}

	void ShrinkEffect::Update(float delta)
	{
		Effect::Update(delta);

		if (Age < animDuration) {
			m_player->GetOwner()->Scale(-scaleSpeed * delta);
			PhysicsComponent* physcomp = m_player->GetOwner()->GetComponent<PhysicsComponent>();
			std::vector<glm::vec2> vertices;

			for (int i = 0; i < physcomp->GetBodyInformation().verticesCount; i++) {
				vertices.push_back(physcomp->GetBodyInformation().vertices[i]);
				vertices[i] -= verticesScaleSpeed[i] * delta;
			}

			physcomp->AdjustPolyShape(&vertices[0], physcomp->GetBodyInformation().verticesCount);

		}
		if (Age > Duration - animDuration) {
			m_player->GetOwner()->Scale(scaleSpeed * delta);
			PhysicsComponent* physcomp = m_player->GetOwner()->GetComponent<PhysicsComponent>();
			std::vector<glm::vec2> vertices;

			for (int i = 0; i < physcomp->GetBodyInformation().verticesCount; i++) {
				vertices.push_back(physcomp->GetBodyInformation().vertices[i]);
				vertices[i] += verticesScaleSpeed[i] * delta;
			}

			physcomp->AdjustPolyShape(&vertices[0], physcomp->GetBodyInformation().verticesCount);
		}
	}

	void ShrinkEffect::End()
	{
		m_player->GetOwner()->SetLocalScale(glm::vec2(1));
		m_effectDisplay->GetOwner()->SetActive(false);
	}

	void ShrinkEffect::Undo()
	{
		m_effectDisplay->GetOwner()->SetActive(false);
		m_player->GetOwner()->SetLocalScale(glm::vec2(1));
		PhysicsComponent* physcomp = m_player->GetOwner()->GetComponent<PhysicsComponent>();
		physcomp->AdjustPolyShape(&s_originalVertices[0], physcomp->GetBodyInformation().verticesCount);
	}
}