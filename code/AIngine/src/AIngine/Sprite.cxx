#include "AIngine/Sprite.h"

#include <memory>

namespace AIngine {

	Sprite::~Sprite()
	{
	}

	Component * Sprite::Copy(GameObject* const owner) const
	{
		Sprite* copy = new Sprite();
		copy->m_owner = owner;
		copy->m_texture = m_texture;
		copy->m_localWorldSize = m_localWorldSize;
		copy->m_parallaxFactor = m_parallaxFactor;
		copy->m_color = m_color;
		copy->m_alpha = m_alpha;
		return std::move(copy);
	}
}