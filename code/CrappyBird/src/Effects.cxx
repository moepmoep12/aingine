#include "Effects.h"
#include "CrappyBird.h"
#include "Player.h"

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
		Texture = Texture2D(Assets::Load<BitmapAsset>("CrappyBird/textures/fast-forward.png")->GetBitmap());
		Rotation = M_PI;
		Color = glm::vec4(1, 0, 1, 1);
		Duration = 5;
	}

	bool SlowEffect::Start(Player * player)
	{
		SlowEffect* sloweffect = player->GetEffect<SlowEffect>();
		if (!sloweffect) {
			CrappyBird::s_GameSpeed -= SpeedDecrease;
			return true;
		}
		else {
			sloweffect->Duration += Duration;
			return false;
		}
	}

	void SlowEffect::End()
	{
		CrappyBird::s_GameSpeed += SpeedDecrease;
	}

	/*************************************** SPEED EFFECT ***********************************************************/

	SpeedEffect::SpeedEffect()
	{
		Texture = Texture2D(Assets::Load<BitmapAsset>("CrappyBird/textures/fast-forward.png")->GetBitmap());
		Rotation = 0;
		Color = glm::vec4(1);
		Duration = 5;
	}


	bool SpeedEffect::Start(Player * player)
	{
		SpeedEffect* speedeffect = player->GetEffect<SpeedEffect>();
		if (!speedeffect) {
			CrappyBird::s_GameSpeed += SpeedIncrease;
			return true;
		}
		else {
			speedeffect->Duration += Duration;
			return false;
		}
	}

	void SpeedEffect::End()
	{
		CrappyBird::s_GameSpeed -= SpeedIncrease;
	}

	/*************************************** SHRINK EFFECT ***********************************************************/

	ShrinkEffect::ShrinkEffect()
	{
		Texture = Texture2D(Assets::Load<BitmapAsset>("CrappyBird/textures/reduce.png")->GetBitmap());
		Rotation = 0;
		Color = glm::vec4(1);
		Duration = 5;
	}

	bool ShrinkEffect::Start(Player * player)
	{
		ShrinkEffect* effect = player->GetEffect<ShrinkEffect>();
		if (!effect) {
			m_player = player;
			PhysicsComponent* physcomp = m_player->GetOwner()->GetComponent<PhysicsComponent>();
			static auto originalVertices = m_player->GetOriginalPhysVertices();

			for (int i = 0; i < physcomp->GetBodyInformation().verticesCount; i++) {
				verticesScaleSpeed[i] = ( destScale * originalVertices[i]) / animDuration;
			}

			return true;
		}

		else {
			effect->Duration += Duration;
			return false;
		}
	}

	void ShrinkEffect::Update(float delta)
	{
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
	}
}