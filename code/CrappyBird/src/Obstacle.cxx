#include "Obstacle.h"

namespace CrappyBird {
	void Obstacle::OnStart()
	{
		glm::vec2 pos = GetOwner()->GetWorldPosition();
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
	}
}