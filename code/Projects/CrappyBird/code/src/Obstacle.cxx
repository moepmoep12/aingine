#include "Obstacle.h"
namespace CrappyBird {

	// Constructor
	Obstacle::Obstacle()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());
	}

	// Start is called when gameplay starts for this script
	void Obstacle::OnStart()
	{
		glm::vec2 pos = GetOwner()->GetWorldPosition();
		m_spawnPosition = pos;
		glm::vec2 size = GetOwner()->GetComponent<Sprite>()->GetLocalWorldSize();
		pos -= size;
		m_rect.x = pos.x;
		m_rect.y = pos.y;
		m_rect.width = size.x;
		m_rect.height = size.y;
	}

	void Obstacle::Set(const AIngine::Structures::RectangleF & rect)
	{
		GetOwner()->SetLocalPosition(rect.GetCenter());
		GetOwner()->GetComponent<Sprite>()->SetLocalWorldSize(glm::vec2(rect.width, rect.height));
		GetOwner()->GetComponent<PhysicsComponent>()->AdjustBoxShape(rect.width, rect.height);
		m_rect = rect;
	}

	void Obstacle::Reset()
	{
		GetOwner()->SetWorldPosition(m_spawnPosition);
	}

	// End is called when gameplay ends for this script
	void Obstacle::OnEnd()
	{
	}

	// Update is called once per frame
	void Obstacle::Update(float delta)
	{
	}

	// Callback for events
	void Obstacle::OnEventData(AIngine::Events::EventData & e)
	{
	}
}