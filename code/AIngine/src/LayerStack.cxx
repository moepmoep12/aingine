#include "LayerStack.h"

AIngine::LayerStack::LayerStack()
{
	m_LayerInsert = m_Layers.begin();
}

AIngine::LayerStack::~LayerStack()
{
	for (Layer* layer : m_Layers)
		delete layer;
}

void AIngine::LayerStack::PushLayer(Layer * layer)
{
	m_LayerInsert = m_Layers.emplace(m_LayerInsert, layer);
}

void AIngine::LayerStack::PushOverlay(Layer * overlay)
{
	m_Layers.emplace_back(overlay);
}

void AIngine::LayerStack::PopLayer(Layer * layer)
{
	auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
	if (it != m_Layers.end())
	{
		m_Layers.erase(it);
		m_LayerInsert--;
	}
}

void AIngine::LayerStack::PopOverlay(Layer * layer)
{
	auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
	if (it != m_Layers.end())
		m_Layers.erase(it);
}
