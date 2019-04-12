#include "LayerStack.h"
#include "log.h"

AIngine::LayerStack::LayerStack()
{
	CORE_INFO("Creating LayerStack");
}

AIngine::LayerStack::~LayerStack()
{
	CORE_INFO("Destructor LayerStack");

	for (Layer* layer : m_Layers)
	{
		CORE_INFO("Deleting layer " + layer->GetName());
		delete layer;
	}
}

void AIngine::LayerStack::PushLayer(Layer * layer)
{
	m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
	m_LayerInsertIndex++;
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
		m_LayerInsertIndex--;
	}
}

void AIngine::LayerStack::PopOverlay(Layer * layer)
{
	auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
	if (it != m_Layers.end())
		m_Layers.erase(it);
}
