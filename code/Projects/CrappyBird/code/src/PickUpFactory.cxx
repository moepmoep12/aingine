#include "PickUpFactory.h"

#include <random>
#include <time.h>

namespace CrappyBird {

	// Constructor
	PickUpFactory::PickUpFactory()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());
		srand(time(NULL));
	}

	void PickUpFactory::SpawnPickUp(const glm::vec2 & pos)
	{
		GameObject* availableObj = GetAvailableGameObject();
		if (availableObj) {
			availableObj->SetLocalPosition(pos);
			AddEffect(*availableObj->GetComponent<PickUp>());
			availableObj->SetActive(true);
		}
	}

	GameObject* PickUpFactory::GetAvailableGameObject()
	{
		for (auto& child : GetOwner()->GetChildren()) {
			if (!child->IsActive())
				return child;
		}
		return nullptr;
	}

	void PickUpFactory::AddEffect(PickUp & obj)
	{
		int value = rand() % 10;

		switch (value) {
		case 0:
			obj.m_Effect = std::make_unique<SpeedEffect>();
			break;
		case 1:
			obj.m_Effect = std::make_unique<SpeedEffect>();
			break;
		case 2:
			obj.m_Effect = std::make_unique<SpeedEffect>();
			break;
		case 3:
			obj.m_Effect = std::make_unique<SpeedEffect>();
			break;

		case 4:
			obj.m_Effect = std::make_unique<SlowEffect>();
			break;
		case 5:
			obj.m_Effect = std::make_unique<SlowEffect>();
			break;

		case 6:
			obj.m_Effect = std::make_unique<ShrinkEffect>();
			break;
		case 7:
			obj.m_Effect = std::make_unique<ShrinkEffect>();
			break;
		case 8:
			obj.m_Effect = std::make_unique<ShrinkEffect>();
			break;
		case 9:
			obj.m_Effect = std::make_unique<ShrinkEffect>();
			break;
		}

		Sprite* sprite = obj.GetOwner()->GetComponent<Sprite>();
		sprite->SetTexture(obj.m_Effect->Texture);
		sprite->SetColor(obj.m_Effect->Color);
		obj.GetOwner()->SetRotation(obj.m_Effect->Rotation);
	}
}